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

#include <ddspipe_core/configuration/MonitorProducerConfiguration.hpp>
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

void TopicsMonitorProducer::register_consumer(
        std::unique_ptr<IMonitorConsumer<MonitoringTopics>> consumer)
{
    if (!enabled_)
    {
        // Don't register the consumer if the producer is not enabled
        return;
    }

    consumers_.push_back(std::move(consumer));
}

void TopicsMonitorProducer::consume()
{
    if (!enabled_)
    {
        // Don't consume if the producer is not enabled
        return;
    }

    const auto data = save_data_();

    for (auto& consumer : consumers_)
    {
        consumer->consume(&data);
    }

    reset_data_();
}

void TopicsMonitorProducer::msg_received(
        const types::DdsTopic& topic,
        const types::ParticipantId& participant_id)
{
    if (!enabled_)
    {
        // Don't save the data if the producer is not enabled
        return;
    }

    // Take the lock to prevent:
    //      1. Changing the data while it's being saved.
    //      2. Simultaneous calls to msg_received.
    std::lock_guard<std::mutex> lock(mutex_);

    // Register the topic
    topic_data_[topic].name(topic.m_topic_name);
    topic_data_[topic].type_name(topic.type_name);

    // Register the participant
    participant_data_[topic][participant_id].participant_id(participant_id);

    // Increase the count of the received messages
    participant_data_[topic][participant_id].msgs_received(participant_data_[topic][participant_id].msgs_received() +
            1);
}

void TopicsMonitorProducer::msg_lost(
        const types::DdsTopic& topic,
        const types::ParticipantId& participant_id)
{
    if (!enabled_)
    {
        // Don't save the data if the producer is not enabled
        return;
    }

    // Take the lock to prevent:
    //      1. Changing the data while it's being saved.
    //      2. Simultaneous calls to msg_lost.
    std::lock_guard<std::mutex> lock(mutex_);

    // Register the topic
    topic_data_[topic].name(topic.m_topic_name);
    topic_data_[topic].type_name(topic.type_name);

    // Register the participant id
    participant_data_[topic][participant_id].participant_id(participant_id);

    // Increase the count of the lost messages
    participant_data_[topic][participant_id].msgs_lost(participant_data_[topic][participant_id].msgs_lost() + 1);
}

void TopicsMonitorProducer::type_discovered(
        const std::string& type_name)
{
    if (!enabled_)
    {
        // Don't save the data if the producer is not enabled
        return;
    }

    // Take the lock to prevent:
    //      1. Changing the data while it's being saved.
    //      2. Simultaneous calls to msg_lost.
    std::lock_guard<std::mutex> lock(mutex_);

    types_discovered_[type_name] = true;
}

void TopicsMonitorProducer::type_mismatch(
        const types::DdsTopic& topic)
{
    if (!enabled_)
    {
        // Don't save the data if the producer is not enabled
        return;
    }

    // Take the lock to prevent:
    //      1. Changing the data while it's being saved.
    //      2. Simultaneous calls to msg_lost.
    std::lock_guard<std::mutex> lock(mutex_);

    // Register the topic
    topic_data_[topic].name(topic.m_topic_name);
    topic_data_[topic].type_name(topic.type_name);

    // Set the type mismatch flag
    topic_data_[topic].type_mismatch(true);
}

void TopicsMonitorProducer::qos_mismatch(
        const types::DdsTopic& topic)
{
    if (!enabled_)
    {
        // Don't save the data if the producer is not enabled
        return;
    }

    // Take the lock to prevent:
    //      1. Changing the data while it's being saved.
    //      2. Simultaneous calls to msg_lost.
    std::lock_guard<std::mutex> lock(mutex_);

    // Register the topic
    topic_data_[topic].name(topic.m_topic_name);
    topic_data_[topic].type_name(topic.type_name);

    // Set the QoS mismatch flag
    topic_data_[topic].qos_mismatch(true);
}

MonitoringTopics TopicsMonitorProducer::save_data_()
{
    // Take the lock to prevent saving the data while it's changing
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<DdsTopic> topics_data;

    // Iterate through the different topics
    for (auto& topic : topic_data_)
    {
        // Set the type discovered flag
        topic.second.type_discovered(types_discovered_[topic.second.type_name()]);

        std::vector<DdsTopicData> topic_participants;

        for (auto& participant : participant_data_[topic.first])
        {
            // Calculate the message reception frequency
            const double period_in_secs = (double) period / 1000;
            participant.second.frequency((double) participant.second.msgs_received() / period_in_secs);

            // Save the participant's data for the topic
            topic_participants.push_back(participant.second);
        }

        // Save the participants' data for the topic
        topic.second.data(topic_participants);

        // Save the topic data
        topics_data.push_back(topic.second);
    }

    MonitoringTopics data;

    // Save the topics' data
    data.topics(topics_data);

    return data;
}

void TopicsMonitorProducer::reset_data_()
{
    // Take the lock to prevent reseting the data while it's being saved
    std::lock_guard<std::mutex> lock(mutex_);

    // Reset the data
    for (auto& topic : participant_data_)
    {
        for (auto& participant : topic.second)
        {
            participant.second.msgs_received(0);
            participant.second.msgs_lost(0);
        }
    }
}

std::ostream& operator <<(
        std::ostream& os,
        const DdsTopicData& data)
{
    os << "Participant ID: " << data.participant_id();
    os << ", Messages Received: " << data.msgs_received();
    os << ", Frequency: " << data.frequency();
    os << ", Messages Lost: " << data.msgs_lost();
    return os;
}

std::ostream& operator <<(
        std::ostream& os,
        const DdsTopic& topic)
{
    auto bool_to_string = [](bool value) -> std::string
            {
                return value ? "true" : "false";
            };

    os << "Topic Name: " << topic.name();
    os << ", Type Name: " << topic.type_name();
    os << ", Type Discovered: " << bool_to_string(topic.type_discovered());
    os << ", Type Mismatch: " << bool_to_string(topic.type_mismatch());
    os << ", QoS Mismatch: " << bool_to_string(topic.qos_mismatch());

    os << ", Data: [";

    for (const auto& data : topic.data())
    {
        os << data << "; ";
    }

    os << "]";

    return os;
}

std::ostream& operator <<(
        std::ostream& os,
        const MonitoringTopics& data)
{
    os << "Monitoring Topics: [";

    for (const auto& topic : data.topics())
    {
        os << topic << "; ";
    }

    os << "]";
    return os;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
