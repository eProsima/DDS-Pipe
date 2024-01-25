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
    start_thread_();
}

Monitor::~Monitor()
{
    stop_thread_();
    clear_consumers();
    clear_clients();
}

void Monitor::register_consumer(
        IMonitorConsumer* consumer)
{
    std::unique_lock<std::mutex> lock(thread_mutex_);
    consumers_.push_back(consumer);
}

void Monitor::clear_consumers()
{
    std::unique_lock<std::mutex> lock(thread_mutex_);
    consumers_.clear();
}

void Monitor::register_client(
        IMonitorClient* client)
{
    std::unique_lock<std::mutex> lock(thread_mutex_);
    clients_.push_back(client);
}

void Monitor::clear_clients()
{
    std::unique_lock<std::mutex> lock(thread_mutex_);
    clients_.clear();
}

void Monitor::start_thread_()
{
    enabled_ = true;
    worker_ = std::thread(&Monitor::run_, this);
}

void Monitor::stop_thread_()
{
    {
        std::unique_lock<std::mutex> lock(thread_mutex_);
        enabled_ = false;
    }

    cv_.notify_one();

    if (worker_.joinable())
    {
        worker_.join();
    }
}

void Monitor::run_()
{
    std::unique_lock<std::mutex> lock(thread_mutex_);

    do {
        for (const IMonitorClient* client : clients_)
        {
            IMonitorData* data = client->save_data();

            for (const auto& consumer : consumers_)
            {
                consumer->consume(data);
            }
        }

        // Wait for either the stop signal or for period_ milliseconds to pass
    } while (!cv_.wait_for(lock, std::chrono::milliseconds(period_), [this]
    {
        return !enabled_;
    }));
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
