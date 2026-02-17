// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <functional>

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/writer/auxiliar/BaseWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * Writer implementation that allow to register a callback to be executed with each data received.
 */
class InternalWriter : public BaseWriter
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    InternalWriter(
            const core::types::ParticipantId& participant_id,
            const std::function<utils::ReturnCode(core::IRoutingData&)>& callback);

protected:

    virtual utils::ReturnCode write_nts_(
            core::IRoutingData& data) noexcept override;

    //! Update method to change the partitions in the content_topicfilter
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void update_partitions(
            const std::set<std::string>& partitions_set) override;

    //! Update method to change the topic_partitions
    virtual void update_topic_partitions(
            const std::map<std::string, std::string>& partition_name) override;

    const std::function<utils::ReturnCode(core::IRoutingData&)> callback_;
};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
