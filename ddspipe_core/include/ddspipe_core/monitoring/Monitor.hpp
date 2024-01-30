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

#include <ddspipe_core/monitoring/clients/IMonitorClient.hpp>
#include <ddspipe_core/monitoring/consumers/IMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/MonitorStatusError.hpp>
#include <ddspipe_core/types/monitoring/status/MonitoringStatus.h>
#include <ddspipe_core/types/monitoring/topics/MonitoringTopics.h>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>


// Monitoring API:

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
class Monitor
{
public:

    // TODO
    Monitor();

    // TODO
    ~Monitor();

    // TODO
    void register_client(
            IMonitorClient* client);

    // TODO
    void clear_clients();

protected:

    // TODO
    void start_thread_();

    // TODO
    void stop_thread_();

    // TODO
    void run_();

    // TODO
    std::thread worker_;

    // TODO
    std::atomic<bool> enabled_;

    // TODO
    std::mutex thread_mutex_;

    // TODO
    std::condition_variable cv_;

    // TODO
    std::vector<IMonitorClient*> clients_;

    // TODO
    int period_ = 1000;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima
