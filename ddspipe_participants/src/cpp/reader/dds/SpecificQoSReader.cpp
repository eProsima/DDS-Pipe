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


#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/participant/RTPSParticipant.h>

#include <cpp_utils/exception/InconsistencyException.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_participants/reader/dds/SpecificQoSReader.hpp>
#include <utils/utils.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

SpecificQoSReader::SpecificQoSReader(
        const core::types::ParticipantId& participant_id,
        const core::types::DdsTopic& topic,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        fastdds::dds::DomainParticipant* participant,
        fastdds::dds::Topic* topic_entity,
        const std::shared_ptr<core::DiscoveryDatabase>& discovery_database)
    : CommonReader(
        participant_id, topic, payload_pool, participant, topic_entity)
    , discovery_database_(discovery_database)
{
}

void SpecificQoSReader::fill_received_data_(
        const fastdds::dds::SampleInfo& info,
        core::types::RtpsPayloadData& data_to_fill) const noexcept
{
    CommonReader::fill_received_data_(info, data_to_fill);

    // Find qos of writer
    try
    {
        data_to_fill.writer_qos = detail::specific_qos_of_writer_(*discovery_database_, data_to_fill.source_guid);
        logDebug(
            DDSPIPE_SpecificQoSReader,
            "Set QoS " << data_to_fill.writer_qos << " for data from " << data_to_fill.source_guid << ".");
    }
    catch (const utils::InconsistencyException& e)
    {
        // Get a message from a writer not in database, this is an error.
        // Remove data and make as it has not been received.
        logError(
            DDSPIPE_SpecificQoSReader,
            "Received a message from Writer " << data_to_fill.source_guid << " that is not stored in DB.");
    }
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
