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
#include <ddspipe_core/library/library_dll.h>
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

// Macro to add an error to the MonitoringStatus.
#define monitor_error(error) MONITOR_ERROR_IMPL_(error)

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * @brief Producer of the \c MonitoringStatus.
 *
 * The \c StatusMonitorProducer produces the \c MonitoringStatus by gathering data with its macros:
 * - \c monitor_error
 *
 * The \c StatusMonitorProducer consumes the \c MonitoringStatus by using its consumers.
 *
 * @note It is a singleton class so its macros can be called from anywhere in the code.
 */
class StatusMonitorProducer : public MonitorProducer
{
public:

    /**
     * @brief Initialize the instance of the \c StatusMonitorProducer.
     *
     * Applications can initialize the instance of the \c StatusMonitorProducer with derived classes.
     *
     * @param instance Instance of the \c StatusMonitorProducer.
     */
    DDSPIPE_CORE_DllAPI
    static void init_instance(
            std::unique_ptr<StatusMonitorProducer> instance);

    /**
     * @brief Get the instance of the \c StatusMonitorProducer.
     *
     * If the instance has not been initialized, it will be initialized with the default configuration.
     *
     * @return Instance of the \c StatusMonitorProducer.
     */
    DDSPIPE_CORE_DllAPI
    static StatusMonitorProducer* get_instance();

    /**
     * @brief Register a consumer.
     *
     * The consumer can be any class that implements the \c IMonitorConsumer interface as long as it is a template class
     * that accepts the \c MonitoringStatus as a template parameter.
     *
     * @param consumer Consumer to be registered.
     */
    DDSPIPE_CORE_DllAPI
    virtual void register_consumer(
            std::unique_ptr<IMonitorConsumer<MonitoringStatus>> consumer);

    /**
     * @brief Consume the \c MonitoringStatus.
     *
     * To consume data, the \c StatusMonitorProducer saves the \c MonitoringStatus and then calls the consume method of
     * its consumers.
     */
    DDSPIPE_CORE_DllAPI
    void consume() override;

    /**
     * @brief Add an error to the \c MonitoringStatus.
     *
     * Method called by the \c monitor_error macro to.
     *
     * @param error String identifying the error to be added to the \c MonitoringStatus.
     */
    DDSPIPE_CORE_DllAPI
    virtual void add_error_to_status(
            const std::string& error);

protected:

    // Generate the MonitoringStatus to be consumed.
    virtual MonitoringStatus* save_data_() const;

    // Instance of the StatusMonitorProducer.
    static std::unique_ptr<StatusMonitorProducer> instance_;

    // Mutex to protect the StatusMonitorProducer.
    mutable std::mutex mutex_;

    // The produced data.
    std::unique_ptr<MonitoringStatus> data_ = std::make_unique<MonitoringStatus>();

    // Vector of consumers of the MonitoringStatus.
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
