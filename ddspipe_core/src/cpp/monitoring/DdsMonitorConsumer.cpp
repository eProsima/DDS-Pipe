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

#include <ddspipe_core/monitoring/DdsMonitorConsumer.hpp>
#include <ddspipe_core/types/monitoring/topics/MonitoringDataPubSubTypes.h>


namespace eprosima {
namespace ddspipe {
namespace core {


DdsMonitorConsumer::DdsMonitorConsumer(std::string topic_name)
    : type_(new MonitoringDataPubSubType())
{
    fastdds::dds::DomainParticipantQos pqos;
    pqos.name("DdsPipeMonitorParticipant");

    // Create the participant
    participant_ = fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(85, pqos);

    if (participant_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Participant " <<
                      pqos.name() << ".");
    }

    // Register the type
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
    topic_ = participant_->create_topic(topic_name, "MonitoringData", fastdds::dds::TOPIC_QOS_DEFAULT);

    if (topic_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Topic for Participant " <<
                      pqos.name() << ".");
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
                      pqos.name() << " in topic " << topic_ << ".");
    }
}

DdsMonitorConsumer::~DdsMonitorConsumer()
{
    fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(participant_);
}

void DdsMonitorConsumer::consume(const MonitoringData& data) const
{
    // The write method can modify the data. Make a copy.
    auto data_copy = data;
    writer_->write(&data_copy);
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
