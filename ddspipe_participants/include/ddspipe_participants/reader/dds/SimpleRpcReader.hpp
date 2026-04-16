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

#pragma once

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/reader/dds/SimpleReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

class SimpleRpcReader : public SimpleReader
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    SimpleRpcReader(
            const core::types::ParticipantId& participant_id,
            const core::types::DdsTopic& topic,
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            fastdds::dds::DomainParticipant* participant,
            fastdds::dds::Topic* topic_entity);

    //! Override Parent method to create an RPC data type.
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual core::types::RtpsPayloadData* create_data_() const noexcept override;

    //! Override Parent method to fill fields exclusive from RPC.
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void fill_received_data_(
            const fastdds::dds::SampleInfo& info,
            core::types::RtpsPayloadData& data_to_fill) const noexcept override;
};

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
