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

#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/types/dds/DomainId.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * The \c DdsMonitorParticipantRegistry is a helper class that manages the \c DomainParticipant instances.
 * \c DdsMonitorConsumers use the registry to avoid creating multiple \c DomainParticipants on the same domain.
 */
class DdsMonitorParticipantRegistry
{
public:

    /**
     * @brief Destroy the \c DomainParticipants.
     */
    DDSPIPE_CORE_DllAPI
    ~DdsMonitorParticipantRegistry();

    /**
     * @brief Get the \c DomainParticipant for the given domain.
     *
     * If the \c DomainParticipant doesn't exist, it is created with the default QoS.
     * If the \c DomainParticipant already exists, it is returned.
     *
     * @todo Let the user configure the QoS.
     *
     * @param domain The domain to get the \c DomainParticipant for.
     * @throws \c InitializationException if the \c DomainParticipant cannot be created.
     *
     * @return The \c DomainParticipant for the given domain.
     */
    DDSPIPE_CORE_DllAPI
    fastdds::dds::DomainParticipant* get_participant(
            const types::DomainIdType& domain);

protected:

    // Map of DomainId to DomainParticipant.
    std::map<types::DomainIdType, fastdds::dds::DomainParticipant*> participants_;

    // Mutex to protect the access to the participants_ map.
    std::mutex mutex_;
};


} // namespace core
} // namespace ddspipe
} // namespace eprosima
