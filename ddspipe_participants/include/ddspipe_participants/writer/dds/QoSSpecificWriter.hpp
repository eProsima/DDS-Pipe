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

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/writer/dds/CommonWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

/**
 * DDS DataWriter with specific QoS implements abstract CommonWriter.
 *
 * This class implements a DDS DataWriter with specific QoS policies.
 */
class QoSSpecificWriter : public CommonWriter
{
public:

    /**
     * @brief Construct a new QoSSpecificWriter object
     *
     * Get the Attributes and TopicQoS and create the QoSSpecificWriter History and the RTPS QoSSpecificWriter.
     *
     * @param participant_id    Router Id of the Participant that has created this QoSSpecificWriter.
     * @param topic             Topic that this QoSSpecificWriter subscribes to.
     * @param payload_pool      Shared Payload Pool to received data and take it.
     * @param participant       DDS Participant pointer.
     * @param topic_entity      DDS Topic pointer.
     *
     * @throw \c InitializationException in case any creation has failed
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    QoSSpecificWriter(
            const core::types::ParticipantId& participant_id,
            const core::types::DdsTopic& topic,
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            const core::types::SpecificEndpointQoS& specific_qos,
            fastdds::dds::DomainParticipant* participant,
            fastdds::dds::Topic* topic_entity);

protected:

    virtual
    fastdds::dds::PublisherQos
    reckon_publisher_qos_() const noexcept override;

    //! Specific writer QoS to override (more or less) the CommonWriter qos
    virtual
    fastdds::dds::DataWriterQos
    reckon_writer_qos_() const noexcept override;

    //! Specific QoS of the Endpoint
    core::types::SpecificEndpointQoS specific_qos_;
};

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
