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

#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>

#include <ddspipe_participants/types/dds/TopicDataType.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

using InternalDataType = fastrtps::rtps::SerializedPayload_t;

TopicDataType::TopicDataType(
        const core::types::DdsTopic& topic)
    : topic_(topic)
{
    // Set topic data
    m_typeSize = 4;
    m_isGetKeyDefined = topic_.topic_qos.keyed;
    auto_fill_type_object(false);
    auto_fill_type_information(false);

    // Set name
    setName(topic_.type_name.c_str());
}

TopicDataType::~TopicDataType()
{
    // Do nothing
}

bool TopicDataType::serialize(
        void* data,
        fastrtps::rtps::SerializedPayload_t* target_payload)
{
    InternalDataType* src_payload = static_cast<InternalDataType*>(data);

    // Copy each varible
    target_payload = src_payload;

    // Copy internal data
    std::memcpy(target_payload->data, src_payload->data, src_payload->length);

    return true;
}

bool TopicDataType::deserialize(
        eprosima::fastrtps::rtps::SerializedPayload_t* src_payload,
        void* data)
{
    InternalDataType* target_payload = static_cast<InternalDataType*>(data);

    // Copy each varible
    target_payload = src_payload;

    // Copy internal data
    std::memcpy(target_payload->data, src_payload->data, src_payload->length);

    return true;
}

std::function<uint32_t()> TopicDataType::getSerializedSizeProvider(
        void* data)
{
    return [data]() -> uint32_t
           {
               auto p = static_cast<InternalDataType*>(data);
               return p->length;
           };
}

bool TopicDataType::getKey(
        void* data,
        eprosima::fastrtps::rtps::InstanceHandle_t* handle,
        bool force_md5 /* = false */)
{
    if (!m_isGetKeyDefined)
    {
        return false;
    }

    InternalDataType* src_payload = static_cast<InternalDataType*>(data);
    for (uint8_t i = 0; i < 16; ++i)
    {
        handle->value[i] = src_payload->data[i];
    }
    return true;
}

void* TopicDataType::createData()
{
    return reinterpret_cast<void*>(new InternalDataType());
}

void TopicDataType::deleteData(
        void* data)
{
    delete(reinterpret_cast<InternalDataType*>(data));
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
