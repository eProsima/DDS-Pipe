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


#include <ddspipe_core/monitoring/clients/TopicsMonitorClient.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {

IMonitorClient* TopicsMonitorClient::get_instance()
{
    return &get_reference();
}

TopicsMonitorClient& TopicsMonitorClient::get_reference()
{
    static TopicsMonitorClient instance;
    return instance;
}

void TopicsMonitorClient::msg_received(
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

IMonitorData* TopicsMonitorClient::save_data() const
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

    MonitoringTopics data;

    // Save the topics' data
    data.topics(topics_data);

    // Create the IMonitorData object
    MonitorTopics* topics = new MonitorTopics();
    topics->data = data;

    return topics;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
