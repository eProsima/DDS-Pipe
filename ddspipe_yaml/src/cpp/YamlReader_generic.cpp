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
#include <cpp_utils/time/time_utils.hpp>
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

unsigned int YamlReader::get_nonnegative_int(
        const Yaml& yml,
        const TagType& tag)
{
    int ret;
    try
    {
        // NOTE: get signed int and check nonnegative afterwards to avoid underflow when negative values provided
        ret = get_scalar<int>(get_value_in_tag(yml, tag));
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error reading nonnegative integer under tag <" << tag << "> :\n " << e.what());
    }

    if (!(ret >= 0))
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error reading nonnegative integer under tag <" << tag <<
                      "> : value is negative.");
    }

    return ret;
}

unsigned int YamlReader::get_positive_int(
        const Yaml& yml,
        const TagType& tag)
{
    int ret;
    try
    {
        ret = get_nonnegative_int(yml, tag);
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

float YamlReader::get_nonnegative_float(
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
                  utils::Formatter() << "Error reading nonnegative float under tag <" << tag << "> :\n " << e.what());
    }

    if (!(ret >= 0))
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error reading nonnegative float under tag <" << tag <<
                      "> : value is negative.");
    }

    return ret;
}

float YamlReader::get_positive_float(
        const Yaml& yml,
        const TagType& tag)
{
    float ret;
    try
    {
        ret = get_nonnegative_float(yml, tag);
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

double YamlReader::get_nonnegative_double(
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
                  utils::Formatter() << "Error reading nonnegative double under tag <" << tag << "> :\n " << e.what());
    }

    if (!(ret >= 0))
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error reading nonnegative double under tag <" << tag <<
                      "> : value is negative.");
    }

    return ret;
}

double YamlReader::get_positive_double(
        const Yaml& yml,
        const TagType& tag)
{
    double ret;
    try
    {
        ret = get_nonnegative_double(yml, tag);
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

template <>
DDSPIPE_YAML_DllAPI
utils::Timestamp YamlReader::get<utils::Timestamp>(
        const Yaml& yml,
        const YamlReaderVersion version /* version */)
{
    utils::Timestamp ret_timestamp;
    std::string datetime_str;
    std::string datetime_format("%Y-%m-%d_%H-%M-%S");
    bool local = true;
    std::chrono::milliseconds ms(0);
    std::chrono::microseconds us(0);
    std::chrono::nanoseconds ns(0);

    // Get optional datetime format
    if (is_tag_present(yml, TIMESTAMP_DATETIME_FORMAT_TAG))
    {
        datetime_format = get<std::string>(yml, TIMESTAMP_DATETIME_FORMAT_TAG, version);
    }

    // Get optional local
    if (is_tag_present(yml, TIMESTAMP_LOCAL_TAG))
    {
        local = get<bool>(yml, TIMESTAMP_LOCAL_TAG, version);
    }

    // Get required datetime
    if (is_tag_present(yml, TIMESTAMP_DATETIME_TAG))
    {
        datetime_str = get<std::string>(yml, TIMESTAMP_DATETIME_TAG, version);
        try
        {
            ret_timestamp = utils::string_to_timestamp(datetime_str, datetime_format, local);
        }
        catch (const std::exception& e)
        {
            throw eprosima::utils::ConfigurationException(utils::Formatter() <<
                          "Error parsing datetime " << datetime_str << " with error:\n " <<
                          e.what());
        }
    }
    else
    {
        throw eprosima::utils::ConfigurationException(utils::Formatter() <<
                      "Timestamp requires to specify <" << TIMESTAMP_DATETIME_TAG << ">.");
    }

    // Get optional milliseconds
    if (is_tag_present(yml, TIMESTAMP_MILLISECONDS_TAG))
    {
        ms = std::chrono::milliseconds(get_nonnegative_int(yml, TIMESTAMP_MILLISECONDS_TAG));
    }

    // Get optional microseconds
    if (is_tag_present(yml, TIMESTAMP_MICROSECONDS_TAG))
    {
        us = std::chrono::microseconds(get_nonnegative_int(yml, TIMESTAMP_MICROSECONDS_TAG));
    }

    // Get optional nanoseconds
    if (is_tag_present(yml, TIMESTAMP_NANOSECONDS_TAG))
    {
        ns = std::chrono::nanoseconds(get_nonnegative_int(yml, TIMESTAMP_NANOSECONDS_TAG));
    }

    return std::chrono::time_point_cast<utils::Timestamp::duration>(ret_timestamp + ms + us + ns);
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
