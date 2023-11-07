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

#include <cpp_utils/Log.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_participants/writer/auxiliar/BaseWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

BaseWriter::BaseWriter(
        const core::types::ParticipantId& participant_id,
        const float max_tx_rate /* = 0 */)
    : participant_id_(participant_id)
    , max_tx_rate_(max_tx_rate)
    , enabled_(false)
{
    logDebug(DDSPIPE_BASEWRITER, "Creating Writer " << *this << ".");

    // Calculate min_intersample_period_ from topic's max_tx_rate only once to lighten hot path
    assert(max_tx_rate_ >= 0);

    if (max_tx_rate_ > 0)
    {
        min_intersample_period_ = std::chrono::nanoseconds((unsigned int)(1e9 / max_tx_rate_));
    }
}

void BaseWriter::enable() noexcept
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    // If it is enabled, do nothing
    if (!enabled_.load())
    {
        enabled_.store(true);

        // Call specific enable
        enable_();
    }
}

void BaseWriter::disable() noexcept
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    // If it is not enabled, do nothing
    if (enabled_.load())
    {
        enabled_.store(false);

        // Call specific disable
        disable_();
    }
}

utils::ReturnCode BaseWriter::write(
        core::IRoutingData& data) noexcept
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (enabled_.load())
    {
        if (!should_send_sample_())
        {
            return utils::ReturnCode::RETCODE_OK;
        }
        else
        {
            return write_nts_(data);
        }

    }
    else
    {
        logDevError(DDSPIPE_BASEWRITER,
                "Attempt to write data from disabled Writer in topic in Participant " << participant_id_);
        return utils::ReturnCode::RETCODE_NOT_ENABLED;
    }
}

void BaseWriter::enable_() noexcept
{
    // It does nothing. Override this method so it has functionality.
}

void BaseWriter::disable_() noexcept
{
    // It does nothing. Override this method so it has functionality.
}

bool BaseWriter::should_send_sample_() noexcept
{
    // Get transmission timestamp
    auto now = utils::now();

    // Max Transmission Rate
    if (max_tx_rate_ > 0)
    {
        auto threshold = last_sent_ts_ + min_intersample_period_;
        if (now < threshold)
        {
            return false;
        }
    }

    // All filters passed -> Update last sent timestamp with this sample's transmission timestamp
    last_sent_ts_ = now;

    return true;
}

std::ostream& operator <<(
        std::ostream& os,
        const BaseWriter& writer)
{
    os << "Writer{" << writer.participant_id_ << "}";
    return os;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
