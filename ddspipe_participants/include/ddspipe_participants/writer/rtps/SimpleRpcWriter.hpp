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

#pragma once

#include <fastdds/rtps/attributes/HistoryAttributes.hpp>
#include <fastdds/dds/publisher/qos/WriterQos.hpp>
#include <fastdds/rtps/history/WriterHistory.hpp>
#include <fastdds/rtps/attributes/WriterAttributes.hpp>
#include <fastdds/rtps/writer/RTPSWriter.hpp>

#include <ddspipe_core/types/participant/ParticipantId.hpp>

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/writer/rtps/CommonWriter.hpp>
#include <ddspipe_participants/efficiency/cache_change/CacheChangePool.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace rtps {

class SimpleRpcWriter : public CommonWriter
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    SimpleRpcWriter(
            const core::types::ParticipantId& participant_id,
            const core::types::DdsTopic& topic,
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            fastdds::rtps::RTPSParticipant* rtps_participant,
            const bool repeater = false);

    //! Override Parent method to fill fields only required for RPC.
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual utils::ReturnCode fill_to_send_data_(
            fastdds::rtps::CacheChange_t* to_send_change_to_fill,
            eprosima::fastdds::rtps::WriteParams& to_send_params,
            const core::types::RtpsPayloadData& data) const noexcept;

    //! Override Parent method to fill fields after message is sent only required for RPC.
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void fill_sent_data_(
            const eprosima::fastdds::rtps::WriteParams& sent_params,
            core::types::RtpsPayloadData& data_to_fill) const noexcept;
};

} /* namespace rtps */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
