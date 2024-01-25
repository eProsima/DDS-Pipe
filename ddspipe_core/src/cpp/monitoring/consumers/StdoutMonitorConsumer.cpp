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


#include <iostream>

#include <ddspipe_core/monitoring/MonitorStatusError.hpp>
#include <ddspipe_core/monitoring/clients/StatusMonitorClient.hpp>
#include <ddspipe_core/monitoring/clients/TopicsMonitorClient.hpp>
#include <ddspipe_core/monitoring/consumers/StdoutMonitorConsumer.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

StdoutMonitorConsumer::StdoutMonitorConsumer(const MonitorConfiguration& configuration)
{
    // TODO
}

void StdoutMonitorConsumer::consume(IMonitorData* data) const
{
    MonitorStatus* status = dynamic_cast<MonitorStatus*>(data);

    if (status != nullptr)
    {
        std::cout << status->data << std::endl;
    }

    MonitorTopics* topics = dynamic_cast<MonitorTopics*>(data);

    if (topics != nullptr)
    {
        std::cout << topics->data << std::endl;
    }
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

std::ostream& operator<<(std::ostream& os, const DdsTopicData& data) {
    os << "Participant ID: " << data.participant_id() << ", Messages Received: " << data.msgs_received() << ", Frequency: " << data.frequency();
    return os;
}

std::ostream& operator<<(std::ostream& os, const DdsTopic& topic) {
    os << "Topic Name: " << topic.name() << ", Type: " << topic.data_type_name() << ", Data: [";

    for (const auto& data : topic.data()) {
        os << data << "; ";
    }

    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const MonitoringTopics& data) {
    os << "Monitoring Data: [";

    for (const auto& topic : data.topics()) {
        os << topic << "; ";
    }

    os << "]";
    return os;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
