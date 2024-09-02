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

#pragma once

#include <fastdds/rtps/transport/TCPTransportDescriptor.hpp>

#include <ddspipe_participants/configuration/InitialPeersParticipantConfiguration.hpp>
#include <ddspipe_participants/participant/dds/CommonParticipant.hpp>
#include <ddspipe_participants/types/security/tls/TlsConfiguration.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

/**
 * TODO
 */
class InitialPeersParticipant
    : public CommonParticipant
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    InitialPeersParticipant(
            const std::shared_ptr<InitialPeersParticipantConfiguration>& participant_configuration,
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            const std::shared_ptr<core::DiscoveryDatabase>& discovery_database);

protected:

    /////
    // DDS specific methods

    /**
     * @brief Static method that gives the QoS for a Initial Peers Participant.
     *
     */
    static fastdds::dds::DomainParticipantQos reckon_participant_qos_(
        const InitialPeersParticipantConfiguration* participant_configuration);
};

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */