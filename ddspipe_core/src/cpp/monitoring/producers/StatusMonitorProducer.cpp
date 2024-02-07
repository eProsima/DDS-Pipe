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


#include <ddspipe_core/configuration/MonitorStatusConfiguration.hpp>
#include <ddspipe_core/monitoring/consumers/DdsMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/consumers/StdoutMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/producers/StatusMonitorProducer.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {


StatusMonitorProducer* StatusMonitorProducer::instance_ = nullptr;


void StatusMonitorProducer::init_instance(StatusMonitorProducer* instance)
{
    instance_ = instance;
}

StatusMonitorProducer* StatusMonitorProducer::get_instance()
{
    if (instance_ == nullptr)
    {
        instance_ = new StatusMonitorProducer();
    }

    return instance_;
}

void StatusMonitorProducer::init(const MonitorStatusConfiguration* configuration)
{
    // Store whether the producer is enabled
    enabled_ = configuration->enabled;

    if (!enabled_)
    {
        // Don't register the consumers if the producer is not enabled
        return;
    }

    // Store the period so it can be used by the Monitor
    period = configuration->period;

    // Register the type
    fastdds::dds::TypeSupport type(new MonitoringStatusPubSubType());

    // Create the consumers
    consumers_.push_back(new DdsMonitorConsumer<MonitoringStatus>(configuration, type));
    consumers_.push_back(new StdoutMonitorConsumer<MonitoringStatus>(configuration));
}

void StatusMonitorProducer::consume()
{
    if (!enabled_)
    {
        // Don't consume if the producer is not enabled
        return;
    }

    const auto data = save_data_();

    for (auto consumer : consumers_)
    {
        consumer->consume(data);
    }
}

void StatusMonitorProducer::add_error_to_status(
        const std::string& error)
{
    if (!enabled_)
    {
        // Don't save the data if the producer is not enabled
        return;
    }

    // Take the lock to prevent:
    //      1. Changing the data while it's being saved.
    //      2. Simultaneous calls to add_error_to_status.
    std::lock_guard<std::mutex> lock(mutex_);

    auto error_status = data_->error_status();

    if (error == "TYPE_MISMATCH")
    {
        error_status.type_mismatch(true);
    }
    else if (error == "QOS_MISMATCH")
    {
        error_status.qos_mismatch(true);
    }

    data_->error_status(error_status);
    data_->has_errors(true);
}

StatusMonitorProducer::~StatusMonitorProducer()
{
    consumers_.clear();
}

MonitoringStatus* StatusMonitorProducer::save_data_() const
{
    return data_;
}

std::ostream& operator<<(std::ostream& os, const MonitoringStatus& data) {
    os << "Monitoring Status: [";

    bool is_first_error = true;

    auto print_error = [&](const std::string& error)
    {
        if (!is_first_error)
        {
            os << ", ";
        }

        os << error;
        is_first_error = false;
    };

    auto error = data.error_status();

    if (error.type_mismatch())
    {
        print_error("TYPE_MISMATCH");
    }

    if (error.qos_mismatch())
    {
        print_error("QOS_MISMATCH");
    }

    os << "]";
    return os;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
