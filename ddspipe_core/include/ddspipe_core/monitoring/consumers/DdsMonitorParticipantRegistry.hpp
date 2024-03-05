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
//

#pragma once

#include <map>
#include <mutex>

#include <fastdds/dds/domain/DomainParticipant.hpp>

#include <ddspipe_core/types/dds/DomainId.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
class DdsMonitorParticipantRegistry
{
public:

    // TODO
    ~DdsMonitorParticipantRegistry();

    // TODO
    fastdds::dds::DomainParticipant* get_participant(
            const types::DomainIdType& domain);

protected:

    // TODO
    std::map<types::DomainIdType, fastdds::dds::DomainParticipant*> participants_;

    // TODO
    std::mutex mutex_;
};


} // namespace core
} // namespace ddspipe
} // namespace eprosima
