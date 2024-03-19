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


#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/monitoring/consumers/DdsMonitorParticipantRegistry.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {


DdsMonitorParticipantRegistry::~DdsMonitorParticipantRegistry()
{
    // Delete all the participants
    for (auto& participant : participants_)
    {
        if (participant.second == nullptr)
        {
            continue;
        }

        logInfo(DDSPIPE_MONITOR,
                "MONITOR | Deleting Participant " << participant.second << " on domain " << participant.first << ".");

        participant.second->delete_contained_entities();
        fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(participant.second);
    }

    participants_.clear();
}

fastdds::dds::DomainParticipant* DdsMonitorParticipantRegistry::get_participant(
        const types::DomainIdType& domain)
{
    std::lock_guard<std::mutex> lock(mutex_);

    fastdds::dds::DomainParticipant* participant;

    if (participants_.find(domain) != participants_.end())
    {
        // The participant already exists. Use it.
        participant = participants_[domain];

        logInfo(DDSPIPE_MONITOR, "MONITOR | Reusing Participant " << participant << " on domain " << domain << ".")
    }
    else
    {
        // The participant does not exist. Create it.
        fastdds::dds::DomainParticipantQos pqos;
        pqos.name("DdsMonitorParticipant_" + std::to_string(domain));

        participant = fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(domain, pqos);

        logInfo(DDSPIPE_MONITOR, "MONITOR | Created Participant " << participant << " on domain " << domain << ".");

        if (participant == nullptr)
        {
            throw utils::InitializationException(
                      utils::Formatter() << "Error creating Participant " <<
                          pqos.name() << ".");
        }

        // Store the participant
        participants_[domain] = participant;
    }

    return participant;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
