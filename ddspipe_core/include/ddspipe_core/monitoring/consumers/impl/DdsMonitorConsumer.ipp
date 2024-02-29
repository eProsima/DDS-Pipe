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

#include <type_traits>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/DynamicDataPtr.h>
#include <fastrtps/types/TypeObjectFactory.h>

#include <cpp_utils/exception/InitializationException.hpp>

#if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopicsPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopicsTypeObject.h>
#else
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopicsPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopicsTypeObject.h>
#endif // if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)


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
    type->auto_fill_type_information(true);
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
void DdsMonitorConsumer<T>::consume(
        const T* data) const
{
    if constexpr (std::is_same<T, MonitoringTopics>::value)
    {
        // TODO(tempate): Remove this method and use the generic one
        auto type_object = GetMonitoringTopicsObject(true);
        auto type_id = GetMonitoringTopicsIdentifier(true);
        fastrtps::types::DynamicType_ptr dyn_type = fastrtps::types::TypeObjectFactory::get_instance()->build_dynamic_type(
                "MonitoringTopics",
                type_id,
                type_object);

        // Create and initialize new data
        fastrtps::types::DynamicData_ptr new_data;
        new_data = fastrtps::types::DynamicDataFactory::get_instance()->create_data(dyn_type);

        for (const auto& topic : data->topics())
        {
            eprosima::fastrtps::types::DynamicData* sequence = new_data->loan_value(0);
            eprosima::fastrtps::types::DynamicType_ptr seq_elem_type = sequence->get_type()->get_descriptor()->get_element_type();

            eprosima::fastrtps::types::DynamicData_ptr seq_elem;
            seq_elem = eprosima::fastrtps::types::DynamicDataFactory::get_instance()->create_data(seq_elem_type);
            seq_elem->set_string_value(topic.name(), 0);
            seq_elem->set_string_value(topic.type_name(), 1);
            seq_elem->set_bool_value(topic.type_discovered(), 2);
            seq_elem->set_bool_value(topic.type_mismatch(), 3);
            seq_elem->set_bool_value(topic.qos_mismatch(), 5);

            for (const auto& participant : topic.data())
            {
                eprosima::fastrtps::types::DynamicData* sequence2 = seq_elem->loan_value(4);
                eprosima::fastrtps::types::DynamicType_ptr seq_elem_type2 = sequence2->get_type()->get_descriptor()->get_element_type();

                eprosima::fastrtps::types::DynamicData_ptr seq_elem2;
                seq_elem2 = eprosima::fastrtps::types::DynamicDataFactory::get_instance()->create_data(seq_elem_type2);

                seq_elem2->set_string_value(participant.participant_id(), 0);
                seq_elem2->set_uint32_value(participant.msgs_lost(), 1);
                seq_elem2->set_uint32_value(participant.msgs_received(), 2);
                seq_elem2->set_float64_value(participant.frequency(), 3);

                eprosima::fastrtps::types::MemberId id2;
                sequence2->insert_complex_value(seq_elem2, id2);
                seq_elem->return_loaned_value(sequence2);
            }

            eprosima::fastrtps::types::MemberId id;
            sequence->insert_complex_value(seq_elem, id);

            new_data->return_loaned_value(sequence);
        }

        writer_->write(new_data.get());
    }
    else
    {
        // The data must be copied since writer_->write takes a non-constant pointer.
        T data_copy = *data;
        writer_->write(&data_copy);
    }
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
