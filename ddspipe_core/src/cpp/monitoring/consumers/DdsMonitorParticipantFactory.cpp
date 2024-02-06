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


#include <cpp_utils/exception/InitializationException.hpp>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <ddspipe_core/monitoring/consumers/DdsMonitorParticipantFactory.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {


std::map<types::DomainIdType, fastdds::dds::DomainParticipant*> DdsMonitorParticipantFactory::participants_;


fastdds::dds::DomainParticipant* DdsMonitorParticipantFactory::get_participant(
        const types::DomainIdType& domain)
{
    fastdds::dds::DomainParticipant* participant;

    if (participants_.find(domain) != participants_.end())
    {
        // The participant already exists. Use it.
        participant = participants_[domain];
    }
    else
    {
        // The participant does not exist. Create it.
        fastdds::dds::DomainParticipantQos pqos;
        pqos.name("DdsPipeMonitorParticipant_" + std::to_string(domain));

        participant = fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(domain, pqos);

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

DdsMonitorParticipantFactory::~DdsMonitorParticipantFactory()
{
    // Delete all the participants
    for (auto& participant : participants_)
    {
        fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(participant.second);
    }

    participants_.clear();
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima