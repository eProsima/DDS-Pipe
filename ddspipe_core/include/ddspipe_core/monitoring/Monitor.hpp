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

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_core/monitoring/IMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/MonitorStatusError.hpp>
#include <ddspipe_core/types/monitoring/status/MonitoringStatus.h>
#include <ddspipe_core/types/monitoring/topics/MonitoringData.h>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>


// Monitoring API:

// DDSPIPE MONITOR MACROS
//! TODO
#define monitor_msg_rx(topic, participant_id) MONITOR_MSG_RX_IMPL_(topic, participant_id)

//! TODO
#define monitor_error(error) MONITOR_ERROR_IMPL_(error)

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
class Monitor
{
public:

    // Static method to get the instance of the class
    static Monitor& get_instance();

    // TODO
    void msg_received(
            const types::DdsTopic& topic,
            const types::ParticipantId& participant_id);

    // TODO
    void add_error_to_status(
            const MonitorStatusError& error);

    // TODO
    void register_consumer(
            IMonitorConsumer* consumer);

    // TODO
    void clear_consumers();

protected:

    // TODO
    Monitor();

    // TODO
    ~Monitor();

    // TODO
    void start_thread_();

    // TODO
    void stop_thread_();

    // TODO
    void run_();

    // TODO
    MonitoringData save_topics_data_();

    // TODO
    MonitoringStatus save_status_data_();

    // TODO
    std::thread worker_;

    // TODO
    std::atomic<bool> enabled_;

    // TODO
    std::mutex thread_mutex_;

    // TODO
    std::condition_variable cv_;

    // TODO
    std::mutex topics_mutex_;

    // TODO
    std::mutex status_mutex_;

    // TODO
    std::map<types::DdsTopic, std::map<types::ParticipantId, MonitoringInfo>> topics_data_;

    // TODO
    MonitoringStatus status_data_;

    // TODO
    std::vector<IMonitorConsumer*> consumers_;

    // TODO
    int period_ = 1000;
};

// The names of variables inside macros must be unique to avoid conflicts with external variables
// TODO: change to the positive case
#if !MONITOR_ENABLED

#define MONITOR_MSG_RX_IMPL_(topic, participant_id) eprosima::ddspipe::core::Monitor::get_instance().msg_received(topic, \
            participant_id)

#define MONITOR_ERROR_IMPL_(error) eprosima::ddspipe::core::Monitor::get_instance().add_error_to_status(error)

#else

#define MONITOR_MSG_RX_IMPL_(topic, participant_id)

#define MONITOR_ERROR_IMPL_(error)

#endif // ifndef MONITOR

} // namespace core
} // namespace ddspipe
} // namespace eprosima
