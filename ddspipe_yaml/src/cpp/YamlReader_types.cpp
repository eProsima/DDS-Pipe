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
#include <cpp_utils/memory/Heritable.hpp>
#include <cpp_utils/utils.hpp>

#include <ddspipe_core/configuration/DdsPipeLogConfiguration.hpp>
#include <ddspipe_core/types/dds/CustomTransport.hpp>
#include <ddspipe_core/types/dds/DomainId.hpp>
#include <ddspipe_core/types/dds/GuidPrefix.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/topic/filter/ManualTopic.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

#include <ddspipe_participants/types/address/Address.hpp>
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

template <>
DDSPIPE_YAML_DllAPI
TransportDescriptors YamlReader::get<TransportDescriptors>(
        const Yaml& yml,
        const YamlReaderVersion /* version */)
{
    return get_enumeration<TransportDescriptors>(
        yml,
                {
                    {TRANSPORT_DESCRIPTORS_BUILTIN_TAG, TransportDescriptors::builtin},
                    {TRANSPORT_DESCRIPTORS_UDP_TAG, TransportDescriptors::udp_only},
                    {TRANSPORT_DESCRIPTORS_SHM_TAG, TransportDescriptors::shm_only}
                });
}

template <>
DDSPIPE_YAML_DllAPI
IgnoreParticipantFlags YamlReader::get<IgnoreParticipantFlags>(
        const Yaml& yml,
        const YamlReaderVersion /* version */)
{
    return get_enumeration<IgnoreParticipantFlags>(
        yml,
                {
                    {IGNORE_PARTICIPANT_FLAGS_NO_FILTER_TAG, IgnoreParticipantFlags::no_filter},
                    {IGNORE_PARTICIPANT_FLAGS_DIFFERENT_HOST_TAG, IgnoreParticipantFlags::filter_different_host},
                    {IGNORE_PARTICIPANT_FLAGS_DIFFERENT_PROCESS_TAG, IgnoreParticipantFlags::filter_different_process},
                    {IGNORE_PARTICIPANT_FLAGS_SAME_PROCESS_TAG, IgnoreParticipantFlags::filter_same_process},
                    {IGNORE_PARTICIPANT_FLAGS_DIFFERENT_AND_SAME_PROCESS_TAG,
                     IgnoreParticipantFlags::filter_different_and_same_process},
                });
}

template <>
DDSPIPE_YAML_DllAPI
YamlReaderVersion YamlReader::get<YamlReaderVersion>(
        const Yaml& yml,
        const YamlReaderVersion /* version */)
{
    return get_enumeration<YamlReaderVersion>(
        yml,
                {
                    {VERSION_TAG_V_1_0, YamlReaderVersion::V_1_0},
                    {VERSION_TAG_V_2_0, YamlReaderVersion::V_2_0},
                    {VERSION_TAG_V_3_0, YamlReaderVersion::V_3_0},
                    {VERSION_TAG_V_3_1, YamlReaderVersion::V_3_1},
                    {VERSION_TAG_V_4_0, YamlReaderVersion::V_4_0},
                    {VERSION_TAG_V_5_0, YamlReaderVersion::V_5_0},
                });
}

template <>
DDSPIPE_YAML_DllAPI
TransportProtocol YamlReader::get<TransportProtocol>(
        const Yaml& yml,
        const YamlReaderVersion /* version */)
{
    return get_enumeration<TransportProtocol>(
        yml,
                {
                    {ADDRESS_TRANSPORT_TCP_TAG, TransportProtocol::tcp},
                    {ADDRESS_TRANSPORT_UDP_TAG, TransportProtocol::udp}
                });
}

template <>
DDSPIPE_YAML_DllAPI
IpVersion YamlReader::get<IpVersion>(
        const Yaml& yml,
        const YamlReaderVersion /* version */)
{
    return get_enumeration<IpVersion>(
        yml,
                {
                    {ADDRESS_IP_VERSION_V4_TAG, IpVersion::v4},
                    {ADDRESS_IP_VERSION_V6_TAG, IpVersion::v6},
                });
}

template <>
DDSPIPE_YAML_DllAPI
PortType YamlReader::get<PortType>(
        const Yaml& yml,
        const YamlReaderVersion /* version */)
{
    // Domain id required
    return PortType(get_scalar<PortType>(yml));
}

template <>
DDSPIPE_YAML_DllAPI
DomainId YamlReader::get<DomainId>(
        const Yaml& yml,
        const YamlReaderVersion /* version */)
{
    // Domain id required
    DomainId domain;
    domain.domain_id = get_scalar<DomainIdType>(yml);
    return domain;
}

template <>
DDSPIPE_YAML_DllAPI
GuidPrefix YamlReader::get<GuidPrefix>(
        const Yaml& yml,
        const YamlReaderVersion /* version */)
{
    // If guid exists, use it. Non mandatory.
    if (is_tag_present(yml, DISCOVERY_SERVER_GUID_TAG))
    {
        std::string guid = get_scalar<std::string>(yml, DISCOVERY_SERVER_GUID_TAG);
        return GuidPrefix(guid);
    }

    // ROS DS is optional.
    bool ros_id = false;
    bool ros_id_set = is_tag_present(yml, DISCOVERY_SERVER_ID_ROS_TAG);
    if (ros_id_set)
    {
        ros_id = get_scalar<bool>(yml, DISCOVERY_SERVER_ID_ROS_TAG);
    }

    // Id is optional.
    uint32_t id = 0;
    bool id_set = is_tag_present(yml, DISCOVERY_SERVER_ID_TAG);
    if (id_set)
    {
        id = get_scalar<uint32_t>(yml, DISCOVERY_SERVER_ID_TAG);
    }

    if (ros_id_set || id_set)
    {
        return GuidPrefix(ros_id, id);
    }
    else
    {
        // Return unknown prefix -> delegate assignment on Fast-DDS
        return GuidPrefix();
    }
}

template <>
DDSPIPE_YAML_DllAPI
Address YamlReader::get<Address>(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Optional get IP version
    IpVersion ip_version;
    bool ip_version_set = is_tag_present(yml, ADDRESS_IP_VERSION_TAG);
    if (ip_version_set)
    {
        // Get IP Version from enumeration
        ip_version = get<IpVersion>(yml, ADDRESS_IP_VERSION_TAG, version);
    }

    // Optional get IP
    IpType ip;
    bool ip_set = is_tag_present(yml, ADDRESS_IP_TAG);
    if (ip_set)
    {
        ip = get<IpType>(yml, ADDRESS_IP_TAG, version);
    }

    // Optional get Domain tag for DNS
    std::string domain_name;
    bool domain_name_set = is_tag_present(yml, ADDRESS_DNS_TAG);
    if (domain_name_set)
    {
        domain_name = get_scalar<std::string>(yml, ADDRESS_DNS_TAG);
    }

    // If IP and domain_name set, warning that domain_name will not be used
    // If only domain_name set, get DNS response
    // If neither set, get default
    if (ip_set && domain_name_set)
    {
        EPROSIMA_LOG_WARNING(ddspipe_YAML,
                "Tag <" << ADDRESS_DNS_TAG << "> will not be used as <" << ADDRESS_IP_TAG << "> is set.");
        domain_name_set = false;
    }
    else if (!ip_set && !domain_name_set)
    {
        throw utils::ConfigurationException(utils::Formatter() <<
                      "Address requires to specify <" << ADDRESS_IP_TAG << "> or <" << ADDRESS_DNS_TAG << ">.");
    }

    // Optional get port
    PortType port;
    bool port_set = is_tag_present(yml, ADDRESS_PORT_TAG);
    if (port_set)
    {
        port = get<PortType>(yml, ADDRESS_PORT_TAG, version);
    }
    else
    {
        port = Address::default_port();
    }

    // WARNING: This adds logic to the parse of the entity,
    // This may not be the best place to do so. In the future move this logic to the Address class.

    // Optional get external port
    // If it is not set, same as internal port is used
    PortType external_port;
    bool external_port_set = is_tag_present(yml, ADDRESS_EXTERNAL_PORT_TAG);
    if (external_port_set)
    {
        external_port = get<PortType>(yml, ADDRESS_EXTERNAL_PORT_TAG, version);
    }
    else
    {
        external_port = port;
    }

    // Optional get Transport protocol
    TransportProtocol tp;
    bool tp_set = is_tag_present(yml, ADDRESS_TRANSPORT_TAG);
    if (tp_set)
    {
        tp = get<TransportProtocol>(yml, ADDRESS_TRANSPORT_TAG, version);
    }
    else
    {
        tp = Address::default_transport_protocol();
    }

    // Construct Address object
    if (domain_name_set)
    {
        if (ip_version_set)
        {
            return Address(port, external_port, ip_version, domain_name, tp);
        }
        else
        {
            return Address(port, external_port, domain_name, tp);
        }
    }
    else
    {
        if (ip_version_set)
        {
            return Address(ip, port, external_port, ip_version, tp);
        }
        else
        {
            return Address(ip, port, external_port, tp);
        }
    }
}

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        core::DdsPublishingConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Required enable
    object.enable = get<bool>(yml, DDS_PUBLISHING_ENABLE_TAG, version);

    // Optional domain
    if (is_tag_present(yml, DDS_PUBLISHING_DOMAIN_TAG))
    {
        object.domain = get<DomainIdType>(yml, DDS_PUBLISHING_DOMAIN_TAG, version);
    }

    // Optional topic name
    if (is_tag_present(yml, DDS_PUBLISHING_TOPIC_NAME_TAG))
    {
        object.topic_name = get<std::string>(yml, DDS_PUBLISHING_TOPIC_NAME_TAG, version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
core::DdsPublishingConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    core::DdsPublishingConfiguration object;
    fill<core::DdsPublishingConfiguration>(object, yml, version);
    return object;
}

/************************
* QoS                   *
************************/

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        TopicQoS& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Durability optional
    if (is_tag_present(yml, QOS_TRANSIENT_TAG))
    {
        if (get<bool>(yml, QOS_TRANSIENT_TAG, version))
        {
            object.durability_qos.set_value(ddspipe::core::types::DurabilityKind::TRANSIENT_LOCAL);
        }
        else
        {
            object.durability_qos.set_value(ddspipe::core::types::DurabilityKind::VOLATILE);
        }
    }

    // Optional Reliability Tag
    if (is_tag_present(yml, QOS_RELIABLE_TAG))
    {
        if (get<bool>(yml, QOS_RELIABLE_TAG, version))
        {
            object.reliability_qos.set_value(ddspipe::core::types::ReliabilityKind::RELIABLE);
        }
        else
        {
            object.reliability_qos.set_value(ddspipe::core::types::ReliabilityKind::BEST_EFFORT);
        }
    }

    // Ownership optional
    if (is_tag_present(yml, QOS_OWNERSHIP_TAG))
    {
        if (get<bool>(yml, QOS_OWNERSHIP_TAG, version))
        {
            object.ownership_qos.set_value(
                ddspipe::core::types::OwnershipQosPolicyKind::EXCLUSIVE_OWNERSHIP_QOS);
        }
        else
        {
            object.ownership_qos.set_value(ddspipe::core::types::OwnershipQosPolicyKind::SHARED_OWNERSHIP_QOS);
        }
    }

    // Use partitions optional
    if (is_tag_present(yml, QOS_PARTITION_TAG))
    {
        object.use_partitions.set_value(get<bool>(yml, QOS_PARTITION_TAG, version));
    }

    // History depth optional
    if (is_tag_present(yml, QOS_HISTORY_DEPTH_TAG))
    {
        object.history_depth.set_value(get<HistoryDepthType>(yml, QOS_HISTORY_DEPTH_TAG, version));
    }

    // Keyed optional
    if (is_tag_present(yml, QOS_KEYED_TAG))
    {
        object.keyed.set_value(get<bool>(yml, QOS_KEYED_TAG, version));
    }

    // Max Transmission Rate optional
    if (is_tag_present(yml, QOS_MAX_TX_RATE_TAG))
    {
        object.max_tx_rate.set_value(get_nonnegative_float(yml, QOS_MAX_TX_RATE_TAG));
    }

    // Max Reception Rate optional
    if (is_tag_present(yml, QOS_MAX_RX_RATE_TAG))
    {
        object.max_rx_rate.set_value(get_nonnegative_float(yml, QOS_MAX_RX_RATE_TAG));
    }

    // Downsampling optional
    if (is_tag_present(yml, QOS_DOWNSAMPLING_TAG))
    {
        object.downsampling.set_value(get_positive_int(yml, QOS_DOWNSAMPLING_TAG));
    }
}

/************************
* LOGGING CONFIGURATION *
************************/

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill<utils::LogFilter>(
        utils::LogFilter& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    if (is_tag_present(yml, LOG_FILTER_ERROR_TAG))
    {
        object[utils::VerbosityKind::Error] = get<std::string>(yml, LOG_FILTER_ERROR_TAG, version);
    }

    if (is_tag_present(yml, LOG_FILTER_WARNING_TAG))
    {
        object[utils::VerbosityKind::Warning] = get<std::string>(yml, LOG_FILTER_WARNING_TAG, version);
    }

    if (is_tag_present(yml, LOG_FILTER_INFO_TAG))
    {
        object[utils::VerbosityKind::Info] = get<std::string>(yml, LOG_FILTER_INFO_TAG, version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
utils::LogFilter YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    utils::LogFilter object;
    fill<utils::LogFilter>(object, yml, version);
    return object;
}

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        core::DdsPipeLogConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Optional publish
    if (is_tag_present(yml, LOG_PUBLISH_TAG))
    {
        object.publish = get<core::DdsPublishingConfiguration>(yml, LOG_PUBLISH_TAG, version);
    }

    // Optional stdout
    if (is_tag_present(yml, LOG_STDOUT_TAG))
    {
        object.stdout_enable = get<bool>(yml, LOG_STDOUT_TAG, version);
    }

    // Verbosity optional
    if (is_tag_present(yml, LOG_VERBOSITY_TAG))
    {
        object.verbosity = get_enumeration<utils::VerbosityKind>(
            yml,
            LOG_VERBOSITY_TAG,
                    {
                        {LOG_VERBOSITY_INFO_TAG, utils::VerbosityKind::Info},
                        {LOG_VERBOSITY_WARNING_TAG, utils::VerbosityKind::Warning},
                        {LOG_VERBOSITY_ERROR_TAG, utils::VerbosityKind::Error}
                    });
    }

    // Filter optional
    if (is_tag_present(yml, LOG_FILTER_TAG))
    {
        fill<utils::LogFilter>(object.filter, get_value_in_tag(yml, LOG_FILTER_TAG), version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
core::DdsPipeLogConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    core::DdsPipeLogConfiguration object;
    fill<core::DdsPipeLogConfiguration>(object, yml, version);
    return object;
}

/************************
* TOPICS                *
************************/

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        DdsTopic& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Name required
    object.m_topic_name = get<std::string>(yml, TOPIC_NAME_TAG, version);

    // Data Type required
    object.type_name = get<std::string>(yml, TOPIC_TYPE_NAME_TAG, version);
}

template <>
DDSPIPE_YAML_DllAPI
DdsTopic YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    DdsTopic object;
    fill<DdsTopic>(object, yml, version);
    return object;
}

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        WildcardDdsFilterTopic& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Name required
    object.topic_name = get<std::string>(yml, TOPIC_NAME_TAG, version);

    // Optional data type
    if (is_tag_present(yml, TOPIC_TYPE_NAME_TAG))
    {
        object.type_name.set_value(get<std::string>(yml, TOPIC_TYPE_NAME_TAG, version));
    }

    // Optional QoS
    if (is_tag_present(yml, TOPIC_QOS_TAG))
    {
        fill<TopicQoS>(object.topic_qos, get_value_in_tag(yml, TOPIC_QOS_TAG), version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
WildcardDdsFilterTopic YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    WildcardDdsFilterTopic object;
    fill<WildcardDdsFilterTopic>(object, yml, version);
    return object;
}

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        ManualTopic& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    auto manual_topic = YamlReader::get<WildcardDdsFilterTopic>(yml, version);
    object.first = utils::Heritable<WildcardDdsFilterTopic>::make_heritable(manual_topic);

    // Optional participants tag
    if (is_tag_present(yml, TOPIC_PARTICIPANTS_TAG))
    {
        object.second = get_set<ParticipantId>(yml, TOPIC_PARTICIPANTS_TAG, version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
ManualTopic YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    auto topic = utils::Heritable<WildcardDdsFilterTopic>::make_heritable();
    std::set<ParticipantId> participants;
    ManualTopic object = std::make_pair(topic, participants);
    fill<ManualTopic>(object, yml, version);
    return object;
}

template <>
DDSPIPE_YAML_DllAPI
utils::Heritable<DistributedTopic> YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // TODO: do not assume it is a Dds Topic
    auto topic = utils::Heritable<DdsTopic>::make_heritable();
    fill<DdsTopic>(topic.get_reference(), yml, version);
    return topic;
}

template <>
DDSPIPE_YAML_DllAPI
utils::Heritable<IFilterTopic> YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // TODO: do not assume it is a Wildcard Topic
    auto topic = utils::Heritable<WildcardDdsFilterTopic>::make_heritable();
    fill<WildcardDdsFilterTopic>(topic.get_reference(), yml, version);
    return topic;
}

/************************
* TLS CONFIGURATION     *
************************/

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        TlsConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Optional private key
    if (is_tag_present(yml, TLS_PRIVATE_KEY_TAG))
    {
        object.private_key_file = get<std::string>(yml, TLS_PRIVATE_KEY_TAG, version);
    }

    // Optional private key password
    if (is_tag_present(yml, TLS_PASSWORD_TAG))
    {
        object.private_key_file_password = get<std::string>(yml, TLS_PASSWORD_TAG, version);
    }

    // Optional certificate authority
    if (is_tag_present(yml, TLS_CA_TAG))
    {
        object.certificate_authority_file = get<std::string>(yml, TLS_CA_TAG, version);
    }

    // Optional certificate chain
    if (is_tag_present(yml, TLS_CERT_TAG))
    {
        object.certificate_chain_file = get<std::string>(yml, TLS_CERT_TAG, version);
    }

    // Optional SNI server name
    if (is_tag_present(yml, TLS_SNI_HOST_TAG))
    {
        object.sni_server_name = get<std::string>(yml, TLS_SNI_HOST_TAG, version);
    }

    // Optional dh params
    if (is_tag_present(yml, TLS_DHPARAMS_TAG))
    {
        object.dh_params_file = get<std::string>(yml, TLS_DHPARAMS_TAG, version);
    }

    // Optional peer verification
    if (is_tag_present(yml, TLS_PEER_VERIFICATION_TAG))
    {
        object.verify_peer = get<bool>(yml, TLS_PEER_VERIFICATION_TAG, version);
    }

    // Check if it should be client or server
    utils::Formatter dummy_formatter__;
    utils::Formatter error_msg_client;
    utils::Formatter error_msg_server;
    if (object.is_valid_kind<TlsKind::both>(dummy_formatter__))
    {
        object.kind = TlsKind::both;
    }
    else if (object.is_valid_kind<TlsKind::client>(error_msg_client))
    {
        object.kind = TlsKind::client;
    }
    else if (object.is_valid_kind<TlsKind::server>(error_msg_server))
    {
        object.kind = TlsKind::server;
    }
    else
    {
        throw utils::ConfigurationException(
                  STR_ENTRY << "Incorrect TLS configuration." <<
                      " Could not be client because: " << error_msg_client <<
                      " Neither server because: " << error_msg_server <<
                      "."
                  );
    }
}

template <>
DDSPIPE_YAML_DllAPI
TlsConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    TlsConfiguration object;
    fill<TlsConfiguration>(object, yml, version);
    return object;
}

std::ostream& operator <<(
        std::ostream& os,
        const YamlReaderVersion& version)
{
    switch (version)
    {
        case V_1_0:
            os << VERSION_TAG_V_1_0;
            break;

        case V_2_0:
            os << VERSION_TAG_V_2_0;
            break;

        case V_3_0:
            os << VERSION_TAG_V_3_0;
            break;

        case V_3_1:
            os << VERSION_TAG_V_3_1;
            break;

        case V_4_0:
            os << VERSION_TAG_V_4_0;
            break;

        case V_5_0:
        case LATEST:
            os << VERSION_TAG_V_5_0;
            break;

        default:
            utils::tsnh(STR_ENTRY << "Value of YamlReaderVersion out of enumeration.");
            break;
    }

    return os;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
