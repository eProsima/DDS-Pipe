// Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
#include <cpp_utils/utils.hpp>

#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

#include <ddspipe_participants/types/address/DiscoveryServerConnectionAddress.hpp>

#include <ddspipe_yaml/core/Yaml.hpp>
#include <ddspipe_yaml/field/YamlObjectField.hpp>
#include <ddspipe_yaml/tags/yaml_configuration_tags.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

template <>
std::vector<utils::Heritable<IYamlObjectField>> object_fields(
        const core::types::TopicQoS& object)
{
    // WARNING these fields are not complete
    // reliability_qos , durability_qos and ownership_qos are parsed separately
    return
    {
        create_object_field(QOS_HISTORY_DEPTH_TAG, OptionalKind::advisable, object.history_depth),
        create_object_field(QOS_PARTITION_TAG, OptionalKind::optional, object.use_partitions),
        create_object_field(QOS_DOWNSAMPLING_TAG, OptionalKind::optional, object.downsampling),
        create_object_field(QOS_KEYED_TAG, OptionalKind::optional, object.keyed),
        create_object_field(QOS_MAX_RECEPTION_RATE_TAG, OptionalKind::optional, object.max_reception_rate),
    };
}

template <>
std::vector<utils::Heritable<IYamlObjectField>> object_fields(
        const core::types::DdsTopic& object)
{
    return
    {
        create_object_field(TOPIC_NAME_TAG, OptionalKind::required, object.m_topic_name),
        create_object_field(TOPIC_TYPE_NAME_TAG, OptionalKind::required, object.type_name),
        create_object_field(TOPIC_QOS_TAG, OptionalKind::optional, object.topic_qos)
    };
}

template <>
std::vector<utils::Heritable<IYamlObjectField>> object_fields(
        const core::types::WildcardDdsFilterTopic& object)
{
    return
    {
        create_object_field(TOPIC_NAME_TAG, OptionalKind::optional, object.topic_name),
        create_object_field(TOPIC_TYPE_NAME_TAG, OptionalKind::optional, object.type_name),
    };
}

template <>
std::vector<utils::Heritable<IYamlObjectField>> object_fields(
        const participants::types::DiscoveryServerConnectionAddress& object)
{
    return
    {
        create_object_field(DISCOVERY_SERVER_GUID_PREFIX_TAG, OptionalKind::required, object.discovery_server_guid_prefix),
        create_object_field(COLLECTION_ADDRESSES_TAG, OptionalKind::advisable, object.addresses),
    };
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
