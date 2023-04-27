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

#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/writer/YamlWriter.hpp>
#include <ddspipe_yaml/field/YamlObjectField.hpp>
#include <ddspipe_yaml/tags/yaml_configuration_tags.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// IMPLEMENTATION OF TYPES
////////////////////////////////////////////////

template <>
void write(
        Yaml& yml,
        const core::types::TopicQoS& object)
{
    write_from_fields(yml, object);

    write<bool>(yml, QOS_RELIABLE_TAG, object.is_reliable());
    write<bool>(yml, QOS_TRANSIENT_TAG, object.is_transient_local());
    write<bool>(yml, QOS_OWNERSHIP_TAG, object.has_ownership());
}

template <>
void write(
        Yaml& yml,
        const core::types::DdsTopic& object)
{
    write_from_fields(yml, object);
}

template <>
void write(
        Yaml& yml,
        const core::types::WildcardDdsFilterTopic& object)
{
    write_from_fields(yml, object);
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
