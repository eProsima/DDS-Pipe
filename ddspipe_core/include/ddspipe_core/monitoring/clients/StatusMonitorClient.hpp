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

#include <ddspipe_core/monitoring/clients/IMonitorClient.hpp>
#include <ddspipe_core/monitoring/MonitorStatusError.hpp>
#include <ddspipe_core/types/monitoring/status/MonitoringStatus.h>

// Monitoring API:

// DDSPIPE MONITOR MACROS

//! TODO
#define monitor_error(error) MONITOR_ERROR_IMPL_(error)

namespace eprosima {
namespace ddspipe {
namespace core {

struct MonitorStatus : public IMonitorData
{
    MonitoringStatus data;
};

/**
 * TODO
 */
class StatusMonitorClient : IMonitorClient
{
public:

    // Static method to get the singleton instance
    static IMonitorClient* get_instance();

    // Static method to get the reference to the singleton instance
    static StatusMonitorClient& get_reference();

    // TODO
    void add_error_to_status(
            const MonitorStatusError& error);

    // TODO
    IMonitorData* save_data() const override;

protected:

    // TODO
    StatusMonitorClient() = default;

    // TODO
    ~StatusMonitorClient() = default;

    // TODO
    mutable std::mutex status_mutex_;

    // TODO
    MonitoringStatus status_data_;

    // TODO
    int period_ = 1000;
};

// The names of variables inside macros must be unique to avoid conflicts with external variables
#ifdef MONITOR_ENABLED

#define MONITOR_ERROR_IMPL_(error) eprosima::ddspipe::core::StatusMonitorClient::get_reference().add_error_to_status(error)

#else

#define MONITOR_ERROR_IMPL_(error)

#endif // ifndef MONITOR

} // namespace core
} // namespace ddspipe
} // namespace eprosima
