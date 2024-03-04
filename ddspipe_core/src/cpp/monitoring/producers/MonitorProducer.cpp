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

#include <ddspipe_core/monitoring/producers/MonitorProducer.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

void MonitorProducer::init(
        const MonitorProducerConfiguration& configuration)
{
    // Store whether the producer is enabled
    enabled_ = configuration.enabled;

    if (!enabled_)
    {
        // Don't register the consumers if the producer is not enabled
        return;
    }

    // Store the period so it can be used by the Monitor
    period = configuration.period;
}

} //namespace core
} //namespace ddspipe
} //namespace eprosima
