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

#include <memory>
#include <mutex>

#include <ddspipe_core/configuration/MonitorProducerConfiguration.hpp>
#include <ddspipe_core/monitoring/consumers/IMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/producers/MonitorProducer.hpp>

#if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)
    #include <ddspipe_core/types/monitoring/status/v1/MonitoringStatus.h>
    #include <ddspipe_core/types/monitoring/status/v1/MonitoringStatusPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/status/v1/MonitoringStatusTypeObject.h>
#else
    #include <ddspipe_core/types/monitoring/status/v2/MonitoringStatus.h>
    #include <ddspipe_core/types/monitoring/status/v2/MonitoringStatusPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/status/v2/MonitoringStatusTypeObject.h>
#endif // if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)

// DDSPIPE MONITOR MACROS

//! TODO
#define monitor_error(error) MONITOR_ERROR_IMPL_(error)

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
class StatusMonitorProducer : public MonitorProducer
{
public:

    // TODO
    static void init_instance(
            std::unique_ptr<StatusMonitorProducer> instance);

    // Static method to get the singleton instance
    static StatusMonitorProducer* get_instance();

    // TODO
    virtual void register_consumer(
            std::unique_ptr<IMonitorConsumer<MonitoringStatus>> consumer);

    // TODO
    virtual void consume() override;

    // TODO
    virtual void add_error_to_status(
            const std::string& error);

protected:

    // TODO
    virtual MonitoringStatus* save_data_() const;

    // TODO
    static std::unique_ptr<StatusMonitorProducer> instance_;

    // TODO
    mutable std::mutex mutex_;

    // TODO
    std::unique_ptr<MonitoringStatus> data_ = std::make_unique<MonitoringStatus>();

    // TODO
    std::vector<std::unique_ptr<IMonitorConsumer<MonitoringStatus>>> consumers_;
};

std::ostream& operator <<(
        std::ostream& os,
        const MonitoringStatus& data);

// The names of variables inside macros must be unique to avoid conflicts with external variables
#define MONITOR_ERROR_IMPL_(error) eprosima::ddspipe::core::StatusMonitorProducer::get_instance()->add_error_to_status( \
        error)

} // namespace core
} // namespace ddspipe
} // namespace eprosima
