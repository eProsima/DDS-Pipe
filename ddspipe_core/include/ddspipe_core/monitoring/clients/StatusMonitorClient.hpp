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

#include <ddspipe_core/configuration/MonitorStatusConfiguration.hpp>
#include <ddspipe_core/monitoring/clients/IMonitorClient.hpp>
#include <ddspipe_core/monitoring/consumers/IMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/MonitorStatusError.hpp>

#if FASTRTPS_VERSION_MAJOR <= 2 && FASTRTPS_VERSION_MINOR < 13
    #include <ddspipe_core/types/monitoring/status/v1/MonitoringStatus.h>
    #include <ddspipe_core/types/monitoring/status/v1/MonitoringStatusPubSubTypes.h>
#else
    #include <ddspipe_core/types/monitoring/status/v2/MonitoringStatus.h>
    #include <ddspipe_core/types/monitoring/status/v2/MonitoringStatusPubSubTypes.h>
#endif // if FASTRTPS_VERSION_MAJOR <= 2 && FASTRTPS_VERSION_MINOR < 13

// Monitoring API:

// DDSPIPE MONITOR MACROS

//! TODO
#define monitor_error(error) MONITOR_ERROR_IMPL_(error)

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
class StatusMonitorClient : public IMonitorClient
{
public:

    // Static method to get the singleton instance
    static StatusMonitorClient* get_instance();

    // TODO
    void init(const MonitorStatusConfiguration* configuration);

    // TODO
    void consume() const override;

    // TODO
    void add_error_to_status(
            const MonitorStatusError& error);

protected:

    // TODO
    StatusMonitorClient();

    // TODO
    ~StatusMonitorClient();

    // TODO
    MonitoringStatus save_data_() const;

    // TODO
    mutable std::mutex status_mutex_;

    // TODO
    MonitoringStatus status_data_;

    // TODO
    std::vector<IMonitorConsumer<MonitoringStatus>*> consumers_;
};

std::ostream& operator<<(std::ostream& os, const MonitoringStatus& data);

// The names of variables inside macros must be unique to avoid conflicts with external variables
#ifdef MONITOR_ENABLED

#define MONITOR_ERROR_IMPL_(error) eprosima::ddspipe::core::StatusMonitorClient::get_instance()->add_error_to_status(error)

#else

#define MONITOR_ERROR_IMPL_(error)

#endif // ifndef MONITOR

} // namespace core
} // namespace ddspipe
} // namespace eprosima
