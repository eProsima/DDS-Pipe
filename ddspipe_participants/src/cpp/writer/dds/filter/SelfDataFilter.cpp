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

#include <ddspipe_participants/writer/dds/filter/SelfDataFilter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

bool SelfDataFilter::evaluate(
            const fastdds::rtps::SerializedPayload_t&,
            const fastdds::dds::IContentFilter::FilterSampleInfo& sample_info,
            const fastdds::rtps::GUID_t& reader_guid) const
{
    // It is relevant only if the reader does not belong to same participant as writer
    return sample_info.sample_identity.writer_guid().guidPrefix != reader_guid.guidPrefix;
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
