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
    : type_(new MonitoringDataPubSubType())
{
    fastdds::dds::DomainParticipantQos pqos;
    pqos.name("DdsPipeMonitorParticipant");

    // CREATE THE PARTICIPANT
    participant_ = fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(85, pqos);

    if (participant_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Participant " <<
                      pqos.name() << ".");
    }

    // REGISTER THE TYPE
    type_.register_type(participant_);

    // CREATE THE PUBLISHER
    publisher_ = participant_->create_publisher(fastdds::dds::PUBLISHER_QOS_DEFAULT, nullptr);

    if (publisher_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Publisher for Participant " <<
                      pqos.name() << ".");
    }

    // CREATE THE TOPIC
    topic_ = participant_->create_topic("MonitoringDataTopic", "MonitoringData", fastdds::dds::TOPIC_QOS_DEFAULT);

    if (topic_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Topic for Participant " <<
                      pqos.name() << ".");
    }

    // CREATE THE WRITER
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

Monitor::~Monitor()
{
    fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(participant_);
}

Monitor& Monitor::get_instance()
{
    static Monitor instance; // Guaranteed to be destroyed and instantiated on first use
    return instance;
}

void Monitor::msg_received(const types::DdsTopic& topic, const types::ParticipantId& participant_id)
{
    // Lock to avoid race conditions
    std::lock_guard<std::mutex> lock(mutex_);

    if (!data_.count(topic) || !data_[topic].count(participant_id))
    {
        // First message received for topic. Save the time.
        data_[topic][participant_id].start_time = utils::now();
        data_[topic][participant_id].data.participant_id(participant_id);
    }

    data_[topic][participant_id].data.msgs_received(data_[topic][participant_id].data.msgs_received() + 1);

    auto data = save_data();
    writer_->write(&data);
}

MonitoringData Monitor::save_data()
{
    // Take the lock to prevent the data from changing while being saved
    // std::lock_guard<std::mutex> lock(mutex_);

    std::vector<DdsTopic> topics;

    // Iterate through the different topics
    for (const auto& topic : data_)
    {
        const auto& dds_topic = topic.first;
        const auto& participants = topic.second;

        DdsTopic topic_data;

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

            topic_participants.push_back(participant_data);
        }

        topic_data.data(topic_participants);

        topics.push_back(topic_data);
    }

    MonitoringData data;
    data.topics(topics);

    return data;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
