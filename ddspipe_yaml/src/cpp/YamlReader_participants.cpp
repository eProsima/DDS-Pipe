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

#include <ddspipe_participants/types/address/Address.hpp>
#include <ddspipe_participants/types/address/DiscoveryServerConnectionAddress.hpp>
#include <ddspipe_participants/types/security/tls/TlsConfiguration.hpp>

#include <ddspipe_participants/configuration/DiscoveryServerParticipantConfiguration.hpp>
#include <ddspipe_participants/configuration/InitialPeersParticipantConfiguration.hpp>
#include <ddspipe_participants/configuration/XmlParticipantConfiguration.hpp>
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
* PARTICIPANTS         *
************************/

//////////////////////////////////
// ParticipantConfiguration
template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        participants::ParticipantConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Id required
    object.id = get<ParticipantId>(yml, PARTICIPANT_NAME_TAG, version);
}

template <>
DDSPIPE_YAML_DllAPI
participants::ParticipantConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    participants::ParticipantConfiguration object;
    fill<participants::ParticipantConfiguration>(object, yml, version);
    return object;
}

//////////////////////////////////
// EchoParticipantConfiguration
template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        participants::EchoParticipantConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Parent class fill
    fill<participants::ParticipantConfiguration>(object, yml, version);

    // data optional
    if (is_tag_present(yml, ECHO_DATA_TAG))
    {
        object.echo_data = get<bool>(yml, ECHO_DATA_TAG, version);
    }

    // discovery optional
    if (is_tag_present(yml, ECHO_DISCOVERY_TAG))
    {
        object.echo_discovery = get<bool>(yml, ECHO_DISCOVERY_TAG, version);
    }

    // verbose optional
    if (is_tag_present(yml, ECHO_VERBOSE_TAG))
    {
        object.verbose = get<bool>(yml, ECHO_VERBOSE_TAG, version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
participants::EchoParticipantConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    participants::EchoParticipantConfiguration object;
    fill<participants::EchoParticipantConfiguration>(object, yml, version);
    return object;
}

//////////////////////////////////
// SimpleParticipantConfiguration
template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        participants::SimpleParticipantConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Parent class fill
    fill<participants::ParticipantConfiguration>(object, yml, version);

    // Domain optional
    if (is_tag_present(yml, DOMAIN_ID_TAG))
    {
        object.domain = get<DomainId>(yml, DOMAIN_ID_TAG, version);
    }

    // Optional whitelist interfaces
    if (YamlReader::is_tag_present(yml, WHITELIST_INTERFACES_TAG))
    {
        object.whitelist = YamlReader::get_set<participants::types::IpType>(yml, WHITELIST_INTERFACES_TAG, version);
    }

    // Optional get Transport descriptors
    if (YamlReader::is_tag_present(yml, TRANSPORT_DESCRIPTORS_TRANSPORT_TAG))
    {
        object.transport = get<core::types::TransportDescriptors>(yml, TRANSPORT_DESCRIPTORS_TRANSPORT_TAG, version);
    }
    else
    {
        object.transport = core::types::TransportDescriptors::builtin;
    }

    // Optional get ignore participant flags
    if (YamlReader::is_tag_present(yml, IGNORE_PARTICIPANT_FLAGS_TAG))
    {
        object.ignore_participant_flags = get<core::types::IgnoreParticipantFlags>(yml, IGNORE_PARTICIPANT_FLAGS_TAG,
                        version);
    }
    else
    {
        object.ignore_participant_flags = core::types::IgnoreParticipantFlags::no_filter;
    }

    // Optional Praticipant Topic QoS
    if (YamlReader::is_tag_present(yml, PARTICIPANT_QOS_TAG))
    {
        fill<TopicQoS>(object.topic_qos, get_value_in_tag(yml, PARTICIPANT_QOS_TAG), version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
participants::SimpleParticipantConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    participants::SimpleParticipantConfiguration object;
    fill<participants::SimpleParticipantConfiguration>(object, yml, version);
    return object;
}

//////////////////////////////////
// DiscoveryServerParticipantConfiguration
template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        participants::DiscoveryServerParticipantConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Parent class fill
    fill<participants::SimpleParticipantConfiguration>(object, yml, version);

    // Optional whitelist interfaces
    if (YamlReader::is_tag_present(yml, WHITELIST_INTERFACES_TAG))
    {
        object.whitelist = YamlReader::get_set<participants::types::IpType>(yml, WHITELIST_INTERFACES_TAG, version);
    }

    // Optional listening addresses
    if (YamlReader::is_tag_present(yml, LISTENING_ADDRESSES_TAG))
    {
        object.listening_addresses = YamlReader::get_set<Address>(yml, LISTENING_ADDRESSES_TAG, version);
    }

    // Optional connection addresses
    if (YamlReader::is_tag_present(yml, CONNECTION_ADDRESSES_TAG))
    {
        object.connection_addresses = YamlReader::get_set<DiscoveryServerConnectionAddress>(
            yml,
            CONNECTION_ADDRESSES_TAG,
            version);
    }

    // Optional TLS
    if (YamlReader::is_tag_present(yml, TLS_TAG))
    {
        YamlReader::fill<TlsConfiguration>(
            object.tls_configuration,
            YamlReader::get_value_in_tag(yml, TLS_TAG),
            version);
    }

    // NOTE: The only field that change regarding the version is the GuidPrefix.
    switch (version)
    {
        case V_1_0:
            object.discovery_server_guid_prefix =
                    YamlReader::get<GuidPrefix>(yml, version);
            break;

        default:
            object.discovery_server_guid_prefix =
                    YamlReader::get<GuidPrefix>(yml, DISCOVERY_SERVER_GUID_PREFIX_TAG, version);
            break;
    }
}

template <>
DDSPIPE_YAML_DllAPI
participants::DiscoveryServerParticipantConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    participants::DiscoveryServerParticipantConfiguration object;
    fill<participants::DiscoveryServerParticipantConfiguration>(object, yml, version);
    return object;
}

//////////////////////////////////
// InitialPeersParticipantConfiguration
template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        participants::InitialPeersParticipantConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Parent class fill
    fill<participants::SimpleParticipantConfiguration>(object, yml, version);

    // Optional whitelist interfaces
    if (YamlReader::is_tag_present(yml, WHITELIST_INTERFACES_TAG))
    {
        object.whitelist = YamlReader::get_set<participants::types::IpType>(yml, WHITELIST_INTERFACES_TAG, version);
    }

    // Optional listening addresses
    if (YamlReader::is_tag_present(yml, LISTENING_ADDRESSES_TAG))
    {
        object.listening_addresses = YamlReader::get_set<Address>(yml, LISTENING_ADDRESSES_TAG, version);
    }

    // Optional connection addresses
    if (YamlReader::is_tag_present(yml, CONNECTION_ADDRESSES_TAG))
    {
        object.connection_addresses = YamlReader::get_set<Address>(
            yml,
            CONNECTION_ADDRESSES_TAG,
            version);
    }

    // Optional TLS
    if (YamlReader::is_tag_present(yml, TLS_TAG))
    {
        YamlReader::fill<TlsConfiguration>(
            object.tls_configuration,
            YamlReader::get_value_in_tag(yml, TLS_TAG),
            version);
    }

    // Optional Repeater
    if (YamlReader::is_tag_present(yml, IS_REPEATER_TAG))
    {
        object.is_repeater = YamlReader::get<bool>(yml, IS_REPEATER_TAG, version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
participants::InitialPeersParticipantConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    participants::InitialPeersParticipantConfiguration object;
    fill<participants::InitialPeersParticipantConfiguration>(object, yml, version);
    return object;
}

//////////////////////////////////
// XmlParticipantConfiguration
template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        participants::XmlParticipantConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Parent class fill
    fill<participants::SimpleParticipantConfiguration>(object, yml, version);

    // Mandatory participant profile
    object.participant_profile = YamlReader::get<std::string>(yml, XML_PARTICIPANT_PROFILE_TAG, version);
}

template <>
DDSPIPE_YAML_DllAPI
participants::XmlParticipantConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    participants::XmlParticipantConfiguration object;
    fill<participants::XmlParticipantConfiguration>(object, yml, version);
    return object;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
