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
 * @file SelfDataFilter.hpp
 */

#pragma once

#include <fastdds/dds/topic/IContentFilter.hpp>

#include <fastdds/rtps/common/Guid.hpp>
#include <fastdds/rtps/common/SerializedPayload.hpp>

#include <ddspipe_participants/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

/**
 * @brief Filter to prevent sending messages from this writer to readers in the same participant.
 *
 * This filter ensures that messages originating from this writer are not delivered
 * to readers that belong to the same participant, avoiding self-data delivery.
 */
class SelfDataFilter : public fastdds::dds::IContentFilter
{
public:

    /**
     * @brief Evaluates whether a message should be sent to a given reader.
     *
     * This method checks whether the reader belongs to the same participant as the writer.
     *
     * @param payload The serialized payload of the message.
     * @param sample_info Information about the sample for content filtering.
     * @param reader_guid The GUID of the reader to which the message may be sent.
     * @return true if the reader does not belong to the same participant.
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
