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

/**
 * @file RepeaterDataFilter.hpp
 */

#pragma once

#include <fastdds/rtps/common/Guid.hpp>
#include <fastdds/rtps/common/WriteParams.hpp>

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/writer/dds/filter/SelfDataFilter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

/**
 * @brief Struct containing all relevant information for the \c RepeaterDataFilter.
 *
 * This structure extends the \c UserWriteData to include the GUID prefix of the last writer
 * that sent a change. It is used to track the origin of a message in repeater scenarios.
 */
struct RepeaterWriteData : public fastdds::rtps::WriteParams::UserWriteData
{
    /// GUID of the writer that sent this change.
    fastdds::rtps::GuidPrefix_t last_writer_guid_prefix;
};

/**
 * @brief Data filter to prevent sending messages back to their source participant.
 *
 * This filter is used by "repeater" participants to propagate information only to external participants,
 * i.e., participants not belonging to the same DDS-Router instance. It ensures that messages are not
 * sent to readers that belong to the participant from which the information was originally received.
 */
class RepeaterDataFilter : public SelfDataFilter
{
public:

    /**
     * @brief Evaluates whether a message should be sent to a given reader.
     *
     * This method checks whether the reader belongs to the same participant from which the message originated.
     * I also checks whether the reader belongs to the same participant as the writer.
     *
     * @param payload The serialized payload of the message.
     * @param sample_info Information about the sample for content filtering.
     * @param reader_guid The GUID of the reader to which the message may be sent.
     * @return true if the reader does not belong to the participant that sent the message
     * and also does not belong to the same participant as the writer.
     * @return false otherwise.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    bool evaluate(
            const fastdds::rtps::SerializedPayload_t& payload,
            const fastdds::dds::IContentFilter::FilterSampleInfo& sample_info,
            const fastdds::rtps::GUID_t& reader_guid) const override;
};

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
