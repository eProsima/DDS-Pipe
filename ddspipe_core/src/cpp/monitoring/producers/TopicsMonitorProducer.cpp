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


#include <ddspipe_core/configuration/MonitorTopicsConfiguration.hpp>
#include <ddspipe_core/monitoring/consumers/DdsMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/consumers/StdoutMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

TopicsMonitorProducer* TopicsMonitorProducer::get_instance()
{
    static TopicsMonitorProducer instance;
    return &instance;
}

void TopicsMonitorProducer::init(const MonitorTopicsConfiguration* configuration)
{
    // Store the period so it can be used by the Monitor
    period = configuration->period;

    // Register the type
    fastdds::dds::TypeSupport type(new MonitoringTopicsPubSubType());

    // Create the consumers
    consumers_.push_back(new DdsMonitorConsumer<MonitoringTopics>(configuration, type));
    consumers_.push_back(new StdoutMonitorConsumer<MonitoringTopics>(configuration));
}

void TopicsMonitorProducer::consume() const
{
    MonitoringTopics data = save_data_();

    for (auto consumer : consumers_)
    {
        consumer->consume(&data);
    }
}

void TopicsMonitorProducer::msg_received(
        const types::DdsTopic& topic,
        const types::ParticipantId& participant_id)
{
    // Take the lock to prevent:
    //      1. Changing the data while it's being saved.
    //      2. Simultaneous calls to msg_received.
    std::lock_guard<std::mutex> lock(mutex_);

    if (!data_.count(topic) || !data_[topic].count(participant_id))
    {
        // First message received for topic & participant

        // Save the time
        data_[topic][participant_id].start_time = utils::now();

        // Save the participant_id
        data_[topic][participant_id].data.participant_id(participant_id);
    }

    // Increase the count of the received messages
    data_[topic][participant_id].data.msgs_received(data_[topic][participant_id].data.msgs_received() + 1);
}

TopicsMonitorProducer::~TopicsMonitorProducer()
{
    consumers_.clear();
}

MonitoringTopics TopicsMonitorProducer::save_data_() const
{
    // Take the lock to prevent saving the data while it's changing
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<DdsTopic> topics_data;

    // Iterate through the different topics
    for (const auto& topic : data_)
    {
        const auto& dds_topic = topic.first;
        const auto& participants = topic.second;

        DdsTopic topic_data;

        // Save the topic name and type
        topic_data.name(dds_topic.m_topic_name);
        topic_data.data_type_name(dds_topic.type_name);

        std::vector<DdsTopicData> topic_participants;

        for (const auto participant : participants)
        {
            const auto& participant_id = participant.first;
            const auto& info = participant.second;

            DdsTopicData participant_data = info.data;

            // Calculate the time between the first sample and the last
            const std::chrono::duration<double> time_elapsed = utils::now() - info.start_time;

            // Calculate the message reception frequency
            participant_data.frequency((double) participant_data.msgs_received() / time_elapsed.count());

            // Save the participant's data for the topic
            topic_participants.push_back(participant_data);
        }

        // Save the participants' data for the topic
        topic_data.data(topic_participants);

        // Save the topic data
        topics_data.push_back(topic_data);
    }

    MonitoringTopics data;

    // Save the topics' data
    data.topics(topics_data);

    return data;
}

std::ostream& operator<<(std::ostream& os, const DdsTopicData& data) {
    os << "Participant ID: " << data.participant_id() << ", Messages Received: " << data.msgs_received() << ", Frequency: " << data.frequency();
    return os;
}

std::ostream& operator<<(std::ostream& os, const DdsTopic& topic) {
    os << "Topic Name: " << topic.name() << ", Type: " << topic.data_type_name() << ", Data: [";

    for (const auto& data : topic.data()) {
        os << data << "; ";
    }

    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const MonitoringTopics& data) {
    os << "Monitoring Topics: [";

    for (const auto& topic : data.topics()) {
        os << topic << "; ";
    }

    os << "]";
    return os;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
