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

/**
 * @file types.cpp
 */

#include <fastrtps/types/TypeObjectFactory.h>

#include <ddspipe_core/types/dynamic_types/types.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

types::TopicInternalTypeDiscriminator DynamicTypeData::internal_type_discriminator() const noexcept
{
    return INTERNAL_TOPIC_TYPE_DYNAMIC_TYPE;
}

DdsTopic type_object_topic()
{
    DdsTopic topic;
    topic.m_topic_name = TYPE_OBJECT_TOPIC_NAME;
    topic.type_name = TYPE_OBJECT_DATA_TYPE_NAME;
    topic.m_internal_type_discriminator = INTERNAL_TOPIC_TYPE_DYNAMIC_TYPE;

    return topic;
}

bool is_type_object_topic(
        const ITopic& topic)
{
    return topic.internal_type_discriminator() == INTERNAL_TOPIC_TYPE_DYNAMIC_TYPE;
}

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
