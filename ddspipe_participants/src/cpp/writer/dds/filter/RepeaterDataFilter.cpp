// Copyright 2025 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <ddspipe_participants/writer/dds/filter/RepeaterDataFilter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

bool RepeaterDataFilter::evaluate(
        const fastdds::rtps::SerializedPayload_t& payload,
        const fastdds::dds::IContentFilter::FilterSampleInfo& sample_info,
        const fastdds::rtps::GUID_t& reader_guid) const
{
    if (!SelfDataFilter::evaluate(payload, sample_info, reader_guid))
    {
        logDebug(
            REPEATER_DATA_FILTER,
            "Ignoring message by SelfDataFilter evaluate result.");

        // Discard sample if it comes from the same participant
        return false;
    }

    auto write_data = std::static_pointer_cast<RepeaterWriteData>(sample_info.user_write_data);

    bool is_relevant = write_data->last_writer_guid_prefix != reader_guid.guidPrefix;

    logDebug(
        REPEATER_DATA_FILTER,
        "Evaluating whether sample with origin writer GUID prefix " << write_data->last_writer_guid_prefix  <<
            " is relevant for reader GUID " << reader_guid << "? " << (is_relevant ? "TRUE" : "FALSE"));

    return is_relevant;
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
