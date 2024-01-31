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


#include <memory>
#include <mutex>

#include <fastrtps/rtps/resources/ResourceEvent.h>

#include <fastdds/rtps/resources/TimedEvent.h>

#include <ddspipe_core/monitoring/clients/IMonitorClient.hpp>


// Monitoring API:

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
class Monitor
{
public:

    // TODO
    Monitor();

    // TODO
    ~Monitor();

    // TODO
    void register_client(
            IMonitorClient* client);

    // TODO
    void clear_clients();

protected:

    // TODO
    std::mutex mutex_;

    // TODO
    fastrtps::rtps::ResourceEvent event_handler_;

    // TODO
    std::vector<std::unique_ptr<fastrtps::rtps::TimedEvent>> events_;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima
