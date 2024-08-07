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

#include <fastdds/rtps/participant/RTPSParticipant.hpp>
#include <fastdds/rtps/RTPSDomain.hpp>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.hpp>
#include <fastdds/rtps/attributes/RTPSParticipantAttributes.hpp>

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
        reckon_participant_attributes_(participant_configuration.get()))
{
}

fastdds::rtps::RTPSParticipantAttributes
SimpleParticipant::reckon_participant_attributes_(
        const SimpleParticipantConfiguration* configuration)
{
    // Use default as base attributes
    fastdds::rtps::RTPSParticipantAttributes params = CommonParticipant::reckon_participant_attributes_(configuration);

    // Configure Participant transports
    if (configuration->transport == core::types::TransportDescriptors::builtin)
    {
        if (!configuration->whitelist.empty())
        {
            params.useBuiltinTransports = false;

            std::shared_ptr<eprosima::fastdds::rtps::SharedMemTransportDescriptor> shm_transport =
                    std::make_shared<eprosima::fastdds::rtps::SharedMemTransportDescriptor>();
            params.userTransports.push_back(shm_transport);

            std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
                    create_descriptor<eprosima::fastdds::rtps::UDPv4TransportDescriptor>(configuration->whitelist);
            params.userTransports.push_back(udp_transport);
        }
    }
    else if (configuration->transport == core::types::TransportDescriptors::shm_only)
    {
        params.useBuiltinTransports = false;

        std::shared_ptr<eprosima::fastdds::rtps::SharedMemTransportDescriptor> shm_transport =
                std::make_shared<eprosima::fastdds::rtps::SharedMemTransportDescriptor>();
        params.userTransports.push_back(shm_transport);
    }
    else if (configuration->transport == core::types::TransportDescriptors::udp_only)
    {
        params.useBuiltinTransports = false;

        std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
                create_descriptor<eprosima::fastdds::rtps::UDPv4TransportDescriptor>(configuration->whitelist);
        params.userTransports.push_back(udp_transport);
    }

    // Participant discovery filter configuration
    switch (configuration->ignore_participant_flags)
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

    return params;
}

} /* namespace rtps */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
