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

#include <ddspipe_core/library/library_dll.h>


namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * @brief Interface that represents a generic producer of the \c Monitor.
 *
 * The Monitor's producers are the entities that produce and consume the monitored data.
 * They use their consumes to consume the data.
 *
 * @tparam T Type of the data that the producer is going to produce.
 */
class IMonitorProducer
{
public:

    /**
     * @brief Construct the \c IMonitorProducer.
     */
    DDSPIPE_CORE_DllAPI
    IMonitorProducer() = default;

    /**
     * @brief Destroy the \c IMonitorProducer.
     */
    DDSPIPE_CORE_DllAPI
    virtual ~IMonitorProducer() = default;

    /**
     * @brief Virtual method to enable the producer.
     */
    DDSPIPE_CORE_DllAPI
    virtual void enable() = 0;

    /**
     * @brief Virtual method to disable the producer.
     */
    DDSPIPE_CORE_DllAPI
    virtual void disable() = 0;

    /**
     * @brief Virtual method to clear the consumers.
     */
    DDSPIPE_CORE_DllAPI
    virtual void clear_consumers() = 0;

    /**
     * @brief Virtual method to clear the data gathered by the producer.
     */
    DDSPIPE_CORE_DllAPI
    virtual void clear_data() = 0;

    /**
     * @brief Virtual method to produce and consume data.
     */
    DDSPIPE_CORE_DllAPI
    virtual void produce_and_consume() = 0;

    /**
     * @brief Virtual method to produce data.
     */
    DDSPIPE_CORE_DllAPI
    virtual void produce() = 0;

    /**
     * @brief Virtual method to consume data.
     */
    DDSPIPE_CORE_DllAPI
    virtual void consume() = 0;

    /**
     * @brief Attribute that represents the period at which the producer should consume data.
     */
    double period = 1000;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima
