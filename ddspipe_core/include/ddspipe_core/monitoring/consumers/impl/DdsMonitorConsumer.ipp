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

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {


template <typename T>
DdsMonitorConsumer<T>::DdsMonitorConsumer(
        const types::DomainIdType& domain,
        const std::string& topic_name,
        fastdds::dds::TypeSupport& type)
{
    // Get the participant from the factory
    fastdds::dds::DomainParticipant* participant = get_participant(domain);

    // Register the type
    type.register_type(participant);

    // Create the publisher
    fastdds::dds::Publisher* publisher = participant->create_publisher(fastdds::dds::PUBLISHER_QOS_DEFAULT, nullptr);

    if (publisher == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Publisher for Participant " <<
                      participant->get_qos().name() << ".");
    }

    // Create the topic
    fastdds::dds::TopicQos tqos = fastdds::dds::TOPIC_QOS_DEFAULT;
    tqos.reliability().kind = fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    tqos.durability().kind = fastdds::dds::VOLATILE_DURABILITY_QOS;

    fastdds::dds::Topic* topic = participant->create_topic(topic_name, type.get_type_name(), tqos);

    if (topic == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Topic " << topic_name <<
                  " for Participant " << participant->guid() << ".");
    }

    // Create the writer
    fastdds::dds::DataWriterQos wqos = fastdds::dds::DATAWRITER_QOS_DEFAULT;

    wqos.data_sharing().automatic();
    wqos.publish_mode().kind = fastdds::dds::SYNCHRONOUS_PUBLISH_MODE;
    wqos.reliability().kind = fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    wqos.durability().kind = fastdds::dds::VOLATILE_DURABILITY_QOS;

    writer_ = publisher->create_datawriter(topic, wqos);

    if (writer_ == nullptr)
    {
        throw utils::InitializationException(
                utils::Formatter() << "Error creating DataWriter for Participant " <<
                      participant->guid() << " in topic " << topic << ".");
    }
}

template <typename T>
void DdsMonitorConsumer<T>::consume(const T* data) const
{
    // The data must be copied since writer_->write takes a non-constant pointer.
    T data_copy = *data;
    writer_->write(&data_copy);
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
