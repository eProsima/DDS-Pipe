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

#pragma once

#include <ddspipe_core/dynamic/DiscoveryDatabase.hpp>

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/reader/dds/CommonReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

/**
 * DDS DataReader with specific QoS implements abstract CommonReader.
 *
 * This class fills the data receive information with the QoS of the Writer that has sent the data.
 * In order to access this QoS it has a reference to the DiscoveryDatabase.
 */
class SpecificQoSReader : public CommonReader
{
public:

    /**
     * @brief Construct a new SpecificQoSReader object
     *
     * Get the Attributes and TopicQoS and create the SpecificQoSReader History and the DDS SpecificQoSReader.
     *
     * @param participant_id    Router Id of the Participant that has created this SpecificQoSReader.
     * @param topic             Topic that this SpecificQoSReader subscribes to.
     * @param payload_pool      Shared Payload Pool to received data and take it.
     * @param subscriber  DDS Subscriber
     * @param topic_entity  DDS Topic
     *
     * @throw \c InitializationException in case any creation has failed
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    SpecificQoSReader(
            const core::types::ParticipantId& participant_id,
            const core::types::DdsTopic& topic,
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            fastdds::dds::DomainParticipant* participant,
            fastdds::dds::Topic* topic_entity,
            const std::shared_ptr<core::DiscoveryDatabase>& discovery_database);

protected:

    /**
     * Specializes \c CommonReader method and set the QoS of the data received.
     */
    virtual void fill_received_data_(
            const fastdds::dds::SampleInfo& info,
            core::types::RtpsPayloadData& data_to_fill) const noexcept override;

    //! Reference to the \c DiscoveryDatabase .
    std::shared_ptr<core::DiscoveryDatabase> discovery_database_;

};

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
