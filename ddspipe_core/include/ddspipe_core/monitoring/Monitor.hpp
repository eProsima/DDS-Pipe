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

#include <cpp_utils/event/PeriodicEventHandler.hpp>

#include <ddspipe_core/configuration/MonitorConfiguration.hpp>
#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/monitoring/consumers/DdsMonitorParticipantRegistry.hpp>
#include <ddspipe_core/monitoring/producers/IMonitorProducer.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * The \c Monitor is the entity that reports internal data of the \c DdsPipe.
 * It manages the \c MonitorProducer instances to produce and consume their data.
 */
class Monitor
{
public:

    /**
     * Construct a \c Monitor.
     *
     * @param configuration The \c MonitorConfiguration to initialize the \c Monitor.
     */
    DDSPIPE_CORE_DllAPI
    Monitor(
            const MonitorConfiguration& configuration);

    /**
     * Destruct the \c Monitor.
     */
    DDSPIPE_CORE_DllAPI
    virtual ~Monitor();

    /**
     * @brief Monitorize the DdsPipe status.
     *
     * The DdsPipe's status is monitored by the \c StatusMonitorProducer, which produces the \c MonitoringStatus.
     */
    DDSPIPE_CORE_DllAPI
    virtual void monitor_status();

    /**
     * Monitorize the topics.
     */
    DDSPIPE_CORE_DllAPI
    virtual void monitor_topics();

protected:

    /**
     * Create a \c PeriodicEventHandler to consume monitored data every period.
     *
     * @param producer The \c producer to register.
     */
    DDSPIPE_CORE_DllAPI
    void register_producer_(
            IMonitorProducer* producer);

    // Configuration of the Monitor.
    MonitorConfiguration configuration_;

    // Producers that produce monitored data.
    std::vector<IMonitorProducer*> producers_;

    // Events that trigger the consumption of monitored data every period.
    std::vector<std::unique_ptr<utils::event::PeriodicEventHandler>> events_;

    // Registry of the DomainParticipants used by the DdsMonitorConsumer.
    DdsMonitorParticipantRegistry registry_;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima
