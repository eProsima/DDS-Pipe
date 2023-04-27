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
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

#include <ddspipe_participants/types/address/Address.hpp>
#include <ddspipe_participants/types/address/DiscoveryServerConnectionAddress.hpp>

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/writer/YamlWriter.hpp>
#include <ddspipe_yaml/field/YamlObjectField.hpp>
#include <ddspipe_yaml/tags/yaml_configuration_tags.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// IMPLEMENTATION OF ENUMERATIONS
////////////////////////////////////////////////

template <>
void write(
        Yaml& yml,
        const participants::types::TransportProtocol& object)
{
    // TODO get a way of not duplicating this map in write and write
    write_enumeration<participants::types::TransportProtocol>(
        yml,
        object,
        {
            {ADDRESS_TRANSPORT_TCP_TAG, participants::types::TransportProtocol::tcp},
            {ADDRESS_TRANSPORT_UDP_TAG, participants::types::TransportProtocol::udp},
        });
}

template <>
void write(
        Yaml& yml,
        const participants::types::IpVersion& object)
{
    write_enumeration<participants::types::IpVersion>(
        yml,
        object,
        {
            {ADDRESS_IP_VERSION_V4_TAG, participants::types::IpVersion::v4},
            {ADDRESS_IP_VERSION_V6_TAG, participants::types::IpVersion::v6},
        });
}

////////////////////////////////////////////////
// IMPLEMENTATION OF TYPES BY DEFAULT
////////////////////////////////////////////////

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

template <>
void write(
        Yaml& yml,
        const participants::types::DiscoveryServerConnectionAddress& object)
{
    write_from_fields(yml, object);
}

////////////////////////////////////////////////
// IMPLEMENTATION OF SPECIFIC TYPES
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
        const core::types::GuidPrefix& object)
{
    write<std::string>(yml, DISCOVERY_SERVER_GUID_TAG, utils::generic_to_string(object));
}

template <>
void write(
        Yaml& yml,
        const participants::types::Address& object)
{
    write<participants::types::IpType>(yml, ADDRESS_IP_TAG, object.ip());
    write<participants::types::PortType>(yml, ADDRESS_PORT_TAG, object.port());
    write<participants::types::IpVersion>(yml, ADDRESS_IP_VERSION_TAG, object.ip_version());
    write<participants::types::TransportProtocol>(yml, ADDRESS_TRANSPORT_TAG, object.transport_protocol());

    if (object.port() != object.external_port())
    {
        write<participants::types::PortType>(yml, ADDRESS_PORT_TAG, object.external_port());
    }
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
