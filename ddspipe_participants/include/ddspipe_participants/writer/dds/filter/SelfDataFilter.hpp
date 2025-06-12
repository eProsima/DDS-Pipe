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
 * This filter allows to not send messages from this Writer to the Readers in the same Participant.
 */
class SelfDataFilter : public fastdds::dds::IContentFilter
{
public:

    // TODO
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
