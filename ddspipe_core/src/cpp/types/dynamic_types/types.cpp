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

DdsTopic type_object_topic()
{
    DdsTopic topic;
    topic.m_topic_name = TYPE_OBJECT_TOPIC_NAME;
    topic.type_name = TYPE_OBJECT_DATA_TYPE_NAME;

    return topic;
}

bool is_type_object_topic(
        const DdsTopic& topic)
{
    return (strcmp(topic.m_topic_name.c_str(), TYPE_OBJECT_TOPIC_NAME) == 0)
           && (strcmp(topic.type_name.c_str(), TYPE_OBJECT_DATA_TYPE_NAME) == 0);
}

Guid new_unique_guid()
{
    static unsigned int current_unique_value = 0;

    Guid new_guid;
    unsigned int numer = ++current_unique_value;
    unsigned int denom = 256;
    for (int i = 0; i < fastrtps::rtps::EntityId_t::size; i++)
    {
        new_guid.entityId.value[i] = numer % denom;
        numer = floor(numer / denom);
    }

    return new_guid;
}

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
