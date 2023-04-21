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

/**
 * @file YamlReader.cpp
 *
 */

#include <cpp_utils/Log.hpp>
#include <cpp_utils/utils.hpp>

#include <ddspipe_core/types/dds/DomainId.hpp>
#include <ddspipe_core/types/dds/GuidPrefix.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

#include <ddspipe_participants/types/address/Address.hpp>
#include <ddspipe_participants/types/address/DiscoveryServerConnectionAddress.hpp>
#include <ddspipe_participants/types/security/tls/TlsConfiguration.hpp>

#include <ddspipe_participants/configuration/DiscoveryServerParticipantConfiguration.hpp>
#include <ddspipe_participants/configuration/InitialPeersParticipantConfiguration.hpp>
#include <ddspipe_participants/configuration/ParticipantConfiguration.hpp>
#include <ddspipe_participants/configuration/EchoParticipantConfiguration.hpp>
#include <ddspipe_participants/configuration/SimpleParticipantConfiguration.hpp>

#include <ddspipe_yaml/Yaml.hpp>
#include <ddspipe_yaml/YamlReader.hpp>
#include <ddspipe_yaml/yaml_configuration_tags.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

using namespace eprosima::ddspipe::core::types;
using namespace eprosima::ddspipe::participants::types;

/************************
* GENERIC              *
************************/

bool YamlReader::is_tag_present(
        const Yaml& yml,
        const TagType& tag)
{
    if (!yml.IsMap() && !yml.IsNull())
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Trying to find a tag: <" << tag << "> in a not yaml object map.");
    }

    // Explicit conversion to avoid windows format warning
    // This method performace is the same as only retrieving bool
    return (yml[tag]) ? true : false;
}

Yaml YamlReader::get_value_in_tag(
        const Yaml& yml,
        const TagType& tag)
{
    if (is_tag_present(yml, tag))
    {
        return yml[tag];
    }
    else
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Required tag not found: <" << tag << ">.");
    }
}

/************************
* STANDARD             *
************************/

template <>
DDSPIPE_YAML_DllAPI
int YamlReader::get<int>(
        const Yaml& yml,
        const YamlReaderVersion version /* version */)
{
    return get_scalar<int>(yml);
}

template <>
DDSPIPE_YAML_DllAPI
unsigned int YamlReader::get<unsigned int>(
        const Yaml& yml,
        const YamlReaderVersion version /* version */)
{
    return get_scalar<unsigned int>(yml);
}

unsigned int YamlReader::get_positive_int(
        const Yaml& yml,
        const TagType& tag)
{
    int ret;
    try
    {
        // NOTE: get signed int and check positive afterwards to avoid underflow when negative values provided
        ret = get_scalar<int>(get_value_in_tag(yml, tag));
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error reading positive integer under tag <" << tag << "> :\n " << e.what());
    }

    if (!(ret > 0))
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error reading positive integer under tag <" << tag <<
                      "> : value is not greater than 0.");
    }

    return ret;
}

template <>
DDSPIPE_YAML_DllAPI
float YamlReader::get<float>(
        const Yaml& yml,
        const YamlReaderVersion version /* version */)
{
    return get_scalar<float>(yml);
}

float YamlReader::get_positive_float(
        const Yaml& yml,
        const TagType& tag)
{
    float ret;
    try
    {
        ret = get_scalar<float>(get_value_in_tag(yml, tag));
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error reading positive float under tag <" << tag << "> :\n " << e.what());
    }

    if (!(ret > 0))
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error reading positive float under tag <" << tag <<
                      "> : value is not greater than 0.");
    }

    return ret;
}

template <>
DDSPIPE_YAML_DllAPI
double YamlReader::get<double>(
        const Yaml& yml,
        const YamlReaderVersion version /* version */)
{
    return get_scalar<double>(yml);
}

double YamlReader::get_positive_double(
        const Yaml& yml,
        const TagType& tag)
{
    double ret;
    try
    {
        ret = get_scalar<double>(get_value_in_tag(yml, tag));
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error reading positive double under tag <" << tag << "> :\n " << e.what());
    }

    if (!(ret > 0))
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error reading positive double under tag <" << tag <<
                      "> : value is not greater than 0.");
    }

    return ret;
}

template <>
DDSPIPE_YAML_DllAPI
bool YamlReader::get<bool>(
        const Yaml& yml,
        const YamlReaderVersion version /* version */)
{
    return get_scalar<bool>(yml);
}

template <>
DDSPIPE_YAML_DllAPI
std::string YamlReader::get<std::string>(
        const Yaml& yml,
        const YamlReaderVersion version /* version */)
{
    return get_scalar<std::string>(yml);
}

bool check_tags(
        const std::vector<YamlFieldCheck>& tags_allowed,
        const Yaml& yml,
        bool fail_with_extra_tags /* = true */,
        bool fail_with_exception /* = true */)
{
    if (!yml.IsMap() && !yml.IsNull())
    {
        throw eprosima::utils::ConfigurationException(STR_ENTRY
                  << "Trying to check tags: in a not yaml object map.");
    }

    // First, check that every required flag is present
    for (const auto& field : tags_allowed)
    {
        if (field.kind == TagKind::required)
        {
            if (!YamlReader::is_tag_present(yml, field.tag))
            {
                if (fail_with_exception)
                {
                    throw eprosima::utils::ConfigurationException(STR_ENTRY
                        << "Required tag <" << field.tag << "> not present.");
                }
                else
                {
                    return false;
                }
            }
        }
    }

    // Now that every required field is present, check for extra tags
    for (const auto& yaml_field : yml)
    {
        auto tag = yaml_field.first.as<std::string>();
        bool should_be_present = false;

        // For each tag, check if it is inside allowed tags
        for (const auto& field : tags_allowed)
        {
            if (field.tag == tag)
            {
                should_be_present = true;
                break;
            }
        }

        // If present tag is not inside allowed ones, fail
        if (!should_be_present)
        {
            if (fail_with_exception)
            {
                throw eprosima::utils::ConfigurationException(STR_ENTRY
                    << "Unexpected tag <" << tag << "> present.");
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
