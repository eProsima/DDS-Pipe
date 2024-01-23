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


namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
class IMonitorConsumer
{
public:

    // TODO
    virtual void consume_status(const MonitoringStatus& data) const = 0;

    // TODO
    virtual void consume_topics(const MonitoringData& data) const = 0;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima
