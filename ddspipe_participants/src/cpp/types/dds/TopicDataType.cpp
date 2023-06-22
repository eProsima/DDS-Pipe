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
        const std::string& type_name,
        const bool keyed,
        const std::shared_ptr<core::PayloadPool>& payload_pool)
    : type_name_(type_name)
    , keyed_(keyed)
    , payload_pool_(payload_pool)
{
    // Set topic data
    m_typeSize = 4;
    m_isGetKeyDefined = keyed_;
    auto_fill_type_object(false);
    auto_fill_type_information(false);

    // Set name
    setName(type_name_.c_str());
}

TopicDataType::~TopicDataType()
{
    // Do nothing
}

bool TopicDataType::serialize(
        void* data,
        fastrtps::rtps::SerializedPayload_t* target_payload)
{
    DataType* src_payload = static_cast<DataType*>(data);

    logDebug(DDSPIPE_DDS_TYPESUPPORT, "Serializing data " << *src_payload << ".");

    // TODO: this could be done when we have access to Fast DDS PayloadPool
    // Copy each variable
    target_payload->copy(&src_payload->payload);

    return true;
}

bool TopicDataType::deserialize(
        eprosima::fastrtps::rtps::SerializedPayload_t* src_payload,
        void* data)
{
    logDebug(DDSPIPE_DDS_TYPESUPPORT, "Deserializing data " << *src_payload << ".");

    DataType* target_payload = static_cast<DataType*>(data);

    // Get data and store it in PayloadPool
    eprosima::fastrtps::rtps::IPayloadPool* _ = nullptr;
    payload_pool_->get_payload(*src_payload, _, target_payload->payload);
    target_payload->payload_owner = payload_pool_.get();

    return true;
}

std::function<uint32_t()> TopicDataType::getSerializedSizeProvider(
        void* data)
{
    return [data]() -> uint32_t
           {
               auto p = static_cast<DataType*>(data);
               return p->payload.length;
           };
}

bool TopicDataType::getKey(
        void*,
        eprosima::fastrtps::rtps::InstanceHandle_t*,
        bool /* = false */)
{
    if (m_isGetKeyDefined)
    {
        // NOTE: this should not happen, if Fast asks for a key we are not able to give it
        // This should not happen as using reader qos expects_inline_qos the instance handle should arrive in
        // inline_qos and this function should not be called
        // PD for reviewer: You shall remember this line, as it will bring hell on hearth in a possible future.
        logDevError(DDSPIPE_PARTICIPANTS_TYPESUPPORT,
                "Generic TypeSupport does not know how to retrieve the key, this should not happen.");
    }
    return false;
}

void* TopicDataType::createData()
{
    return reinterpret_cast<void*>(new DataType());
}

void TopicDataType::deleteData(
        void* data)
{
    delete(reinterpret_cast<DataType*>(data));
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
