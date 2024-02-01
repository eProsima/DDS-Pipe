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

#include <ddspipe_core/configuration/MonitorTopicsConfiguration.hpp>
#include <ddspipe_core/monitoring/clients/IMonitorClient.hpp>
#include <ddspipe_core/monitoring/consumers/IMonitorConsumer.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

#if FASTRTPS_VERSION_MAJOR <= 2 && FASTRTPS_VERSION_MINOR < 13
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopicsPubSubTypes.h>
#else
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopicsPubSubTypes.h>
#endif // if FASTRTPS_VERSION_MAJOR <= 2 && FASTRTPS_VERSION_MINOR < 13


// Monitoring API:

// DDSPIPE MONITOR MACROS
//! TODO
#define monitor_msg_rx(topic, participant_id) MONITOR_MSG_RX_IMPL_(topic, participant_id)


namespace eprosima {
namespace ddspipe {
namespace core {

struct MonitoringInfo
{
    utils::Timestamp start_time;
    DdsTopicData data;
};

/**
 * TODO
 */
class TopicsMonitorClient : public IMonitorClient
{
public:

    // TODO
    static TopicsMonitorClient* get_instance();

    // TODO
    void init(const MonitorTopicsConfiguration* configuration);

    // TODO
    void consume() const override;

    // TODO
    void msg_received(
            const types::DdsTopic& topic,
            const types::ParticipantId& participant_id);

protected:

    // TODO
    TopicsMonitorClient();

    // TODO
    ~TopicsMonitorClient();

    // TODO
    MonitoringTopics save_data_() const;

    // TODO
    mutable std::mutex topics_mutex_;

    // TODO
    std::map<types::DdsTopic, std::map<types::ParticipantId, MonitoringInfo>> topics_data_;

    // TODO
    std::vector<IMonitorConsumer<MonitoringTopics>*> consumers_;
};

std::ostream& operator<<(std::ostream& os, const DdsTopicData& data);
std::ostream& operator<<(std::ostream& os, const DdsTopic& topic);
std::ostream& operator<<(std::ostream& os, const MonitoringTopics& data);

// The names of variables inside macros must be unique to avoid conflicts with external variables
#ifdef MONITOR_ENABLED

#define MONITOR_MSG_RX_IMPL_(topic, participant_id) eprosima::ddspipe::core::TopicsMonitorClient::get_instance()->msg_received(topic, \
            participant_id)
#else

#define MONITOR_MSG_RX_IMPL_(topic, participant_id)

#endif // ifndef MONITOR

} // namespace core
} // namespace ddspipe
} // namespace eprosima
