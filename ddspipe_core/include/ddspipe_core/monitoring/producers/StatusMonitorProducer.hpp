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
#include <ddspipe_core/monitoring/consumers/IMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/producers/IMonitorProducer.hpp>

#include <ddspipe_core/types/monitoring/status/v2/MonitoringStatus.h>
#include <ddspipe_core/types/monitoring/status/v2/MonitoringStatusPubSubTypes.h>

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
class StatusMonitorProducer : public IMonitorProducer
{
public:

    // TODO
    static void init_instance(StatusMonitorProducer* instance);

    // Static method to get the singleton instance
    static StatusMonitorProducer* get_instance();

    // TODO
    virtual void init(const MonitorStatusConfiguration* configuration);

    // TODO
    virtual void consume() const override;

    // TODO
    virtual void add_error_to_status(
            const std::string& error);

protected:

    // TODO
    StatusMonitorProducer() = default;

    // TODO
    ~StatusMonitorProducer();

    // TODO
    virtual MonitoringStatus* save_data_() const;

    // TODO
    static StatusMonitorProducer* instance_;

    // TODO
    mutable std::mutex mutex_;

    // TODO
    MonitoringStatus* data_ = new MonitoringStatus();

    // TODO
    std::vector<IMonitorConsumer<MonitoringStatus>*> consumers_;
};

std::ostream& operator<<(std::ostream& os, const MonitoringStatus& data);

// The names of variables inside macros must be unique to avoid conflicts with external variables
#ifdef MONITOR_ENABLED

#define MONITOR_ERROR_IMPL_(error) eprosima::ddspipe::core::StatusMonitorProducer::get_instance()->add_error_to_status(error)

#else

#define MONITOR_ERROR_IMPL_(error)

#endif // ifndef MONITOR

} // namespace core
} // namespace ddspipe
} // namespace eprosima
