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


#include <fastdds/rtps/RTPSDomain.hpp>
#include <fastdds/rtps/participant/RTPSParticipant.hpp>

#include <cpp_utils/Log.hpp>

#include <ddspipe_participants/reader/rtps/SpecificQoSReader.hpp>
#include <utils/utils.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace rtps {

SpecificQoSReader::SpecificQoSReader(
        const core::types::ParticipantId& participant_id,
        const core::types::DdsTopic& topic,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        fastdds::rtps::RTPSParticipant* rtps_participant,
        const std::shared_ptr<core::DiscoveryDatabase>& discovery_database)
    : CommonReader(
        participant_id, topic, payload_pool, rtps_participant,
        reckon_history_attributes_(topic),
        reckon_reader_attributes_(topic),
        reckon_topic_description_(topic),
        reckon_reader_qos_(topic))
    , discovery_database_(discovery_database)
{
}

void SpecificQoSReader::fill_received_data_(
        const fastdds::rtps::CacheChange_t& received_change,
        core::types::RtpsPayloadData& data_to_fill) const noexcept
{
    CommonReader::fill_received_data_(received_change, data_to_fill);

    // During endpoint teardown, last cache changes can be processed after endpoint removal.
    if (!data_to_fill.source_guid.is_valid())
    {
        logDebug(
            DDSPIPE_SpecificQoSReader,
            "Skipping writer QoS lookup for invalid writer GUID " << data_to_fill.source_guid << ".");
        return;
    }

    // During teardown it is expected that late samples can outlive writer discovery data
    if (detail::try_specific_qos_of_writer_(*discovery_database_, data_to_fill.source_guid, data_to_fill.writer_qos))
    {
        logDebug(
            DDSPIPE_SpecificQoSReader,
            "Set QoS " << data_to_fill.writer_qos << " for data from " << data_to_fill.source_guid << ".");
    }
    else
    {
        logDebug(
            DDSPIPE_SpecificQoSReader,
            "Skipping writer QoS lookup for removed writer " << data_to_fill.source_guid << ".");
    }
}

} /* namespace rtps */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
