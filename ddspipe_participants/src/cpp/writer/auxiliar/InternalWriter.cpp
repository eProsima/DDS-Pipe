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

#include <ddspipe_participants/writer/auxiliar/InternalWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

InternalWriter::InternalWriter(
        const core::types::ParticipantId& participant_id,
        const std::function<utils::ReturnCode(core::IRoutingData&)>& callback)
    : BaseWriter(participant_id)
    , callback_(callback)
{
    // Do nothing
}

utils::ReturnCode InternalWriter::write_nts_(
        core::IRoutingData& data) noexcept
{
    return callback_(data);
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
