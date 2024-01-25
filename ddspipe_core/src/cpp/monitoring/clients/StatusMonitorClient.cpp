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


#include <ddspipe_core/monitoring/clients/StatusMonitorClient.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {

IMonitorClient* StatusMonitorClient::get_instance()
{
    return &get_reference();
}

StatusMonitorClient& StatusMonitorClient::get_reference()
{
    static StatusMonitorClient instance;
    return instance;
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

IMonitorData* StatusMonitorClient::save_data() const
{
    // Take the lock to prevent saving the data while it's changing
    std::lock_guard<std::mutex> lock(status_mutex_);

    // Create the IMonitorData object
    MonitorStatus* status = new MonitorStatus();
    status->data = status_data_;

    return status;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
