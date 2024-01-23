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

#include <ddspipe_core/monitoring/StdoutMonitorConsumer.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {

StdoutMonitorConsumer::StdoutMonitorConsumer(const MonitorConfiguration& configuration)
{
    // TODO
}

void StdoutMonitorConsumer::consume(const MonitoringStatus& data) const
{
    std::cout << data << std::endl;
}

void StdoutMonitorConsumer::consume(const MonitoringData& data) const
{
    std::cout << data << std::endl;
}

std::ostream& operator<<(std::ostream& os, const MonitoringStatus& data) {
    os << "Monitoring Status: [";

    // TODO

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

std::ostream& operator<<(std::ostream& os, const MonitoringData& data) {
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
