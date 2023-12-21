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

    if (src_payload->payload_owner == payload_pool_.get())
    {
        // The src and dst Payload Pools are the same. The payload can be referenced.
        // We do not call get_payload since Fast-DDS doesn't call release_payload internally.
        // If we did, there would be leaks.
        target_payload->data = src_payload->payload.data;
    }
    else
    {
        logWarning(DDSPIPE_DDS_TYPESUPPORT, "Copying the payload between two different payload pools.");

        // The src and dst Payload Pools are different. The payload must be copied.
        target_payload->copy(&src_payload->payload);
    }

    return true;
}

bool TopicDataType::deserialize(
        eprosima::fastrtps::rtps::SerializedPayload_t* src_payload,
        void* data)
{
    logDebug(DDSPIPE_DDS_TYPESUPPORT, "Deserializing data " << *src_payload << ".");

    DataType* target_payload = static_cast<DataType*>(data);

    // Get data and store it in PayloadPool
    eprosima::fastrtps::rtps::IPayloadPool* payload_owner =
            static_cast<eprosima::fastrtps::rtps::IPayloadPool*>(payload_pool_.get());

    payload_pool_->get_payload(*src_payload, payload_owner, target_payload->payload);

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
        void* data,
        eprosima::fastrtps::rtps::InstanceHandle_t* handle,
        bool /* = false */)
{
    if (m_isGetKeyDefined)
    {
        // Load the instanceHandle from data into handle
        auto p = static_cast<DataType*>(data);
        *handle = p->instanceHandle;
        return true;
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
