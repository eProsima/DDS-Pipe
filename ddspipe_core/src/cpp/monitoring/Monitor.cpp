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

#include <fastdds/dds/topic/TypeSupport.hpp>

#include <cpp_utils/Log.hpp>

#include <ddspipe_core/monitoring/Monitor.hpp>
#include <ddspipe_core/monitoring/consumers/DdsMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/consumers/LogMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/producers/StatusMonitorProducer.hpp>
#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>

#if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)
    #include <ddspipe_core/types/monitoring/status/v1/MonitoringStatus.h>
    #include <ddspipe_core/types/monitoring/status/v1/MonitoringStatusPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopicsPubSubTypes.h>
#else
    #include <ddspipe_core/types/monitoring/status/v2/MonitoringStatus.h>
    #include <ddspipe_core/types/monitoring/status/v2/MonitoringStatusPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopicsPubSubTypes.h>
#endif // if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)

namespace eprosima {
namespace ddspipe {
namespace core {

Monitor::Monitor(
        const MonitorConfiguration& configuration)
    : configuration_(configuration)
{
}

Monitor::~Monitor()
{
    events_.clear();

    for (auto& producer : producers_)
    {
        producer->disable();
        producer->clear_consumers();
        producer->clear_data();
    }

    producers_.clear();
}

void Monitor::monitor_status()
{
    logInfo(DDSPIPE_MONITOR, "MONITOR | Registering Status Monitor Producer.");

    // Register the Status Monitor Producer
    auto status_producer = ddspipe::core::StatusMonitorProducer::get_instance();
    status_producer->init(configuration_.producers.at(STATUS_MONITOR_PRODUCER_ID));

    // Register the type
    fastdds::dds::TypeSupport type(new MonitoringStatusPubSubType());

    // Register the consumers
    status_producer->register_consumer(std::make_unique<ddspipe::core::LogMonitorConsumer<MonitoringStatus>>());

    if (configuration_.consumers.count(STATUS_MONITOR_PRODUCER_ID) > 0)
    {
        status_producer->register_consumer(std::make_unique<ddspipe::core::DdsMonitorConsumer<MonitoringStatus>>(
                    configuration_.consumers.at(STATUS_MONITOR_PRODUCER_ID), registry_, type));
    }

    register_producer_(status_producer);
}

void Monitor::monitor_topics()
{
    logInfo(DDSPIPE_MONITOR, "MONITOR | Registering Topics Monitor Producer.");

    // Register the Topics Monitor Producer
    auto topics_producer = ddspipe::core::TopicsMonitorProducer::get_instance();
    topics_producer->init(configuration_.producers.at(TOPICS_MONITOR_PRODUCER_ID));

    // Register the type
    fastdds::dds::TypeSupport type(new MonitoringTopicsPubSubType());

    // Register the consumers
    topics_producer->register_consumer(std::make_unique<ddspipe::core::LogMonitorConsumer<MonitoringTopics>>());

    if (configuration_.consumers.count(TOPICS_MONITOR_PRODUCER_ID) > 0)
    {
        topics_producer->register_consumer(std::make_unique<ddspipe::core::DdsMonitorConsumer<MonitoringTopics>>(
                    configuration_.consumers.at(TOPICS_MONITOR_PRODUCER_ID), registry_, type));
    }

    register_producer_(topics_producer);
}

void Monitor::register_producer_(
        IMonitorProducer* producer)
{
    logInfo(DDSPIPE_MONITOR, "MONITOR | Registering producer " << producer << ".");

    producers_.push_back(producer);

    std::function<void()> periodic_callback = [producer]()
            {
                producer->produce_and_consume();
            };

    const auto duration = utils::Duration_ms(static_cast<int>(producer->period));
    events_.emplace_back(std::make_unique<utils::event::PeriodicEventHandler>(periodic_callback, duration));
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
