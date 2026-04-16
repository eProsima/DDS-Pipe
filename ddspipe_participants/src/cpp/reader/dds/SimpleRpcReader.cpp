// Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Log.hpp>
#include <ddspipe_core/types/data/RpcPayloadData.hpp>
#include <ddspipe_participants/reader/dds/SimpleRpcReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

SimpleRpcReader::SimpleRpcReader(
        const core::types::ParticipantId& participant_id,
        const core::types::DdsTopic& topic,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        fastdds::dds::DomainParticipant* participant,
        fastdds::dds::Topic* topic_entity)
    : SimpleReader(
        participant_id, topic, payload_pool, participant, topic_entity)
{
    // Do nothing
}

//! Override Parent method to create an RPC data type.
core::types::RtpsPayloadData* SimpleRpcReader::create_data_() const noexcept
{
    return new core::types::RpcPayloadData();
}

//! Override Parent method to fill fields exclusive from RPC.
void SimpleRpcReader::fill_received_data_(
        const fastdds::dds::SampleInfo& info,
        core::types::RtpsPayloadData& data_to_fill) const noexcept
{
    SimpleReader::fill_received_data_(info, data_to_fill);

    // Get internal RpcPayload
    core::types::RpcPayloadData& rpc_data = dynamic_cast<core::types::RpcPayloadData&>(data_to_fill);

    // Set write params and origin sequence number
    fastdds::rtps::WriteParams write_params{};
    write_params.sample_identity(info.related_sample_identity); // To behave as RTPS
    rpc_data.write_params.set_value(write_params);
    rpc_data.origin_sequence_number = info.sample_identity.sequence_number();
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
