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

#include <ddspipe_core/configuration/MonitorProducerConfiguration.hpp>
#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/monitoring/producers/IMonitorProducer.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * @brief Class that represents a generic producer of the \c Monitor.
 *
 * The Monitor's producers are the entities that produce and consume the monitored data.
 * They use their consumes to consume the data.
 *
 * @tparam T Type of the data that the producer is going to produce.
 */
class MonitorProducer : public IMonitorProducer
{
public:

    /**
     * @brief Destroy the \c MonitorProducer.
     */
    virtual ~MonitorProducer() = default;

    /**
     * @brief Virtual method that initializes a producer with a given configuration.
     *
     * @param configuration Configuration to initialize the producer with.
     */
    DDSPIPE_CORE_DllAPI
    virtual void init(
            const MonitorProducerConfiguration& configuration);

protected:

    // Whether the producer should produce and consume data.
    bool enabled_ = false;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima
