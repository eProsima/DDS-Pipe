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
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>

#include <ddspipe_core/types/monitoring/topics/MonitoringTopicsPubSubTypes.h>


namespace eprosima {
namespace ddspipe {
namespace core {


template <typename T>
DdsMonitorConsumer<T>::DdsMonitorConsumer(
        const MonitorConfiguration& configuration,
        const std::string& topic_name,
        const std::string& type_name,
        fastdds::dds::TypeSupport& type)
{
    fastdds::dds::DomainParticipantQos pqos;
    pqos.name("DdsPipeMonitorParticipant");

    // Create the participant
    participant_ = fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(configuration.domain, pqos);

    if (participant_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Participant " <<
                      pqos.name() << ".");
    }

    // Register the types
    type_ = type;
    type_.register_type(participant_);

    // Create the publisher
    publisher_ = participant_->create_publisher(fastdds::dds::PUBLISHER_QOS_DEFAULT, nullptr);

    if (publisher_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Publisher for Participant " <<
                      pqos.name() << ".");
    }

    // Create the topic
    fastdds::dds::TopicQos tqos = fastdds::dds::TOPIC_QOS_DEFAULT;
    tqos.reliability().kind = fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    tqos.durability().kind = fastdds::dds::VOLATILE_DURABILITY_QOS;

    topic_ = participant_->create_topic(topic_name, type_name, tqos);

    if (topic_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Topic " << topic_name <<
                  " for Participant " << participant_->guid() << ".");
    }

    // Create the writer
    fastdds::dds::DataWriterQos wqos = fastdds::dds::DATAWRITER_QOS_DEFAULT;

    wqos.data_sharing().automatic();
    wqos.publish_mode().kind = fastdds::dds::SYNCHRONOUS_PUBLISH_MODE;
    wqos.reliability().kind = fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    wqos.durability().kind = fastdds::dds::VOLATILE_DURABILITY_QOS;

    writer_ = publisher_->create_datawriter(topic_, wqos);

    if (writer_ == nullptr)
    {
        throw utils::InitializationException(
                utils::Formatter() << "Error creating DataWriter for Participant " <<
                      participant_->guid() << " in topic " << topic_ << ".");
    }
}

template <typename T>
DdsMonitorConsumer<T>::~DdsMonitorConsumer()
{
    fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(participant_);
}


template <typename T>
void DdsMonitorConsumer<T>::consume(const T* data) const
{
    T data_copy = *data;
    writer_->write(&data_copy);
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
