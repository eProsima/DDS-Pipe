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

#include <cpp_utils/Log.hpp>

#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

std::mutex TopicsMonitorProducer::mutex_;
std::unique_ptr<TopicsMonitorProducer> TopicsMonitorProducer::instance_ = nullptr;

void TopicsMonitorProducer::init_instance(
        std::unique_ptr<TopicsMonitorProducer> instance)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (instance_ != nullptr)
    {
        logWarning(DDSPIPE_MONITOR, "MONITOR | TopicsMonitorProducer instance is already initialized.");
        return;
    }

    instance_ = std::move(instance);
}

TopicsMonitorProducer* TopicsMonitorProducer::get_instance()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (instance_ == nullptr)
    {
        instance_ = std::make_unique<TopicsMonitorProducer>();
    }

    return instance_.get();
}

void TopicsMonitorProducer::enable()
{
    std::lock_guard<std::mutex> lock(mutex_);

    logInfo(DDSPIPE_MONITOR, "MONITOR | Enabling TopicsMonitorProducer.");

    enabled_ = true;
}

void TopicsMonitorProducer::disable()
{
    std::lock_guard<std::mutex> lock(mutex_);

    logInfo(DDSPIPE_MONITOR, "MONITOR | Disabling TopicsMonitorProducer.");

    enabled_ = false;
}

void TopicsMonitorProducer::register_consumer(
        std::unique_ptr<IMonitorConsumer<MonitoringTopics>> consumer)
{
    if (!enabled_)
    {
        logWarning(DDSPIPE_MONITOR,
                "MONITOR | Not registering consumer " << consumer->get_name() << " on TopicsMonitorProducer"
                "since the TopicsMonitorProducer is disabled.");

        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    logInfo(DDSPIPE_MONITOR, "MONITOR | Registering consumer " << consumer->get_name() << " on TopicsMonitorProducer.");

    consumers_.push_back(std::move(consumer));
}

void TopicsMonitorProducer::clear_consumers()
{
    std::lock_guard<std::mutex> lock(mutex_);

    logInfo(DDSPIPE_MONITOR, "MONITOR | Removing all consumers from TopicsMonitorProducer.");

    consumers_.clear();
}

void TopicsMonitorProducer::produce_and_consume()
{
    if (!enabled_)
    {
        // Don't produce if the producer is not enabled
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    produce_nts_();
    consume_nts_();
}

void TopicsMonitorProducer::produce()
{
    if (!enabled_)
    {
        // Don't produce if the producer is not enabled
        return;
    }

    // Take the lock to prevent saving the data while it's changing
    std::lock_guard<std::mutex> lock(mutex_);

    produce_nts_();
}

void TopicsMonitorProducer::consume()
{
    if (!enabled_)
    {
        // Don't consume if the producer is not enabled
        return;
    }

    // Take the lock to prevent consuming the data while it's changing
    std::lock_guard<std::mutex> lock(mutex_);

    consume_nts_();
}

void TopicsMonitorProducer::clear_data()
{
    // Take the lock to prevent clearing the data while it's being saved
    std::lock_guard<std::mutex> lock(mutex_);

    logInfo(DDSPIPE_MONITOR, "MONITOR | Clearing the data.");

    topic_data_.clear();
    participant_data_.clear();
    types_discovered_.clear();

    data_.topics().clear();
}

void TopicsMonitorProducer::msgs_received(
        const types::DdsTopic& topic,
        const types::ParticipantId& participant_id,
        const int number_of_messages /*= 1*/)
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

    logInfo(DDSPIPE_MONITOR, "MONITOR | Received " << number_of_messages << " messages from Participant " <<
            participant_id << " on Topic " << topic << ".");

    // Register the topic
    topic_data_[topic].name(topic.m_topic_name);
    topic_data_[topic].type_name(topic.type_name);

    // Register the participant
    participant_data_[topic][participant_id].participant_id(participant_id);

    // Increase the count of the received messages
    participant_data_[topic][participant_id].msgs_received(participant_data_[topic][participant_id].msgs_received() +
            number_of_messages);
}

void TopicsMonitorProducer::msgs_lost(
        const types::DdsTopic& topic,
        const types::ParticipantId& participant_id,
        const int number_of_messages /*= 1*/)
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

    logInfo(DDSPIPE_MONITOR, "MONITOR | Lost " << number_of_messages << " messages from Participant " <<
            participant_id << " on Topic " << topic << ".");

    // Register the topic
    topic_data_[topic].name(topic.m_topic_name);
    topic_data_[topic].type_name(topic.type_name);

    // Register the participant id
    participant_data_[topic][participant_id].participant_id(participant_id);

    // Increase the count of the lost messages
    participant_data_[topic][participant_id].msgs_lost(participant_data_[topic][participant_id].msgs_lost() +
            number_of_messages);
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

    logInfo(DDSPIPE_MONITOR, "MONITOR | Discovered type " << type_name << ".");

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

    logInfo(DDSPIPE_MONITOR, "MONITOR | Type mismatch on Topic " << topic << ".");

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

    logInfo(DDSPIPE_MONITOR, "MONITOR | QoS mismatch on Topic " << topic << ".");

    // Register the topic
    topic_data_[topic].name(topic.m_topic_name);
    topic_data_[topic].type_name(topic.type_name);

    // Set the QoS mismatch flag
    topic_data_[topic].qos_mismatch(true);
}

void TopicsMonitorProducer::produce_nts_()
{
    logInfo(DDSPIPE_MONITOR, "MONITOR | Producing MonitoringTopics.");

    std::vector<DdsTopic> topics_data;

    // Iterate through the different topics
    for (auto& topic : topic_data_)
    {
        // Set the type discovered flag
        topic.second.type_discovered(types_discovered_[topic.second.type_name()]);

        std::vector<DdsTopicData> topic_participants;

        for (auto& participant : participant_data_[topic.first])
        {
            // Calculate the message reception rate
            const double period_in_secs = (double) period / 1000;
            participant.second.msg_rx_rate((double) participant.second.msgs_received() / period_in_secs);

            // Save the participant's data for the topic
            topic_participants.push_back(participant.second);
        }

        // Save the participants' data for the topic
        topic.second.data(topic_participants);

        // Save the topic data
        topics_data.push_back(topic.second);
    }

    // Save the topics' data
    data_.topics(topics_data);
}

void TopicsMonitorProducer::consume_nts_()
{
    logInfo(DDSPIPE_MONITOR, "MONITOR | Consuming MonitoringTopics.");

    for (auto& consumer : consumers_)
    {
        consumer->consume(data_);
    }

    reset_data_();
}

void TopicsMonitorProducer::reset_data_()
{
    logInfo(DDSPIPE_MONITOR, "MONITOR | Resetting the messages received and lost for the next period.");

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

} //namespace core
} //namespace ddspipe
} //namespace eprosima

namespace std {

std::ostream& operator <<(
        std::ostream& os,
        const DdsTopicData& data)
{
    os << "Participant ID: " << data.participant_id();
    os << ", Messages Received: " << data.msgs_received();
    os << ", Messages Lost: " << data.msgs_lost();
    os << ", Message Reception Rate: " << data.msg_rx_rate();
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

} // namespace std
