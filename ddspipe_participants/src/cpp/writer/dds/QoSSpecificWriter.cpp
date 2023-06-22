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


#include <ddspipe_participants/writer/dds/QoSSpecificWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

using namespace eprosima::ddspipe::core::types;

QoSSpecificWriter::QoSSpecificWriter(
        const ParticipantId& participant_id,
        const DdsTopic& topic,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        const core::types::SpecificEndpointQoS& specific_qos,
        fastdds::dds::DomainParticipant* participant,
        fastdds::dds::Topic* topic_entity)
    : CommonWriter(
        participant_id, topic, payload_pool, participant, topic_entity)
    , specific_qos_(specific_qos)
{
}

fastdds::dds::PublisherQos QoSSpecificWriter::reckon_publisher_qos_() const noexcept
{
    // NOTE: this does not handle different partitions
    fastdds::dds::PublisherQos qos = dds_participant_->get_default_publisher_qos();

    if (topic_.topic_qos.has_partitions())
    {
        qos.partition(specific_qos_.partitions);
    }

    return qos;
}

fastdds::dds::DataWriterQos QoSSpecificWriter::reckon_writer_qos_() const noexcept
{
    // Get QoS from parent class
    fastdds::dds::DataWriterQos qos = CommonWriter::reckon_writer_qos_();

    // Set Ownership
    if (topic_.topic_qos.has_ownership())
    {
        // Set ownership
        qos.ownership_strength(specific_qos_.ownership_strength);
    }

    return qos;
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
