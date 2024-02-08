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
#include <ddspipe_core/monitoring/consumers/IMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/producers/StatusMonitorProducer.hpp>
#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
template <typename T>
class StdoutMonitorConsumer : public IMonitorConsumer<T>
{
public:

    // TODO
    StdoutMonitorConsumer(
            const MonitorConfiguration* /* configuration */);

    // TODO
    void consume(const T* data) const override;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima

#include <ddspipe_core/monitoring/consumers/impl/StdoutMonitorConsumer.ipp>
