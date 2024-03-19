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


#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>

#include <cpp_utils/Log.hpp>
#include <cpp_utils/exception/InitializationException.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {


template <typename T>
DdsMonitorConsumer<T>::DdsMonitorConsumer(
        const DdsPublishingConfiguration& configuration,
        DdsMonitorParticipantRegistry& registry,
        fastdds::dds::TypeSupport& type)
{
    // Get the participant from the factory
    participant_ = registry.get_participant(configuration.domain);

    // Register the type
    type.register_type(participant_);

    // Create the topic
    topic_ = participant_->create_topic(configuration.topic_name, type.get_type_name(),
                    fastdds::dds::TOPIC_QOS_DEFAULT);

    if (topic_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Topic " << configuration.topic_name <<
                      " for Participant " << participant_->guid() << ".");
    }

    // Create the publisher
    publisher_ = participant_->create_publisher(fastdds::dds::PUBLISHER_QOS_DEFAULT, nullptr);

    if (publisher_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Publisher for Participant " <<
                      participant_->get_qos().name() << ".");
    }

    // Create the writer
    writer_ = publisher_->create_datawriter(topic_, fastdds::dds::DATAWRITER_QOS_DEFAULT);

    if (writer_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating DataWriter for Participant " <<
                      participant_->guid() << " in topic " << topic_ << ".");
    }

    logInfo(DDSPIPE_MONITOR, "MONITOR | Created DataWriter " << writer_ << " for Participant " <<
            participant_->guid() << " in topic " << topic_ << ".");
}

template <typename T>
DdsMonitorConsumer<T>::~DdsMonitorConsumer()
{
    if (publisher_ != nullptr)
    {
        if (writer_ != nullptr)
        {
            publisher_->delete_datawriter(writer_);
        }

        participant_->delete_publisher(publisher_);
    }

    if (topic_ != nullptr)
    {
        participant_->delete_topic(topic_);
    }
}

template <typename T>
void DdsMonitorConsumer<T>::consume(
        const T& data) const
{
    writer_->write(&const_cast<T&>(data));
}

template <typename T>
std::string DdsMonitorConsumer<T>::get_name() const
{
    return "DdsMonitorConsumer";
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
