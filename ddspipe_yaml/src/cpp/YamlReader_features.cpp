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

#include <set>

#include <cpp_utils/memory/Heritable.hpp>

#include <ddspipe_core/configuration/DdsPublishingConfiguration.hpp>
#include <ddspipe_core/configuration/MonitorConfiguration.hpp>
#include <ddspipe_core/configuration/MonitorProducerConfiguration.hpp>
#include <ddspipe_core/configuration/RoutesConfiguration.hpp>
#include <ddspipe_core/configuration/TopicRoutesConfiguration.hpp>
#include <ddspipe_core/monitoring/producers/StatusMonitorProducer.hpp>
#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>
#include <ddspipe_core/types/dds/DomainId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/topic/dds/DistributedTopic.hpp>
#include <ddspipe_participants/xml/XmlHandler.hpp>
#include <ddspipe_participants/xml/XmlHandlerConfiguration.hpp>

#include <ddspipe_yaml/Yaml.hpp>
#include <ddspipe_yaml/YamlReader.hpp>
#include <ddspipe_yaml/YamlValidator.hpp>
#include <ddspipe_yaml/yaml_configuration_tags.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

/************************
* XML                   *
************************/

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        participants::XmlHandlerConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Optional raw
    if (is_tag_present(yml, XML_RAW_TAG))
    {
        object.raw = get<std::string>(yml, XML_RAW_TAG, version);
    }

    // Optional files
    if (is_tag_present(yml, XML_FILES_TAG))
    {
        object.files = get_set<std::string>(yml, XML_FILES_TAG, version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
bool YamlValidator::validate<participants::XmlHandlerConfiguration>(
        const Yaml& yml,
        const YamlReaderVersion& /* version */)
{
    static const std::set<TagType> tags{
        XML_RAW_TAG,
        XML_FILES_TAG};

    return YamlValidator::validate_tags(yml, tags);
}

template <>
DDSPIPE_YAML_DllAPI
participants::XmlHandlerConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    YamlValidator::validate<participants::XmlHandlerConfiguration>(yml, version);

    participants::XmlHandlerConfiguration object;
    fill<participants::XmlHandlerConfiguration>(object, yml, version);
    return object;
}

/************************
* Routes Configuration  *
************************/

template <>
DDSPIPE_YAML_DllAPI
bool YamlValidator::validate<core::RoutesConfiguration>(
        const Yaml& yml,
        const YamlReaderVersion& /* version */)
{
    static const std::set<TagType> tags{
        ROUTES_SRC_TAG,
        ROUTES_DST_TAG};

    return YamlValidator::validate_tags(yml, tags);
}

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        core::RoutesConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Check it is a list
    if (!yml.IsSequence())
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() <<
                      "Custom forwarding routes must be specified in an array under tag: " <<
                      ROUTES_TAG);
    }

    for (const auto& route_yml : yml)
    {
        // The validation has to be done here since the yml object is a list with the route
        YamlValidator::validate<core::RoutesConfiguration>(route_yml, version);

        core::types::ParticipantId src;
        std::set<core::types::ParticipantId> dst;

        // Required route source
        src = get<core::types::ParticipantId>(route_yml, ROUTES_SRC_TAG, version);

        if (object.routes.count(src) != 0)
        {
            throw eprosima::utils::ConfigurationException(
                      utils::Formatter() <<
                          "Multiple routes defined for participant " << src  << " : only one allowed.");
        }

        // Optional route destination(s)
        if (is_tag_present(route_yml, ROUTES_DST_TAG))
        {
            // NOTE: Inner conversion from list to set removes duplicates
            dst = get_set<core::types::ParticipantId>(route_yml, ROUTES_DST_TAG, version);
        }
        else
        {
            // Do not forward incoming traffic to any destination
        }

        // Insert route
        object.routes[src] = dst;
    }
}

template <>
DDSPIPE_YAML_DllAPI
core::RoutesConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // NOTE: The validation has to be done inside the fill method since the yml object is a list with the routes

    core::RoutesConfiguration object;
    fill<core::RoutesConfiguration>(object, yml, version);
    return object;
}

/******************************
* Topic Routes Configuration  *
******************************/

template <>
DDSPIPE_YAML_DllAPI
bool YamlValidator::validate<core::TopicRoutesConfiguration>(
        const Yaml& yml,
        const YamlReaderVersion& /* version */)
{
    static const std::set<TagType> tags{
        TOPIC_NAME_TAG,
        TOPIC_TYPE_NAME_TAG,
        ROUTES_TAG};

    return YamlValidator::validate_tags(yml, tags);
}

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        core::TopicRoutesConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Check it is a list
    if (!yml.IsSequence())
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() <<
                      "Custom topic forwarding routes must be specified in an array under tag: " <<
                      TOPIC_ROUTES_TAG);
    }

    for (const auto& topic_routes_yml : yml)
    {
        YamlValidator::validate<core::TopicRoutesConfiguration>(topic_routes_yml, version);

        // utils::Heritable<ddspipe::core::types::DistributedTopic> topic;
        auto topic = utils::Heritable<ddspipe::core::types::DdsTopic>::make_heritable();
        core::RoutesConfiguration routes;

        // Required topic and type names
        fill<eprosima::ddspipe::core::types::DdsTopic>(topic.get_reference(), topic_routes_yml, version);

        if (object.topic_routes.count(topic) != 0)
        {
            throw eprosima::utils::ConfigurationException(
                      utils::Formatter() <<
                          "Multiple routes defined for topic " << topic  << " : only one allowed.");
        }

        // Required route
        object.topic_routes[topic] = get<core::RoutesConfiguration>(topic_routes_yml, ROUTES_TAG, version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
core::TopicRoutesConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // NOTE: The validation has to be done inside the fill method since the yml object is a list with the topic routes

    core::TopicRoutesConfiguration object;
    fill<core::TopicRoutesConfiguration>(object, yml, version);
    return object;
}

/**************************
* Monitor Configuration  *
**************************/

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        core::MonitorProducerConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Optional enable
    if (is_tag_present(yml, MONITOR_ENABLE_TAG))
    {
        object.enabled = get<bool>(yml, MONITOR_ENABLE_TAG, version);
    }

    // Optional period
    if (is_tag_present(yml, MONITOR_PERIOD_TAG))
    {
        object.period = get_positive_double(yml, MONITOR_PERIOD_TAG);
    }
}

template <>
DDSPIPE_YAML_DllAPI
bool YamlValidator::validate<core::MonitorProducerConfiguration>(
        const Yaml& yml,
        const YamlReaderVersion& /* version */)
{
    static const std::set<TagType> tags{
        MONITOR_ENABLE_TAG,
        MONITOR_PERIOD_TAG};

    return YamlValidator::validate_tags(yml, tags);
}

template <>
DDSPIPE_YAML_DllAPI
core::MonitorProducerConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    YamlValidator::validate<core::MonitorProducerConfiguration>(yml, version);

    core::MonitorProducerConfiguration object;
    fill<core::MonitorProducerConfiguration>(object, yml, version);
    return object;
}

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        core::MonitorConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    core::types::DomainIdType domain = 0;

    // Optional domain
    if (is_tag_present(yml, MONITOR_DOMAIN_TAG))
    {
        domain = get<int>(yml, MONITOR_DOMAIN_TAG, version);
    }

    static const std::set<TagType> tags{
        MONITOR_ENABLE_TAG,
        MONITOR_PERIOD_TAG,
        DDS_PUBLISHING_ENABLE_TAG,
        DDS_PUBLISHING_DOMAIN_TAG,
        DDS_PUBLISHING_TOPIC_NAME_TAG,
        DDS_PUBLISHING_PUBLISH_TYPE_TAG};

    /////
    // Get optional monitor status tag
    if (is_tag_present(yml, MONITOR_STATUS_TAG))
    {
        YamlValidator::validate_tags(yml[MONITOR_STATUS_TAG], tags);

        // NOTE: Use fill instead of get to avoid throwing exceptions if tags are not present
        fill<core::MonitorProducerConfiguration>(object.producers[core::STATUS_MONITOR_PRODUCER_ID],
                get_value_in_tag(yml, MONITOR_STATUS_TAG), version);

        // NOTE: Set the generic domain first so it can be overwritten by the specific domain if present
        object.consumers[core::STATUS_MONITOR_PRODUCER_ID].domain = domain;
        fill<core::DdsPublishingConfiguration>(object.consumers[core::STATUS_MONITOR_PRODUCER_ID],
                get_value_in_tag(yml, MONITOR_STATUS_TAG), version);
    }

    /////
    // Get optional monitor topics tag
    if (is_tag_present(yml, MONITOR_TOPICS_TAG))
    {
        YamlValidator::validate_tags(yml[MONITOR_TOPICS_TAG], tags);

        // NOTE: Use fill instead of get to avoid throwing exceptions if tags are not present
        fill<core::MonitorProducerConfiguration>(object.producers[core::TOPICS_MONITOR_PRODUCER_ID],
                get_value_in_tag(yml, MONITOR_TOPICS_TAG), version);

        // NOTE: Set the generic domain first so it can be overwritten by the specific domain if present
        object.consumers[core::TOPICS_MONITOR_PRODUCER_ID].domain = domain;
        fill<core::DdsPublishingConfiguration>(object.consumers[core::TOPICS_MONITOR_PRODUCER_ID],
                get_value_in_tag(yml, MONITOR_TOPICS_TAG), version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
bool YamlValidator::validate<core::MonitorConfiguration>(
        const Yaml& yml,
        const YamlReaderVersion& /* version */)
{
    static const std::set<TagType> tags{
        MONITOR_DOMAIN_TAG,
        MONITOR_STATUS_TAG,
        MONITOR_TOPICS_TAG};

    return YamlValidator::validate_tags(yml, tags);
}

template <>
DDSPIPE_YAML_DllAPI
core::MonitorConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    YamlValidator::validate<core::MonitorConfiguration>(yml, version);

    core::MonitorConfiguration object;
    fill<core::MonitorConfiguration>(object, yml, version);
    return object;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
