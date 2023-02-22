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

/**
 * @file InternalReader.cpp
 */

#include <cpp_utils/exception/UnsupportedException.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_participants/reader/auxiliar/InternalReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;

InternalReader::InternalReader(
        const ParticipantId& participant_id)
    : BaseReader(participant_id)
{
    // Do nothing
}

void InternalReader::enable_nts_() noexcept
{
    // Process data added to queue while being disabled
    std::lock_guard<DataReceivedType> lock(data_to_send_);
    if (!data_to_send_.empty())
    {
        on_data_available_();
    }
}

void InternalReader::simulate_data_reception(
        std::unique_ptr<IRoutingData>&& data) noexcept
{
    std::lock_guard<DataReceivedType> lock(data_to_send_);

    // Even if disabled, the data will be stored
    data_to_send_.push(std::move(data));

    // Call on data available callback
    // NOTE: Track should be already created (lambda set), otherwise data will not be processed until reader is enabled
    on_data_available_();
}

utils::ReturnCode InternalReader::take_nts_(
        std::unique_ptr<IRoutingData>& data) noexcept
{
    std::lock_guard<DataReceivedType> lock(data_to_send_);

    // Enable check is done in BaseReader

    // There is no data pending sent
    if (data_to_send_.empty())
    {
        return utils::ReturnCode::RETCODE_NO_DATA;
    }

    // Move first data in queue to input
    data = std::move(data_to_send_.front());
    data_to_send_.pop();

    return utils::ReturnCode::RETCODE_OK;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
