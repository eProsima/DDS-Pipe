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
{
    fastdds::rtps::ThreadSettings thread_config;
    event_handler_.init_thread(thread_config, "ddspipe.monitor%u");
}

Monitor::~Monitor()
{
    event_handler_.stop_thread();
    clear_producers();
}

void Monitor::register_producer(
        IMonitorProducer* producer)
{
    std::unique_lock<std::mutex> lock(mutex_);

    events_.emplace_back(std::make_unique<fastrtps::rtps::TimedEvent>(
        event_handler_,
        [producer]() -> bool
        {
            producer->consume();

            // The return value must be true to reschedule the event.
            return true;
        },
        producer->period));

    // Start the timer
    events_.back()->restart_timer();
}

void Monitor::clear_producers()
{
    std::unique_lock<std::mutex> lock(mutex_);
    events_.clear();
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
