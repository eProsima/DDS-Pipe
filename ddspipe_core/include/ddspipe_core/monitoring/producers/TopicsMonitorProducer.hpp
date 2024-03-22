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
//

#pragma once

#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_core/configuration/MonitorProducerConfiguration.hpp>
#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/monitoring/consumers/IMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/producers/MonitorProducer.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

#if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopicsPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopicsTypeObject.h>
#else
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopicsPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopicsTypeObject.h>
#endif // if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)


// Macro to notify that a message has been received in a topic by a participant.
#define monitor_msg_rx(topic, participant_id) MONITOR_MSG_RX_IMPL_(topic, participant_id)

// Macro to notify that n messages have been received in a topic by a participant.
#define monitor_msgs_rx(topic, participant_id, n) MONITOR_MSGS_RX_IMPL_(topic, participant_id, n)

// Macro to notify that a message has been lost in a topic by a participant.
#define monitor_msg_lost(topic, participant_id) MONITOR_MSG_LOST_IMPL_(topic, participant_id)

// Macro to notify that n messages have been lost in a topic by a participant.
#define monitor_msgs_lost(topic, participant_id, n) MONITOR_MSGS_LOST_IMPL_(topic, participant_id, n)

// Macro to notify that a type has been discovered.
#define monitor_type_discovered(type_name) MONITOR_TYPE_DISCOVERED_IMPL_(type_name)

// Macro to notify that there has been a type mismatch in a topic.
#define monitor_type_mismatch(topic) MONITOR_TYPE_MISMATCH_IMPL_(topic)

// Macro to notify that there has been a QoS mismatch in a topic.
#define monitor_qos_mismatch(topic) MONITOR_QOS_MISMATCH_IMPL_(topic)

namespace eprosima {
namespace ddspipe {
namespace core {

const std::string TOPICS_MONITOR_PRODUCER_ID = "topics";

/**
 * @brief Producer of the \c MonitoringTopics.
 *
 * The \c TopicsMonitorProducer produces the \c MonitoringTopics by gathering data with its macros:
 * - \c monitor_msg_rx
 * - \c monitor_msg_lost
 * - \c monitor_type_discovered
 * - \c monitor_type_mismatch
 * - \c monitor_qos_mismatch
 *
 * The \c TopicsMonitorProducer consumes the \c MonitoringTopics by using its consumers.
 *
 * @note It is a singleton class so its macros can be called from anywhere in the code.
 */
class TopicsMonitorProducer : public MonitorProducer
{
public:

    /**
     * @brief Destroy the \c TopicsMonitorProducer.
     */
    virtual ~TopicsMonitorProducer() = default;

    /**
     * @brief Initialize the instance of the \c TopicsMonitorProducer.
     *
     * Applications can initialize the instance of the \c TopicsMonitorProducer with derived classes.
     *
     * @param instance Instance of the \c TopicsMonitorProducer.
     */
    DDSPIPE_CORE_DllAPI
    static void init_instance(
            std::unique_ptr<TopicsMonitorProducer> instance);

    /**
     * @brief Get the instance of the \c TopicsMonitorProducer.
     *
     * If the instance has not been initialized, it will be initialized with the default configuration.
     *
     * @return Instance of the \c TopicsMonitorProducer.
     */
    DDSPIPE_CORE_DllAPI
    static TopicsMonitorProducer* get_instance();

    /**
     * @brief Enable the \c TopicsMonitorProducer.
     */
    DDSPIPE_CORE_DllAPI
    void enable() override;

    /**
     * @brief Disable the \c TopicsMonitorProducer.
     */
    DDSPIPE_CORE_DllAPI
    void disable() override;

    /**
     * @brief Register a consumer.
     *
     * The consumer can be any class that implements the \c IMonitorConsumer interface as long as it is a template class
     * that accepts the \c MonitoringTopics as a template parameter.
     *
     * @param consumer Consumer to be registered.
     */
    DDSPIPE_CORE_DllAPI
    void register_consumer(
            std::unique_ptr<IMonitorConsumer<MonitoringTopics>> consumer);

    /**
     * @brief Remove all consumers.
     */
    DDSPIPE_CORE_DllAPI
    void clear_consumers() override;

    /**
     * @brief Produce and consume the \c MonitoringTopics.
     *
     * Produces a \c MonitoringTopics with the data gathered and consumes it.
     */
    DDSPIPE_CORE_DllAPI
    void produce_and_consume() override;

    /**
     * @brief Produce the \c MonitoringTopics.
     *
     * Generates a \c MonitoringTopics with the data gathered.
     */
    DDSPIPE_CORE_DllAPI
    void produce() override;

    /**
     * @brief Consume the \c MonitoringTopics.
     *
     * Calls the consume method of its consumers.
     */
    DDSPIPE_CORE_DllAPI
    void consume() override;

    ///////////////////
    // Data methods ///
    ///////////////////

    /**
     * @brief Clear the data gathered.
     */
    DDSPIPE_CORE_DllAPI
    void clear_data() override;

    /**
     * @brief Increase the number of messages received in a \c topic by a participant.
     *
     * Method called by the \c monitor_msg_rx macro.
     *
     * @param topic Topic where the message was received.
     * @param participant_id Participant that received the message.
     */
    DDSPIPE_CORE_DllAPI
    void msgs_received(
            const types::DdsTopic& topic,
            const types::ParticipantId& participant_id,
            const int number_of_messages = 1);

    /**
     * @brief Increase the number of messages lost in a \c topic by a participant.
     *
     * Method called by the \c monitor_msg_lost macro.
     *
     * @param topic Topic where the message was lost.
     * @param participant_id Participant that noticed the message was lost.
     */
    DDSPIPE_CORE_DllAPI
    void msgs_lost(
            const types::DdsTopic& topic,
            const types::ParticipantId& participant_id,
            const int number_of_messages = 1);

    /**
     * @brief Set a type as discovered in the \c MonitoringTopics.
     *
     * Method called by the \c monitor_type_discovered macro.
     *
     * @param type_name Name of the type discovered.
     */
    DDSPIPE_CORE_DllAPI
    void type_discovered(
            const std::string& type_name);

    /**
     * @brief Set a type mismatch in a \c topic.
     *
     * Method called by the \c monitor_type_mismatch macro.
     *
     * @param topic Topic with the type mismatch.
     */
    DDSPIPE_CORE_DllAPI
    void type_mismatch(
            const types::DdsTopic& topic);

    /**
     * @brief Set a QoS mismatch in a \c topic.
     *
     * Method called by the \c monitor_qos_mismatch macro.
     *
     * @param topic Topic with the QoS mismatch.
     */
    DDSPIPE_CORE_DllAPI
    void qos_mismatch(
            const types::DdsTopic& topic);

protected:

    // Produce data_.
    void produce_nts_();

    // Consume data_.
    void consume_nts_();

    // Generate the MonitoringTopics to be consumed.
    void reset_data_();

    // Instance of the TopicsMonitorProducer.
    static std::unique_ptr<TopicsMonitorProducer> instance_;

    // Mutex to protect the TopicsMonitorProducer.
    static std::mutex mutex_;

    // The produced data.
    MonitoringTopics data_;

    // Data specific to a Topic.
    std::map<types::DdsTopic, DdsTopic> topic_data_;

    // Data specific to a Participant.
    std::map<types::DdsTopic, std::map<types::ParticipantId, DdsTopicData>> participant_data_;

    // The types that have been discovered.
    std::map<std::string, bool> types_discovered_;

    // Vector of consumers of the MonitoringTopics.
    std::vector<std::unique_ptr<IMonitorConsumer<MonitoringTopics>>> consumers_;
};


// The names of variables inside macros must be unique to avoid conflicts with external variables
#define MONITOR_MSG_RX_IMPL_(topic, participant_id) \
    eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()->msgs_received(topic, participant_id)

#define MONITOR_MSGS_RX_IMPL_(topic, participant_id, n) \
    eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()->msgs_received(topic, participant_id, n)

#define MONITOR_MSG_LOST_IMPL_(topic, participant_id) \
    eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()->msgs_lost(topic, participant_id)

#define MONITOR_MSGS_LOST_IMPL_(topic, participant_id, n) \
    eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()->msgs_lost(topic, participant_id, n)

#define MONITOR_TYPE_DISCOVERED_IMPL_(type_name) eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()-> \
            type_discovered(type_name)

#define MONITOR_TYPE_MISMATCH_IMPL_(topic) eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()->type_mismatch( \
        topic)

#define MONITOR_QOS_MISMATCH_IMPL_(topic) eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()->qos_mismatch( \
        topic)

} // namespace core
} // namespace ddspipe
} // namespace eprosima

namespace std {

std::ostream& operator <<(
        std::ostream& os,
        const DdsTopicData& data);

std::ostream& operator <<(
        std::ostream& os,
        const DdsTopic& topic);

std::ostream& operator <<(
        std::ostream& os,
        const MonitoringTopics& data);

} // namespace std
