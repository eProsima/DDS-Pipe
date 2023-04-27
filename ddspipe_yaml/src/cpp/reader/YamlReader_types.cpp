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

#include <ddspipe_core/types/dds/GuidPrefix.hpp>
#include <ddspipe_core/types/dds/TopicQoS.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

#include <ddspipe_participants/types/address/Address.hpp>
#include <ddspipe_participants/types/address/DiscoveryServerConnectionAddress.hpp>

#include <ddspipe_yaml/field/YamlObjectField.hpp>
#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/reader/YamlReader.hpp>
#include <ddspipe_yaml/tags/yaml_configuration_tags.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// IMPLEMENTATION OF ENUMERATIONS
////////////////////////////////////////////////

template <>
void read(
        const Yaml& yml,
        participants::types::TransportProtocol& object)
{
    // TODO get a way of not duplicating this map in write and read
    read_enumeration<participants::types::TransportProtocol>(
        yml,
        {
            {ADDRESS_TRANSPORT_TCP_TAG, participants::types::TransportProtocol::tcp},
            {ADDRESS_TRANSPORT_UDP_TAG, participants::types::TransportProtocol::udp},
        });
}

template <>
void read(
        const Yaml& yml,
        participants::types::IpVersion& object)
{
    read_enumeration<participants::types::IpVersion>(
        yml,
        {
            {ADDRESS_IP_VERSION_V4_TAG, participants::types::IpVersion::v4},
            {ADDRESS_IP_VERSION_V6_TAG, participants::types::IpVersion::v6},
        });
}

////////////////////////////////////////////////
// IMPLEMENTATION OF TYPES BY DEFAULT
////////////////////////////////////////////////

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

template <>
void read(
        const Yaml& yml,
        participants::types::DiscoveryServerConnectionAddress& object)
{
    read_from_fields(yml, object);
}

////////////////////////////////////////////////
// IMPLEMENTATION OF SPECIFIC TYPES
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
        core::types::GuidPrefix& object)
{
    // If guid exists, use it. Non mandatory.
    if (is_tag_present(yml, DISCOVERY_SERVER_GUID_TAG))
    {
        std::string guid = read<std::string>(yml, DISCOVERY_SERVER_GUID_TAG);
        object = core::types::GuidPrefix(guid);
        return;
    }

    // ROS DS is optional.
    bool ros_id;
    bool ros_id_set = is_tag_present(yml, DISCOVERY_SERVER_ID_ROS_TAG);
    if (ros_id_set)
    {
        ros_id = read<bool>(yml, DISCOVERY_SERVER_ID_ROS_TAG);
    }

    // Id is mandatory if guid is not present
    uint32_t id = read<uint32_t>(yml, DISCOVERY_SERVER_ID_TAG);

    // Create GuidPrefix
    if (ros_id_set)
    {
        object = core::types::GuidPrefix(ros_id, id);
    }
    else
    {
        object = core::types::GuidPrefix(id);
    }
}

template <>
void read(
        const Yaml& yml,
        participants::types::Address& object)
{
    // Optional get IP version
    participants::types::IpVersion ip_version;
    bool ip_version_set = is_tag_present(yml, ADDRESS_IP_VERSION_TAG);
    if (ip_version_set)
    {
        // Get IP Version from enumeration
        ip_version = read<participants::types::IpVersion>(yml, ADDRESS_IP_VERSION_TAG);
    }

    // Optional get IP
    participants::types::IpType ip;
    bool ip_set = is_tag_present(yml, ADDRESS_IP_TAG);
    if (ip_set)
    {
        ip = read<participants::types::IpType>(yml, ADDRESS_IP_TAG);
    }

    // Optional get Domain tag for DNS
    std::string domain_name;
    bool domain_name_set = is_tag_present(yml, ADDRESS_DNS_TAG);
    if (domain_name_set)
    {
        domain_name = read<std::string>(yml, ADDRESS_DNS_TAG);
    }

    // If IP and domain_name set, warning that domain_name will not be used
    // If only domain_name set, get DNS response
    // If neither set, get default
    if (ip_set && domain_name_set)
    {
        logWarning(ddspipe_YAML,
                "Tag <" << ADDRESS_DNS_TAG << "> will not be used as <" << ADDRESS_IP_TAG << "> is set.");
        domain_name_set = false;
    }
    else if (!ip_set && !domain_name_set)
    {
        throw eprosima::utils::ConfigurationException(utils::Formatter() <<
                      "Address requires to specify <" << ADDRESS_IP_TAG << "> or <" << ADDRESS_DNS_TAG << ">.");
    }

    // Optional get port
    participants::types::PortType port;
    bool port_set = is_tag_present(yml, ADDRESS_PORT_TAG);
    if (port_set)
    {
        port = read<participants::types::PortType>(yml, ADDRESS_PORT_TAG);
    }
    else
    {
        port = participants::types::Address::default_port();
    }

    // WARNING: This adds logic to the parse of the entity,
    // This may not be the best place to do so. In the future move this logic to the Address class.

    // Optional get external port
    // If it is not set, same as internal port is used
    participants::types::PortType external_port;
    bool external_port_set = is_tag_present(yml, ADDRESS_EXTERNAL_PORT_TAG);
    if (external_port_set)
    {
        external_port = read<participants::types::PortType>(yml, ADDRESS_EXTERNAL_PORT_TAG);
    }
    else
    {
        external_port = port;
    }

    // Optional get Transport protocol
    participants::types::TransportProtocol tp;
    bool tp_set = is_tag_present(yml, ADDRESS_TRANSPORT_TAG);
    if (tp_set)
    {
        tp = read<participants::types::TransportProtocol>(yml, ADDRESS_TRANSPORT_TAG);
    }
    else
    {
        tp = participants::types::Address::default_transport_protocol();
    }

    // Construct Address object
    if (domain_name_set)
    {
        if (ip_version_set)
        {
            object = participants::types::Address(port, external_port, ip_version, domain_name, tp);
        }
        else
        {
            object = participants::types::Address(port, external_port, domain_name, tp);
        }
    }
    else
    {
        if (ip_version_set)
        {
            object = participants::types::Address(ip, port, external_port, ip_version, tp);
        }
        else
        {
            object = participants::types::Address(ip, port, external_port, tp);
        }
    }
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
