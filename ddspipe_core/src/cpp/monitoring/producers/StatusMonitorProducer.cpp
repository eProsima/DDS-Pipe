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

#include <cpp_utils/Log.hpp>

#include <ddspipe_core/monitoring/producers/StatusMonitorProducer.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {


std::mutex StatusMonitorProducer::mutex_;
std::unique_ptr<StatusMonitorProducer> StatusMonitorProducer::instance_ = nullptr;


void StatusMonitorProducer::init_instance(
        std::unique_ptr<StatusMonitorProducer> instance)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (instance_ != nullptr)
    {
        logWarning(DDSPIPE_MONITOR, "MONITOR | StatusMonitorProducer instance is already initialized.");
        return;
    }

    instance_ = std::move(instance);
}

StatusMonitorProducer* StatusMonitorProducer::get_instance()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (instance_ == nullptr)
    {
        instance_ = std::make_unique<StatusMonitorProducer>();
    }

    return instance_.get();
}

void StatusMonitorProducer::enable()
{
    std::lock_guard<std::mutex> lock(mutex_);

    logInfo(DDSPIPE_MONITOR, "MONITOR | Enabling StatusMonitorProducer.");

    enabled_ = true;
}

void StatusMonitorProducer::disable()
{
    std::lock_guard<std::mutex> lock(mutex_);

    logInfo(DDSPIPE_MONITOR, "MONITOR | Disabling StatusMonitorProducer.");

    enabled_ = false;
}

void StatusMonitorProducer::register_consumer(
        std::unique_ptr<IMonitorConsumer<MonitoringStatus>> consumer)
{
    if (!enabled_)
    {
        logWarning(DDSPIPE_MONITOR,
                "MONITOR | Not registering consumer " << consumer->get_name() << " on StatusMonitorProducer"
                "since the StatusMonitorProducer is disabled.");

        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    logInfo(DDSPIPE_MONITOR, "MONITOR | Registering consumer " << consumer->get_name() << " on StatusMonitorProducer.");

    consumers_.push_back(std::move(consumer));
}

void StatusMonitorProducer::clear_consumers()
{
    std::lock_guard<std::mutex> lock(mutex_);

    logInfo(DDSPIPE_MONITOR, "MONITOR | Removing consumers from StatusMonitorProducer.");

    consumers_.clear();
}

void StatusMonitorProducer::produce_and_consume()
{
    if (!enabled_)
    {
        // Don't produce and consume if the producer is not enabled
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    produce_nts_();
    consume_nts_();
}

void StatusMonitorProducer::produce()
{
    if (!enabled_)
    {
        // Don't produce if the producer is not enabled
        return;
    }

    // Take the lock to prevent:
    //      1. Changing the data while it's being saved.
    //      2. Simultaneous calls to save_data_.
    std::lock_guard<std::mutex> lock(mutex_);

    produce_nts_();
}

void StatusMonitorProducer::consume()
{
    if (!enabled_)
    {
        // Don't consume if the producer is not enabled
        return;
    }

    // Take the lock to prevent:
    //      1. Changing the data while it's being consumed.
    //      2. Simultaneous calls to save_data_.
    std::lock_guard<std::mutex> lock(mutex_);

    consume_nts_();
}

void StatusMonitorProducer::clear_data()
{
    std::lock_guard<std::mutex> lock(mutex_);

    logInfo(DDSPIPE_MONITOR, "MONITOR | Clearing StatusMonitorProducer data.");

    error_status_.type_mismatch(false);
    error_status_.qos_mismatch(false);
    has_errors_ = false;

    data_.error_status(error_status_);
    data_.has_errors(has_errors_);
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

    logInfo(DDSPIPE_MONITOR, "MONITOR | Adding error " << error << " to status.");

    if (error == "TYPE_MISMATCH")
    {
        error_status_.type_mismatch(true);
    }
    else if (error == "QOS_MISMATCH")
    {
        error_status_.qos_mismatch(true);
    }

    has_errors_ = true;
}

void StatusMonitorProducer::produce_nts_()
{
    logInfo(DDSPIPE_MONITOR, "MONITOR | Producing MonitoringStatus.");

    data_.error_status(error_status_);
    data_.has_errors(has_errors_);
}

void StatusMonitorProducer::consume_nts_()
{
    logInfo(DDSPIPE_MONITOR, "MONITOR | Consuming MonitoringStatus.")

    for (auto& consumer : consumers_)
    {
        consumer->consume(data_);
    }
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima

namespace std {

std::ostream& operator <<(
        std::ostream& os,
        const MonitoringStatus& data)
{
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

} // namespace std
