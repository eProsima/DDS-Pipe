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

#include <ddspipe_core/configuration/MonitorConfiguration.hpp>
#include <ddspipe_core/types/monitoring/status/MonitoringStatus.h>
#include <ddspipe_core/types/monitoring/topics/MonitoringData.h>
#include <ddspipe_core/monitoring/IMonitorConsumer.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
class StdoutMonitorConsumer : public IMonitorConsumer
{
public:

    // TODO
    StdoutMonitorConsumer(
            const MonitorConfiguration& configuration);

    // TODO
    virtual void consume(const MonitoringStatus& data) const override;

    // TODO
    virtual void consume(const MonitoringData& data) const override;
};

std::ostream& operator<<(std::ostream& os, const MonitoringStatus& data);
std::ostream& operator<<(std::ostream& os, const DdsTopicData& data);
std::ostream& operator<<(std::ostream& os, const DdsTopic& topic);
std::ostream& operator<<(std::ostream& os, const MonitoringData& data);

} // namespace core
} // namespace ddspipe
} // namespace eprosima
