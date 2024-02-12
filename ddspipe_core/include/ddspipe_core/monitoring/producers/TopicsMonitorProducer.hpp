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

#include <mutex>

#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_core/configuration/MonitorProducerConfiguration.hpp>
#include <ddspipe_core/monitoring/consumers/IMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/producers/IMonitorProducer.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

#if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopicsPubSubTypes.h>
#else
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopicsPubSubTypes.h>
#endif // if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)

// DDSPIPE MONITOR MACROS

//! TODO
#define monitor_msg_rx(topic, participant_id) MONITOR_MSG_RX_IMPL_(topic, participant_id)

//! TODO
#define monitor_msg_lost(topic, participant_id) MONITOR_MSG_LOST_IMPL_(topic, participant_id)

//! TODO
#define monitor_type_discovered(topic) MONITOR_TYPE_DISCOVERED(topic)

//! TODO
#define monitor_type_mismatch(topic) MONITOR_TYPE_MISMATCH(topic)

//! TODO
#define monitor_qos_mismatch(topic) MONITOR_QOS_MISMATCH(topic)

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
class TopicsMonitorProducer : public IMonitorProducer
{
public:

    // TODO
    static TopicsMonitorProducer* get_instance();

    // TODO
    void init(
            const MonitorProducerConfiguration& configuration);

    // TODO
    void consume() override;

    // TODO
    void msg_received(
            const types::DdsTopic& topic,
            const types::ParticipantId& participant_id);

    // TODO
    void msg_lost(
            const types::DdsTopic& topic,
            const types::ParticipantId& participant_id);

    // TODO
    void type_discovered(
            const types::DdsTopic& topic);

    // TODO
    void type_mismatch(
            const types::DdsTopic& topic);

    // TODO
    void qos_mismatch(
            const types::DdsTopic& topic);

protected:

    // TODO
    TopicsMonitorProducer() = default;

    // TODO
    ~TopicsMonitorProducer();

    // TODO
    MonitoringTopics save_data_();

    // TODO
    void reset_data_();

    // TODO
    mutable std::mutex mutex_;

    // TODO
    std::map<types::DdsTopic, DdsTopic> topic_data_;

    // TODO
    std::map<types::DdsTopic, std::map<types::ParticipantId, DdsTopicData>> participant_data_;

    // TODO
    std::vector<IMonitorConsumer<MonitoringTopics>*> consumers_;
};

std::ostream& operator <<(
        std::ostream& os,
        const DdsTopicData& data);

std::ostream& operator <<(
        std::ostream& os,
        const DdsTopic& topic);

std::ostream& operator <<(
        std::ostream& os,
        const MonitoringTopics& data);

// The names of variables inside macros must be unique to avoid conflicts with external variables
#define MONITOR_MSG_RX_IMPL_(topic, \
            participant_id) eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()->msg_received(topic, \
            participant_id)

#define MONITOR_MSG_LOST_IMPL_(topic, \
            participant_id) eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()->msg_lost(topic, \
            participant_id)

#define MONITOR_TYPE_DISCOVERED_IMPL_(topic) eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()-> \
            type_discovered(topic)

#define MONITOR_TYPE_MISMATCH_IMPL_(topic) eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()->type_mismatch( \
        topic)

#define MONITOR_QOS_MISMATCH_IMPL_(topic) eprosima::ddspipe::core::TopicsMonitorProducer::get_instance()->qos_mismatch( \
        topic)

} // namespace core
} // namespace ddspipe
} // namespace eprosima
