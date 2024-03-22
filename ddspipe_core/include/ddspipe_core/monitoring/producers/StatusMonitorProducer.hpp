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
#include <string>

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

const std::string STATUS_MONITOR_PRODUCER_ID = "status";

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
     * @brief Destroy the \c StatusMonitorProducer.
     */
    virtual ~StatusMonitorProducer() = default;

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
     * @brief Enable the \c StatusMonitorProducer.
     */
    DDSPIPE_CORE_DllAPI
    void enable() override;

    /**
     * @brief Disable the \c StatusMonitorProducer.
     */
    DDSPIPE_CORE_DllAPI
    void disable() override;

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
     * @brief Remove all consumers.
     */
    DDSPIPE_CORE_DllAPI
    void clear_consumers() override;

    /**
     * @brief Produce and consume the \c MonitoringStatus.
     *
     * Produces a \c MonitoringStatus with the data gathered and consumes it.
     */
    DDSPIPE_CORE_DllAPI
    void produce_and_consume() override;

    /**
     * @brief Produce the \c MonitoringStatus.
     *
     * Generates a \c MonitoringStatus with the data gathered by the producer.
     */
    DDSPIPE_CORE_DllAPI
    void produce() override;

    /**
     * @brief Consume the \c MonitoringStatus.
     *
     * Calls the consume method of its consumers.
     */
    DDSPIPE_CORE_DllAPI
    void consume() override;

    ///////////////////
    // Data methods ///
    ///////////////////

    /**
     * @brief Clear the data gathered.
     */
    DDSPIPE_CORE_DllAPI
    void clear_data() override;

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

    // Produce data_.
    void produce_nts_();

    // Consume data_.
    void consume_nts_();

    // Instance of the StatusMonitorProducer.
    static std::unique_ptr<StatusMonitorProducer> instance_;

    // Mutex to protect the StatusMonitorProducer.
    DDSPIPE_CORE_DllAPI
    static std::mutex mutex_;

    // The produced data.
    MonitoringStatus data_;

    // Errors gathered by the producer.
    MonitoringErrorStatus error_status_;

    // Whether the producer has gathered any errors.
    bool has_errors_ = false;

    // Vector of consumers of the MonitoringStatus.
    std::vector<std::unique_ptr<IMonitorConsumer<MonitoringStatus>>> consumers_;
};


// The names of variables inside macros must be unique to avoid conflicts with external variables
#define MONITOR_ERROR_IMPL_(error) eprosima::ddspipe::core::StatusMonitorProducer::get_instance()->add_error_to_status( \
        error)

} // namespace core
} // namespace ddspipe
} // namespace eprosima

namespace std {

std::ostream& operator <<(
        std::ostream& os,
        const MonitoringStatus& data);

} // namespace std
