// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <memory>

#include <fastdds/rtps/attributes/RTPSParticipantAttributes.hpp>
#include <fastdds/rtps/participant/RTPSParticipant.hpp>
#include <fastdds/rtps/RTPSDomain.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.hpp>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.hpp>
#include <fastdds/utils/IPLocator.hpp>

#include <cpp_utils/exception/ConfigurationException.hpp>

#include <ddspipe_participants/participant/rtps/SimpleParticipant.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace rtps {

SimpleParticipant::SimpleParticipant(
        const std::shared_ptr<SimpleParticipantConfiguration>& participant_configuration,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        const std::shared_ptr<core::DiscoveryDatabase>& discovery_database)
    : CommonParticipant(
        participant_configuration,
        payload_pool,
        discovery_database,
        participant_configuration->domain,
        participant_configuration->allowed_partition_list)
{
}

fastdds::rtps::RTPSParticipantAttributes
SimpleParticipant::reckon_participant_attributes_() const
{
    // Use default as base attributes
    fastdds::rtps::RTPSParticipantAttributes params = CommonParticipant::reckon_participant_attributes_();

    std::shared_ptr<SimpleParticipantConfiguration> simple_configuration =
            std::dynamic_pointer_cast<SimpleParticipantConfiguration>(configuration_);

    if (simple_configuration == nullptr)
    {
        throw utils::ConfigurationException("Failed to cast ParticipantConfiguration to SimpleParticipantConfiguration.");
    }

    // Configure Participant transports
    if (simple_configuration->transport == core::types::TransportDescriptors::builtin)
    {
        if (!simple_configuration->whitelist.empty())
        {
            params.useBuiltinTransports = false;

            std::shared_ptr<eprosima::fastdds::rtps::SharedMemTransportDescriptor> shm_transport =
                    std::make_shared<eprosima::fastdds::rtps::SharedMemTransportDescriptor>();
            params.userTransports.push_back(shm_transport);

            std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
                    create_descriptor<eprosima::fastdds::rtps::UDPv4TransportDescriptor>(simple_configuration->whitelist);
            params.userTransports.push_back(udp_transport);
        }
    }
    else if (simple_configuration->transport == core::types::TransportDescriptors::shm_only)
    {
        params.useBuiltinTransports = false;

        std::shared_ptr<eprosima::fastdds::rtps::SharedMemTransportDescriptor> shm_transport =
                std::make_shared<eprosima::fastdds::rtps::SharedMemTransportDescriptor>();
        params.userTransports.push_back(shm_transport);
    }
    else if (simple_configuration->transport == core::types::TransportDescriptors::udp_only)
    {
        params.useBuiltinTransports = false;

        std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
                create_descriptor<eprosima::fastdds::rtps::UDPv4TransportDescriptor>(simple_configuration->whitelist);
        params.userTransports.push_back(udp_transport);
    }

    // Participant discovery filter configuration
    switch (simple_configuration->ignore_participant_flags)
    {
        case core::types::IgnoreParticipantFlags::no_filter:
            params.builtin.discovery_config.ignoreParticipantFlags =
                    eprosima::fastdds::rtps::ParticipantFilteringFlags::NO_FILTER;
            break;
        case core::types::IgnoreParticipantFlags::filter_different_host:
            params.builtin.discovery_config.ignoreParticipantFlags =
                    eprosima::fastdds::rtps::ParticipantFilteringFlags::FILTER_DIFFERENT_HOST;
            break;
        case core::types::IgnoreParticipantFlags::filter_different_process:
            params.builtin.discovery_config.ignoreParticipantFlags =
                    eprosima::fastdds::rtps::ParticipantFilteringFlags::FILTER_DIFFERENT_PROCESS;
            break;
        case core::types::IgnoreParticipantFlags::filter_same_process:
            params.builtin.discovery_config.ignoreParticipantFlags =
                    eprosima::fastdds::rtps::ParticipantFilteringFlags::FILTER_SAME_PROCESS;
            break;
        case core::types::IgnoreParticipantFlags::filter_different_and_same_process:
            params.builtin.discovery_config.ignoreParticipantFlags =
                    static_cast<eprosima::fastdds::rtps::ParticipantFilteringFlags>(
                eprosima::fastdds::rtps::ParticipantFilteringFlags::FILTER_DIFFERENT_PROCESS |
                eprosima::fastdds::rtps::ParticipantFilteringFlags::FILTER_SAME_PROCESS);
            break;
        default:
            break;
    }

    // Easy mode IP configuration
    params.easy_mode_ip = simple_configuration->easy_mode_ip;

    return params;
}

} /* namespace rtps */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
