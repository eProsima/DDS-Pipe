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
#include <cpp_utils/memory/Heritable.hpp>

#include <ddspipe_core/configuration/RoutesConfiguration.hpp>
#include <ddspipe_core/configuration/TopicRoutesConfiguration.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/topic/dds/DistributedTopic.hpp>
#include <ddspipe_participants/xml/XmlHandler.hpp>
#include <ddspipe_participants/xml/XmlHandlerConfiguration.hpp>

#include <ddspipe_yaml/Yaml.hpp>
#include <ddspipe_yaml/YamlReader.hpp>
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
participants::XmlHandlerConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    participants::XmlHandlerConfiguration object;
    fill<participants::XmlHandlerConfiguration>(object, yml, version);
    return object;
}

/************************
* Routes Configuration  *
************************/

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
        core::types::ParticipantId src;
        std::set<core::types::ParticipantId> dst;

        // Required route source
        if (is_tag_present(route_yml, ROUTES_SRC_TAG))
        {
            src = get<core::types::ParticipantId>(route_yml, ROUTES_SRC_TAG, version);
            if (object.routes.count(src) != 0)
            {
                throw eprosima::utils::ConfigurationException(
                          utils::Formatter() <<
                              "Multiple routes defined for participant " << src  << " : only one allowed.");
            }
        }
        else
        {
            throw eprosima::utils::ConfigurationException(
                      utils::Formatter() <<
                          "Source participant required under tag " << ROUTES_SRC_TAG << " in route definition.");
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
    core::RoutesConfiguration object;
    fill<core::RoutesConfiguration>(object, yml, version);
    return object;
}

/******************************
* Topic Routes Configuration  *
******************************/

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
        // utils::Heritable<ddspipe::core::types::DistributedTopic> topic;
        auto topic = utils::Heritable<ddspipe::core::types::DdsTopic>::make_heritable();
        core::RoutesConfiguration routes;

        // Required topic and type names
        if (!(is_tag_present(topic_routes_yml,
                TOPIC_NAME_TAG) && is_tag_present(topic_routes_yml, TOPIC_TYPE_NAME_TAG)))
        {
            throw eprosima::utils::ConfigurationException(
                      utils::Formatter() <<
                          "Topic routes require topic and type names to be defined under tags " << TOPIC_NAME_TAG << " and " << TOPIC_TYPE_NAME_TAG <<
                          ", respectively.");
        }
        else
        {
            topic = get<utils::Heritable<ddspipe::core::types::DistributedTopic>>(topic_routes_yml, version);
            if (object.topic_routes.count(topic) != 0)
            {
                throw eprosima::utils::ConfigurationException(
                          utils::Formatter() <<
                              "Multiple routes defined for topic " << topic  << " : only one allowed.");
            }
        }

        // Required routes
        if (is_tag_present(topic_routes_yml, ROUTES_TAG))
        {
            routes = get<core::RoutesConfiguration>(topic_routes_yml, ROUTES_TAG, version);
        }
        else
        {
            throw eprosima::utils::ConfigurationException(
                      utils::Formatter() <<
                          "No routes found under tag " << ROUTES_TAG << " for topic " << topic << " .");
        }

        // Insert routes
        object.topic_routes[topic] = routes;
    }
}

template <>
DDSPIPE_YAML_DllAPI
core::TopicRoutesConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    core::TopicRoutesConfiguration object;
    fill<core::TopicRoutesConfiguration>(object, yml, version);
    return object;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
