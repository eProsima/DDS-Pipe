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
#include <ddspipe_core/monitoring/clients/StatusMonitorClient.hpp>
#include <ddspipe_core/monitoring/consumers/DdsMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/consumers/StdoutMonitorConsumer.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {

StatusMonitorClient* StatusMonitorClient::get_instance()
{
    static StatusMonitorClient instance;
    return &instance;
}

void StatusMonitorClient::init(const MonitorStatusConfiguration* configuration)
{
    // Store the period so it can be used by the Monitor
    period = configuration->period;

    // Register the type
    fastdds::dds::TypeSupport type(new MonitoringStatusPubSubType());

    // Create the consumers
    consumers_.push_back(new DdsMonitorConsumer<MonitoringStatus>(configuration, type));
    consumers_.push_back(new StdoutMonitorConsumer<MonitoringStatus>(configuration));
}

void StatusMonitorClient::consume() const
{
    const MonitoringStatus data = save_data_();

    for (auto consumer : consumers_)
    {
        consumer->consume(&data);
    }
}

void StatusMonitorClient::add_error_to_status(
        const MonitorStatusError& error)
{
    // Take the lock to prevent:
    //      1. Changing the data while it's being saved.
    //      2. Simultaneous calls to add_error_to_status.
    std::lock_guard<std::mutex> lock(status_mutex_);

    auto error_status = status_data_.error_status();

    switch (error)
    {
        case MonitorStatusError::MCAP_FILE_CREATION_FAILURE:
            error_status.mcap_file_creation_failure(true);
            break;

        case MonitorStatusError::DISK_FULL:
            error_status.disk_full(true);
            break;

        case MonitorStatusError::TYPE_MISMATCH:
            error_status.type_mismatch(true);
            break;

        case MonitorStatusError::QOS_MISMATCH:
            error_status.qos_mismatch(true);
            break;
    }

    status_data_.error_status(error_status);
    status_data_.has_errors(true);
}

StatusMonitorClient::StatusMonitorClient()
{
}

StatusMonitorClient::~StatusMonitorClient()
{
    consumers_.clear();
}

MonitoringStatus StatusMonitorClient::save_data_() const
{
    // Take the lock to prevent saving the data while it's changing
    std::lock_guard<std::mutex> lock(status_mutex_);

    return status_data_;
}

std::ostream& operator<<(std::ostream& os, const MonitoringStatus& data) {
    os << "Monitoring Status: [";

    bool is_first_error = true;

    auto print_error = [&](const MonitorStatusError& error)
    {
        if (!is_first_error)
        {
            os << ", ";
        }

        os << error;
        is_first_error = false;
    };

    const auto& status = data.error_status();

    if (status.mcap_file_creation_failure())
    {
        print_error(MonitorStatusError::MCAP_FILE_CREATION_FAILURE);
    }

    if (status.disk_full())
    {
        print_error(MonitorStatusError::DISK_FULL);
    }

    if (status.type_mismatch())
    {
        print_error(MonitorStatusError::TYPE_MISMATCH);
    }

    if (status.qos_mismatch())
    {
        print_error(MonitorStatusError::QOS_MISMATCH);
    }

    os << "]";
    return os;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
