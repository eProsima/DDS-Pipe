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
#include <ddspipe_core/types/dds/TopicQoS.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

#include <ddspipe_yaml/field/YamlObjectField.hpp>
#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/reader/YamlReader.hpp>
#include <ddspipe_yaml/tags/yaml_configuration_tags.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// IMPLEMENTATION OF TYPES
////////////////////////////////////////////////

template <>
void read(
        const Yaml& yml,
        core::types::TopicQoS& object)
{
    read_from_fields(yml, object);

    // Reliability optional
    if (is_tag_present(yml, QOS_RELIABLE_TAG))
    {
        if (read<bool>(yml, QOS_RELIABLE_TAG))
        {
            object.reliability_qos = eprosima::ddspipe::core::types::ReliabilityKind::RELIABLE;
        }
        else
        {
            object.reliability_qos = eprosima::ddspipe::core::types::ReliabilityKind::BEST_EFFORT;
        }
    }

    // Durability optional
    if (is_tag_present(yml, QOS_TRANSIENT_TAG))
    {
        if (read<bool>(yml, QOS_TRANSIENT_TAG))
        {
            object.durability_qos = eprosima::ddspipe::core::types::DurabilityKind::TRANSIENT_LOCAL;
        }
        else
        {
            object.durability_qos = eprosima::ddspipe::core::types::DurabilityKind::VOLATILE;
        }
    }

    // Ownership optional
    if (is_tag_present(yml, QOS_OWNERSHIP_TAG))
    {
        if (read<bool>(yml, QOS_OWNERSHIP_TAG))
        {
            object.ownership_qos = eprosima::ddspipe::core::types::OwnershipQosPolicyKind::EXCLUSIVE_OWNERSHIP_QOS;
        }
        else
        {
            object.ownership_qos = eprosima::ddspipe::core::types::OwnershipQosPolicyKind::SHARED_OWNERSHIP_QOS;
        }
    }
}

template <>
void read(
        const Yaml& yml,
        core::types::DdsTopic& object)
{
    read_from_fields(yml, object);
}

template <>
void read(
        const Yaml& yml,
        core::types::WildcardDdsFilterTopic& object)
{
    read_from_fields(yml, object);
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
