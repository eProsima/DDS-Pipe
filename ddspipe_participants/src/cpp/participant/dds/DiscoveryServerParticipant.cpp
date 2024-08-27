// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <fastdds/rtps/transport/UDPv4TransportDescriptor.hpp>
#include <fastdds/rtps/transport/UDPv6TransportDescriptor.hpp>
#include <fastdds/rtps/transport/TCPv4TransportDescriptor.hpp>
#include <fastdds/rtps/transport/TCPv6TransportDescriptor.hpp>

#include <cpp_utils/exception/ConfigurationException.hpp>
#include <cpp_utils/utils.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_participants/types/security/tls/TlsConfiguration.hpp>

#include <ddspipe_participants/participant/dds/DiscoveryServerParticipant.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

DiscoveryServerParticipant::DiscoveryServerParticipant(
        const std::shared_ptr<DiscoveryServerParticipantConfiguration>& participant_configuration,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        const std::shared_ptr<core::DiscoveryDatabase>& discovery_database)
    : CommonParticipant(
        participant_configuration,
        payload_pool,
        discovery_database,
        reckon_participant_qos_()),
      configuration_(*reinterpret_cast<DiscoveryServerParticipantConfiguration*>(participant_configuration.get()))
{
}

fastdds::dds::DomainParticipantQos
DiscoveryServerParticipant::reckon_participant_qos_() const
{
    // Use default as base qos
    fastdds::dds::DomainParticipantQos pqos = CommonParticipant::reckon_participant_qos_();

    // Auxiliary variable to save characters and improve readability
    const core::types::GuidPrefix& discovery_server_guid_prefix = configuration_.discovery_server_guid_prefix;
    const auto& tls_config = configuration_.tls_configuration;

    // Needed values to check at the end if descriptor must be set
    bool has_listening_tcp_ipv4 = false;
    bool has_listening_tcp_ipv6 = false;
    bool has_listening_udp_ipv4 = false;
    bool has_listening_udp_ipv6 = false;
    bool has_connection_descriptor = false;

    pqos.transport().use_builtin_transports = false;

    /////
    // Set listening addresses
    for (types::Address address : configuration_.listening_addresses)
    {
        if (!address.is_valid())
        {
            // Invalid address, continue with next one
            EPROSIMA_LOG_WARNING(DDSPIPE_DISCOVERYSERVER_PARTICIPANT,
                    "Discard listening address: " << address <<
                    " in Participant " << configuration_.id << " initialization.");
            continue;
        }

        // Create DS SERVER locator
        eprosima::fastdds::rtps::Locator listening_locator;
        listening_locator.kind = address.get_locator_kind();
        eprosima::fastdds::rtps::IPLocator::setPhysicalPort(listening_locator, address.port());

        std::shared_ptr<eprosima::fastdds::rtps::TransportDescriptorInterface> descriptor;

        switch (address.get_locator_kind())
        {
            case LOCATOR_KIND_UDPv4:
            {
                has_listening_udp_ipv4 = true;

                auto descriptor_tmp =
                        create_descriptor<eprosima::fastdds::rtps::UDPv4TransportDescriptor>(configuration_.whitelist);
                descriptor = descriptor_tmp;

                eprosima::fastdds::rtps::IPLocator::setIPv4(listening_locator, address.ip());

                break;
            }

            case LOCATOR_KIND_UDPv6:
            {
                has_listening_udp_ipv6 = true;

                auto descriptor_tmp =
                        create_descriptor<eprosima::fastdds::rtps::UDPv6TransportDescriptor>(configuration_.whitelist);
                descriptor = descriptor_tmp;

                eprosima::fastdds::rtps::IPLocator::setIPv6(listening_locator, address.ip());

                break;
            }

            case LOCATOR_KIND_TCPv4:
            {
                has_listening_tcp_ipv4 = true;

                std::shared_ptr<eprosima::fastdds::rtps::TCPv4TransportDescriptor> descriptor_tmp;

                // We check if several descriptors share a WAN address.
                // If so, we add a new port to the previously created descriptor.
                bool same_wan_addr = false;

                auto it = pqos.transport().user_transports.begin();
                while (it != pqos.transport().user_transports.end())
                {
                    std::shared_ptr<eprosima::fastdds::rtps::TCPv4TransportDescriptor> tmp_descriptor =
                            std::dynamic_pointer_cast<eprosima::fastdds::rtps::TCPv4TransportDescriptor>(*it);

                    if ((tmp_descriptor != nullptr) && (address.ip() == tmp_descriptor->get_WAN_address()))
                    {
                        // Save in the new descriptor the previously added descriptor with the same WAN address
                        descriptor_tmp = tmp_descriptor;
                        // Set that a descriptor with same WAN address was found
                        same_wan_addr = true;
                        // Remove the previously added descriptor as this will be replaced by the same one updated with
                        // more locators.
                        pqos.transport().user_transports.erase(it);
                        break;
                    }
                }

                // Add the new locator to the descriptor if another with the same wan address was found
                if (same_wan_addr)
                {
                    descriptor_tmp->add_listener_port(address.port());
                }
                else
                {
                    descriptor_tmp =
                            create_descriptor<eprosima::fastdds::rtps::TCPv4TransportDescriptor>(
                        configuration_.whitelist);

                    descriptor_tmp->add_listener_port(address.port());
                    descriptor_tmp->set_WAN_address(address.ip());

                    // Enable TLS
                    if (tls_config.is_active())
                    {
                        tls_config.enable_tls(descriptor_tmp);
                    }
                }

                descriptor = descriptor_tmp;

                eprosima::fastdds::rtps::IPLocator::setPhysicalPort(listening_locator, address.external_port());
                eprosima::fastdds::rtps::IPLocator::setLogicalPort(listening_locator, address.external_port());
                eprosima::fastdds::rtps::IPLocator::setIPv4(listening_locator, address.ip());

                break;
            }

            case LOCATOR_KIND_TCPv6:
            {
                has_listening_tcp_ipv6 = true;

                auto descriptor_tmp =
                        create_descriptor<eprosima::fastdds::rtps::TCPv6TransportDescriptor>(configuration_.whitelist);
                descriptor_tmp->add_listener_port(address.port());

                // Enable TLS
                if (tls_config.is_active())
                {
                    tls_config.enable_tls(descriptor_tmp);
                }

                descriptor = descriptor_tmp;

                eprosima::fastdds::rtps::IPLocator::setPhysicalPort(listening_locator, address.external_port());
                eprosima::fastdds::rtps::IPLocator::setLogicalPort(listening_locator, address.external_port());
                eprosima::fastdds::rtps::IPLocator::setIPv6(listening_locator, address.ip());

                break;
            }

            default:
                break;
        }

        // Add descriptor
        pqos.transport().user_transports.push_back(descriptor);

        // Set participant as SERVER
        pqos.wire_protocol().builtin.discovery_config.discoveryProtocol =
                eprosima::fastdds::rtps::DiscoveryProtocol::SERVER;

        // Set SERVER's listening locator for PDP
        pqos.wire_protocol().builtin.metatrafficUnicastLocatorList.push_back(listening_locator);
        pqos.wire_protocol().default_unicast_locator_list.push_back(listening_locator);

        logDebug(DDSPIPE_DISCOVERYSERVER_PARTICIPANT,
                "Add listening address " << address << " to Participant " << configuration_.id << ".");
    }

    /////
    // Set connection addresses
    for (types::DiscoveryServerConnectionAddress connection_address : configuration_.connection_addresses)
    {
        if (!connection_address.is_valid())
        {
            // Invalid connection address, continue with next one
            EPROSIMA_LOG_WARNING(DDSPIPE_DISCOVERYSERVER_PARTICIPANT,
                    "Discard connection address with remote server in Participant " <<
                    configuration_.id << " initialization.");
            continue;
        }

        for (types::Address address : connection_address.addresses())
        {
            if (!address.is_valid())
            {
                // Invalid ip address, continue with next one
                EPROSIMA_LOG_WARNING(DDSPIPE_DISCOVERYSERVER_PARTICIPANT,
                        "Discard connection address with remote server due to invalid ip address " <<
                        address.ip() << " in Participant " << configuration_.id <<
                        " initialization.");
                continue;
            }

            // Create DS locator
            eprosima::fastdds::rtps::Locator server_locator;
            server_locator.kind = address.get_locator_kind();
            eprosima::fastdds::rtps::IPLocator::setPhysicalPort(server_locator, address.port());

            std::shared_ptr<eprosima::fastdds::rtps::TransportDescriptorInterface> descriptor;

            switch (address.get_locator_kind())
            {
                case LOCATOR_KIND_UDPv4:
                {
                    if (!has_listening_udp_ipv4)
                    {
                        has_connection_descriptor = true;
                        auto descriptor_tmp =
                                create_descriptor<eprosima::fastdds::rtps::UDPv4TransportDescriptor>(
                            configuration_.whitelist);
                        // descriptor_tmp->interfaceWhiteList.push_back(address.ip());
                        descriptor = descriptor_tmp;
                    }

                    eprosima::fastdds::rtps::IPLocator::setIPv4(server_locator, address.ip());

                    break;
                }

                case LOCATOR_KIND_UDPv6:
                {
                    if (!has_listening_udp_ipv6)
                    {
                        has_connection_descriptor = true;
                        auto descriptor_tmp =
                                create_descriptor<eprosima::fastdds::rtps::UDPv6TransportDescriptor>(
                            configuration_.whitelist);
                        // descriptor_tmp->interfaceWhiteList.push_back(address.ip());
                        descriptor = descriptor_tmp;
                    }

                    eprosima::fastdds::rtps::IPLocator::setIPv6(server_locator, address.ip());

                    break;
                }

                case LOCATOR_KIND_TCPv4:
                {
                    if (!has_listening_tcp_ipv4)
                    {
                        has_connection_descriptor = true;
                        auto descriptor_tmp =
                                create_descriptor<eprosima::fastdds::rtps::TCPv4TransportDescriptor>(
                            configuration_.whitelist);
                        descriptor_tmp->add_listener_port(0);
                        // descriptor_tmp->interfaceWhiteList.push_back(address.ip());

                        // Enable TLS
                        if (tls_config.is_active())
                        {
                            tls_config.enable_tls(descriptor_tmp, true);
                        }

                        descriptor = descriptor_tmp;
                    }

                    eprosima::fastdds::rtps::IPLocator::setLogicalPort(server_locator, address.port());
                    eprosima::fastdds::rtps::IPLocator::setIPv4(server_locator, address.ip());

                    break;
                }

                case LOCATOR_KIND_TCPv6:
                {
                    if (!has_listening_tcp_ipv6)
                    {
                        has_connection_descriptor = true;
                        auto descriptor_tmp =
                                create_descriptor<eprosima::fastdds::rtps::TCPv6TransportDescriptor>(
                            configuration_.whitelist);
                        // descriptor_tmp->add_listener_port(0);
                        descriptor_tmp->interfaceWhiteList.push_back(address.ip());

                        // Enable TLS
                        if (tls_config.is_active())
                        {
                            tls_config.enable_tls(descriptor_tmp, true);
                        }

                        descriptor = descriptor_tmp;
                    }

                    eprosima::fastdds::rtps::IPLocator::setLogicalPort(server_locator, address.port());
                    eprosima::fastdds::rtps::IPLocator::setIPv6(server_locator, address.ip());

                    break;
                }

                default:
                    break;
            }

            if (has_connection_descriptor)
            {
                // Set participant as DS CLIENT
                pqos.wire_protocol().builtin.discovery_config.discoveryProtocol =
                        eprosima::fastdds::rtps::DiscoveryProtocol::SUPER_CLIENT;

                // Add descriptor
                pqos.transport().user_transports.push_back(descriptor);

                logDebug(DDSPIPE_DISCOVERYSERVER_PARTICIPANT,
                        "Add connection address " << address << " to Participant " << configuration_.id << ".");
            }

            // Add remote SERVER to CLIENT's list of SERVERs
            pqos.wire_protocol().builtin.discovery_config.m_DiscoveryServers.push_back(server_locator);
        }
    }

    /////
    // Set Server Guid
    // params.prefix = discovery_server_guid_prefix; TODO Irene: Check if this is needed

    logDebug(DDSPIPE_DISCOVERYSERVER_PARTICIPANT,
            "Configured Participant " << configuration_.id << " with server guid: " <<
            discovery_server_guid_prefix);

    return pqos;
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
