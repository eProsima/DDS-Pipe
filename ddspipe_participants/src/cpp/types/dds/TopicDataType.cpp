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
        bool /* = false */)
{
    // NOTE: This method returns false because Fast DDS always sends the KEY_HASH in inline QoS.
    // As a result, the reader will never call this method when communicating with a Fast DDS writer.
    // This would only be needed if receiving data from another DDS vendor that omits the KEY_HASH.
    // Workaround in that case (different DDS vendor that omits KEY_HASH): set expects_inline_qos_ to
    // true in DataReaderQos
    return false;
}

bool TopicDataType::compute_key(
        const void* const data,
        fastdds::rtps::InstanceHandle_t& handle,
        bool /* = false */)
{
    if (is_compute_key_provided)
    {
        // Load the instanceHandle from data into handle
        const auto p = static_cast<const DataType*>(data);
        handle = p->instanceHandle;
        return true;
    }

    return false;
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
