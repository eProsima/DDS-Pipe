// Copyright 2025 Sony Group Corporation.
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

#include <fastdds/dds/log/Log.hpp>
#include <fastdds/rtps/RTPSDomain.hpp>
#include <fastdds/rtps/participant/RTPSParticipant.hpp>

#include <ddspipe_core/types/data/RpcPayloadData.hpp>
#include <ddspipe_core/types/data/RtpsPayloadData.hpp>

#include <ddspipe_participants/reader/rpc/DdsSimpleReader.hpp>
#include <cpp_utils/exception/InitializationException.hpp>

#include <utils/utils.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace rpc {

DdsSimpleReader::DdsSimpleReader(
        const core::types::ParticipantId& participant_id,
        const core::types::DdsTopic& topic,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        fastdds::dds::DomainParticipant* participant,
        fastdds::dds::Topic* topic_entity)
    : CommonReader(
        participant_id, topic, payload_pool, participant, topic_entity)
{
    EPROSIMA_LOG_INFO(DDSPIPE_RPC_READER, "Creating RPC Reader for topic " << topic_);
}

void DdsSimpleReader::fill_received_data_(
        const fastdds::dds::SampleInfo& info,
        core::types::RtpsPayloadData& data_to_fill) const noexcept
{
    core::types::RpcPayloadData& rpc_data = dynamic_cast<core::types::RpcPayloadData&>(data_to_fill);

    // Store the new data that has arrived in the Track data
    // Get the writer guid
    rpc_data.source_guid = detail::guid_from_instance_handle(info.publication_handle);
    // Get source timestamp
    rpc_data.source_timestamp = info.source_timestamp;
    // Get Participant receiver
    rpc_data.participant_receiver = participant_id_;

    // Set Instance Handle to rpc_data
    if (topic_.topic_qos.keyed)
    {
        rpc_data.instanceHandle = info.instance_handle;

        // Set change kind
        switch (info.instance_state)
        {
            case fastdds::dds::InstanceStateKind::ALIVE_INSTANCE_STATE:
                rpc_data.kind = core::types::ChangeKind::ALIVE;
                break;

            case fastdds::dds::InstanceStateKind::NOT_ALIVE_DISPOSED_INSTANCE_STATE:
                rpc_data.kind = core::types::ChangeKind::NOT_ALIVE_DISPOSED;
                break;

            default:
                rpc_data.kind = core::types::ChangeKind::NOT_ALIVE_UNREGISTERED;
                break;
        }
    }
    else
    {
        rpc_data.kind = core::types::ChangeKind::ALIVE;
    }

    fastdds::rtps::WriteParams write_params{};
    write_params.related_sample_identity() = info.related_sample_identity;
    write_params.sample_identity() = info.sample_identity;
    write_params.source_timestamp() = info.source_timestamp;
    rpc_data.write_params.set_value(write_params);

    rpc_data.origin_sequence_number = info.sample_identity.sequence_number();
}

core::types::RtpsPayloadData* DdsSimpleReader::create_data_() const noexcept
{
    return new core::types::RpcPayloadData();
}

} /* namespace rpc */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
