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
#include <fastdds/rtps/common/CacheChange.hpp>

#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/types/data/RpcPayloadData.hpp>

#include <ddspipe_participants/efficiency/cache_change/CacheChangePool.hpp>
#include <ddspipe_participants/types/dds/RouterCacheChange.hpp>
#include <ddspipe_participants/writer/dds/SimpleRpcWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

SimpleRpcWriter::SimpleRpcWriter(
        const core::types::ParticipantId& participant_id,
        const core::types::DdsTopic& topic,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        fastdds::dds::DomainParticipant* participant,
        fastdds::dds::Topic* topic_entity,
        const bool repeater /* = false */)
    : SimpleWriter(
        participant_id, topic, payload_pool, participant, topic_entity, repeater)
{
    // Do nothing
}

utils::ReturnCode SimpleRpcWriter::fill_to_send_data_(
        fastdds::rtps::WriteParams& to_send_params,
        const core::types::RtpsPayloadData& data) const noexcept
{
    SimpleWriter::fill_to_send_data_(
        to_send_params,
        data);

    const core::types::RpcPayloadData& rpc_data = dynamic_cast<const core::types::RpcPayloadData&>(data);
    if (!rpc_data.write_params.is_set())
    {
        EPROSIMA_LOG_ERROR(DDSPIPE_RPC_WRITER,
                "RPC Writer in Participant " << participant_id_ <<
                " for topic " << topic_ <<
                " trying to send data without write_params set.");
        return utils::ReturnCode::RETCODE_ERROR;
    }

    // Set RPC specific params
    to_send_params.related_sample_identity(rpc_data.write_params.get_reference().related_sample_identity());
    to_send_params.has_more_replies(rpc_data.write_params.get_reference().has_more_replies());

    return utils::ReturnCode::RETCODE_OK;
}

void SimpleRpcWriter::fill_sent_data_(
        const fastdds::rtps::WriteParams& sent_params,
        core::types::RtpsPayloadData& data_to_fill) const noexcept
{
    SimpleWriter::fill_sent_data_(
        sent_params,
        data_to_fill);

    core::types::RpcPayloadData& rpc_data = dynamic_cast<core::types::RpcPayloadData&>(data_to_fill);
    rpc_data.sent_sequence_number = sent_params.sample_identity().sequence_number();
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
