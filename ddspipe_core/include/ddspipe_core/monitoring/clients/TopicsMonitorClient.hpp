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

#include <ddspipe_core/monitoring/clients/IMonitorClient.hpp>
#include <ddspipe_core/types/monitoring/topics/MonitoringTopics.h>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>


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

struct MonitorTopics : public IMonitorData
{
    MonitoringTopics data;
};

/**
 * TODO
 */
class TopicsMonitorClient : IMonitorClient
{
public:

    // Static method to get the instance of the class
    static IMonitorClient* get_instance();

    // TODO
    static TopicsMonitorClient& get_reference();

    // TODO
    void msg_received(
            const types::DdsTopic& topic,
            const types::ParticipantId& participant_id);

    // TODO
    IMonitorData* save_data() const override;

protected:

    // TODO
    TopicsMonitorClient() = default;

    // TODO
    ~TopicsMonitorClient() = default;

    // TODO
    mutable std::mutex topics_mutex_;

    // TODO
    std::map<types::DdsTopic, std::map<types::ParticipantId, MonitoringInfo>> topics_data_;
};

// The names of variables inside macros must be unique to avoid conflicts with external variables
#ifdef MONITOR_ENABLED

#define MONITOR_MSG_RX_IMPL_(topic, participant_id) eprosima::ddspipe::core::TopicsMonitorClient::get_reference().msg_received(topic, \
            participant_id)
#else

#define MONITOR_MSG_RX_IMPL_(topic, participant_id)

#endif // ifndef MONITOR

} // namespace core
} // namespace ddspipe
} // namespace eprosima
