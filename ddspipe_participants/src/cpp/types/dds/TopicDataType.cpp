// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fastcdr/CdrSizeCalculator.hpp>

#include <cpp_utils/Log.hpp>

#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>
#include <fastdds/dds/xtypes/dynamic_types/DynamicPubSubType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObjectUtils.hpp>

#include <ddspipe_participants/types/dds/TopicDataType.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

using eprosima::ddspipe::core::types::operator <<;

TopicDataType::TopicDataType(
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        const std::string& type_name,
        const fastdds::dds::xtypes::TypeIdentifierPair& type_identifiers,
        const bool keyed)
    : payload_pool_(payload_pool)
    , type_name_(type_name)
    , keyed_(keyed)
{
    // Set topic data
    max_serialized_type_size = 4;
    is_compute_key_provided = keyed_;

    // Set name
    set_name(type_name_.c_str());

    // Set Type Identifiers
    type_identifiers_ = type_identifiers;

    // Eagerly try to build the dynamic type for keyed topics
    // If it fails (e.g. dependency types missing from registry), disable key computation
    // so the DataWriter does not drop data on the first write attempt
    if (keyed_ && !initialize_dynamic_type_())
    {
        EPROSIMA_LOG_WARNING(DDSPIPE_DDS_TYPESUPPORT,
                "Cannot build dynamic type for keyed topic "
                << type_name_ << ". Key computation will be disabled for this topic.");
        is_compute_key_provided = false;
    }
}

TopicDataType::~TopicDataType()
{
    // Do nothing
}

bool TopicDataType::serialize(
        const void* data,
        fastdds::rtps::SerializedPayload_t& target_payload,
        fastdds::dds::DataRepresentationId_t data_representation)
{
    const DataType* src_payload = static_cast<const DataType*>(data);

    logDebug(DDSPIPE_DDS_TYPESUPPORT, "Serializing data " << *src_payload << ".");

    if (src_payload->payload_owner == payload_pool_.get())
    {
        // The src and dst Payload Pools are the same. The payload can be referenced.
        // We do not call get_payload since Fast-DDS doesn't call release_payload internally.
        // If we did, there would be leaks.
        target_payload.data = src_payload->payload.data;
    }
    else
    {
        EPROSIMA_LOG_WARNING(DDSPIPE_DDS_TYPESUPPORT, "Copying the payload between two different payload pools.");

        // The src and dst Payload Pools are different. The payload must be copied.
        target_payload.copy(&src_payload->payload);
    }

    return true;
}

bool TopicDataType::deserialize(
        fastdds::rtps::SerializedPayload_t& src_payload,
        void* data)
{
    logDebug(DDSPIPE_DDS_TYPESUPPORT, "Deserializing data " << src_payload << ".");

    DataType* target_payload = static_cast<DataType*>(data);

    // Get data and store it in PayloadPool
    payload_pool_->get_payload(src_payload, target_payload->payload);

    return true;
}

uint32_t TopicDataType::calculate_serialized_size(
        const void* const data,
        fastdds::dds::DataRepresentationId_t /* data_representation */)
{
    const auto p = static_cast<const DataType*>(data);
    return p->payload.length;
}

bool TopicDataType::compute_key(
        fastdds::rtps::SerializedPayload_t& payload,
        fastdds::rtps::InstanceHandle_t& handle,
        bool force_md5)
{
    if (!is_compute_key_provided)
    {
        return false;
    }

    if (!initialize_dynamic_type_())
    {
        // Dynamic type could not be built (missing dependency types in registry)
        // Disable key computation so data is not dropped and future writes skip this path
        EPROSIMA_LOG_WARNING(DDSPIPE_DDS_TYPESUPPORT,
                "Cannot compute key for type "
                << type_name_ << ": dynamic type unavailable. Disabling key computation.");
        is_compute_key_provided = false;
        return false;
    }

    fastdds::dds::DynamicPubSubType pub_sub_type(dynamic_type_);
    return pub_sub_type.compute_key(payload, handle, force_md5);
}

bool TopicDataType::compute_key(
        const void* const data,
        fastdds::rtps::InstanceHandle_t& handle,
        bool force_md5)
{
    if (is_compute_key_provided)
    {
        const auto p = static_cast<const DataType*>(data);

        // Fast path: key already available in routing data.
        if (p->instanceHandle.isDefined())
        {
            handle = p->instanceHandle;
            return true;
        }

        // Fallback path: compute key from serialized payload when handle is not present.
        if (p->payload.data != nullptr && p->payload.length > 0)
        {
            fastdds::rtps::SerializedPayload_t payload(p->payload.length);
            if (!payload.copy(&p->payload))
            {
                return false;
            }

            return compute_key(payload, handle, force_md5);
        }

        return false;
    }

    return false;
}

bool TopicDataType::initialize_dynamic_type_() const
{
    // Double-check pattern less lock overhead
    if (dynamic_type_)
    {
        return true;
    }

    if (dynamic_type_init_failed_)
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(dynamic_type_mtx_);

    // Handles the race where two threads both pas the first check,
    // then one acquires the lock and sets dynamic_type_init_failed_ = true
    // Without this second check, the other thread would proceed to retry initialization
    // after the first thread already determined its a permanent failure
    if (dynamic_type_)
    {
        return true;
    }

    if (dynamic_type_init_failed_)
    {
        return false;
    }

    fastdds::dds::xtypes::TypeInformation type_information;

    auto& registry = fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry();

    auto try_get_type_information = [&](const fastdds::dds::xtypes::TypeIdentifierPair& identifiers) -> bool
            {
                return fastdds::dds::RETCODE_OK == registry.get_type_information(identifiers, type_information);
            };

    if (!try_get_type_information(type_identifiers_))
    {
        fastdds::dds::xtypes::TypeIdentifierPair complete_only;
        complete_only.type_identifier1(type_identifiers_.type_identifier1());

        if (!try_get_type_information(complete_only))
        {
            fastdds::dds::xtypes::TypeIdentifierPair minimal_only;
            minimal_only.type_identifier2(type_identifiers_.type_identifier2());

            if (!try_get_type_information(minimal_only))
            {
                dynamic_type_init_failed_ = true;
                return false;
            }
        }
    }

    const auto complete_type_identifier = type_information.complete().typeid_with_size().type_id();
    fastdds::dds::xtypes::TypeObject type_object;
    if (fastdds::dds::RETCODE_OK != registry.get_type_object(complete_type_identifier, type_object))
    {
        dynamic_type_init_failed_ = true;
        return false;
    }

    // Pre-check consistency before calling create_type_w_type_object
    // If dependencies are missing from the registry, skip type creation entirely
    // to avoid noisy error logs from internal FastDDS type creation functions
    try
    {
        fastdds::dds::xtypes::TypeObjectUtils::type_object_consistency(type_object);
    }
    catch (const fastdds::dds::xtypes::InvalidArgumentError&)
    {
        EPROSIMA_LOG_WARNING(DDSPIPE_DDS_TYPESUPPORT,
                "TypeObject for "
                << type_name_ << " has unresolvable dependencies. "
                << "Skipping dynamic type creation.");
        dynamic_type_init_failed_ = true;
        return false;
    }

    auto dyn_type_builder = fastdds::dds::DynamicTypeBuilderFactory::get_instance()->create_type_w_type_object(
        type_object);
    if (!dyn_type_builder)
    {
        dynamic_type_init_failed_ = true;
        return false;
    }

    dynamic_type_ = dyn_type_builder->build();
    if (!dynamic_type_)
    {
        dynamic_type_init_failed_ = true;
    }
    return dynamic_type_ != nullptr;
}

void* TopicDataType::create_data()
{
    return reinterpret_cast<void*>(new DataType());
}

void TopicDataType::delete_data(
        void* data)
{
    delete(reinterpret_cast<DataType*>(data));
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
