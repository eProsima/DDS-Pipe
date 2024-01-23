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


#include <ddspipe_core/monitoring/Monitor.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {

Monitor::Monitor()
{
    start_thread_();
}

Monitor::~Monitor()
{
    stop_thread_();
    clear_consumers();
}

Monitor& Monitor::get_instance()
{
    static Monitor instance;
    return instance;
}

void Monitor::register_consumer(
        IMonitorConsumer* consumer)
{
    std::unique_lock<std::mutex> lock(thread_mutex_);
    consumers_.push_back(consumer);
}

void Monitor::clear_consumers()
{
    std::unique_lock<std::mutex> lock(thread_mutex_);
    consumers_.clear();
}

void Monitor::msg_received(
        const types::DdsTopic& topic,
        const types::ParticipantId& participant_id)
{
    // Take the lock to prevent:
    //      1. Changing the data while it's being saved.
    //      2. Simultaneous calls to msg_received.
    std::lock_guard<std::mutex> lock(topics_mutex_);

    if (!topics_data_.count(topic) || !topics_data_[topic].count(participant_id))
    {
        // First message received for topic & participant

        // Save the time
        topics_data_[topic][participant_id].start_time = utils::now();

        // Save the participant_id
        topics_data_[topic][participant_id].data.participant_id(participant_id);
    }

    // Increase the count of the received messages
    topics_data_[topic][participant_id].data.msgs_received(topics_data_[topic][participant_id].data.msgs_received() + 1);
}

void Monitor::add_error_to_status(
        const MonitorStatusError& error)
{
    // Take the lock to prevent:
    //      1. Changing the data while it's being saved.
    //      2. Simultaneous calls to add_error_to_status.
    std::lock_guard<std::mutex> lock(status_mutex_);

    auto error_status = status_data_.error_status();

    switch (error)
    {
        case MonitorStatusError::MCAP_FILE_CREATION_FAILURE:
            error_status.mcap_file_creation_failure(true);
            break;

        case MonitorStatusError::DISK_FULL:
            error_status.disk_full(true);
            break;

        case MonitorStatusError::TYPE_MISMATCH:
            error_status.type_mismatch(true);
            break;

        case MonitorStatusError::QOS_MISMATCH:
            error_status.qos_mismatch(true);
            break;
    }

    status_data_.error_status(error_status);
}

void Monitor::start_thread_()
{
    enabled_ = true;
    worker_ = std::thread(&Monitor::run_, this);
}

void Monitor::stop_thread_()
{
    {
        std::unique_lock<std::mutex> lock(thread_mutex_);
        enabled_ = false;
    }

    cv_.notify_one();

    if (worker_.joinable())
    {
        worker_.join();
    }
}

void Monitor::run_()
{
    std::unique_lock<std::mutex> lock(thread_mutex_);

    do {
        const auto& topics_data = Monitor::get_instance().save_topics_data_();
        const auto& status_data = Monitor::get_instance().save_status_data_();

        for (const auto& consumer : consumers_)
        {
            consumer->consume_topics(topics_data);
            consumer->consume_status(status_data);
        }

        // Wait for either the stop signal or for 5 seconds to pass
    } while (!cv_.wait_for(lock, std::chrono::milliseconds(period_), [this]
    {
        return !enabled_;
    }));
}

MonitoringData Monitor::save_topics_data_()
{
    // Take the lock to prevent saving the data while it's changing
    std::lock_guard<std::mutex> lock(topics_mutex_);

    std::vector<DdsTopic> topics_data;

    // Iterate through the different topics
    for (const auto& topic : topics_data_)
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

    MonitoringData data;

    // Save the topics' data
    data.topics(topics_data);

    return data;
}

MonitoringStatus Monitor::save_status_data_()
{
    // Take the lock to prevent saving the data while it's changing
    std::lock_guard<std::mutex> lock(status_mutex_);

    return status_data_;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima