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

#include <ddspipe_core/monitoring/clients/StatusMonitorClient.hpp>
#include <ddspipe_core/monitoring/clients/TopicsMonitorClient.hpp>
#include <ddspipe_core/monitoring/consumers/DdsMonitorConsumer.hpp>
#include <ddspipe_core/types/monitoring/status/MonitoringStatusPubSubTypes.h>
#include <ddspipe_core/types/monitoring/topics/MonitoringTopicsPubSubTypes.h>


namespace eprosima {
namespace ddspipe {
namespace core {


DdsMonitorConsumer::DdsMonitorConsumer(
        const MonitorConfiguration& configuration)
    : status_type_(new MonitoringStatusPubSubType())
    , topics_type_(new MonitoringTopicsPubSubType())
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
    status_type_.register_type(participant_);
    topics_type_.register_type(participant_);

    // Create the publisher
    publisher_ = participant_->create_publisher(fastdds::dds::PUBLISHER_QOS_DEFAULT, nullptr);

    if (publisher_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Publisher for Participant " <<
                      pqos.name() << ".");
    }

    // STATUS

    // Create the topic
    status_topic_ = create_topic_(configuration.status.topic_name, "MonitoringStatus");

    // Create the writer
    status_writer_ = create_writer_(status_topic_);

    // TOPICS

    // Create the topic
    topics_topic_ = create_topic_(configuration.topics.topic_name, "MonitoringTopics");

    // Create the writer
    topics_writer_ = create_writer_(topics_topic_);
}

DdsMonitorConsumer::~DdsMonitorConsumer()
{
    fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(participant_);
}

void DdsMonitorConsumer::consume(IMonitorData* data) const
{
    MonitorStatus* status = dynamic_cast<MonitorStatus*>(data);

    if (status != nullptr)
    {
        status_writer_->write(&status->data);
        return;
    }

    MonitorTopics* topics = dynamic_cast<MonitorTopics*>(data);

    if (topics != nullptr)
    {
        topics_writer_->write(&topics->data);
        return;
    }
}

fastdds::dds::Topic* DdsMonitorConsumer::create_topic_(
        const std::string& topic_name,
        const std::string& type_name)
{
    fastdds::dds::TopicQos tqos = fastdds::dds::TOPIC_QOS_DEFAULT;
    tqos.reliability().kind = fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    tqos.durability().kind = fastdds::dds::VOLATILE_DURABILITY_QOS;

    fastdds::dds::Topic* topic = participant_->create_topic(topic_name, type_name, tqos);

    if (topic == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Topic " << topic_name <<
                  " for Participant " << participant_->guid() << ".");
    }

    return topic;
}

fastdds::dds::DataWriter* DdsMonitorConsumer::create_writer_(
        fastdds::dds::Topic* topic)
{
    fastdds::dds::DataWriterQos wqos = fastdds::dds::DATAWRITER_QOS_DEFAULT;

    wqos.data_sharing().automatic();
    wqos.publish_mode().kind = fastdds::dds::SYNCHRONOUS_PUBLISH_MODE;
    wqos.reliability().kind = fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    wqos.durability().kind = fastdds::dds::VOLATILE_DURABILITY_QOS;

    fastdds::dds::DataWriter* writer = publisher_->create_datawriter(topic, wqos);

    if (writer == nullptr)
    {
        throw utils::InitializationException(
                utils::Formatter() << "Error creating DataWriter for Participant " <<
                      participant_->guid() << " in topic " << topic << ".");
    }

    return writer;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
