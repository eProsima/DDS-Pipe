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

/**
 * @file MonitorTopicsConfiguration.cpp
 *
 */

#include <cpp_utils/Formatter.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/configuration/MonitorTopicsConfiguration.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

bool MonitorTopicsConfiguration::is_valid(
        utils::Formatter& error_msg) const noexcept
{
    if (period <= 0)
    {
        error_msg << "Period must be greater than 0.";
        return false;
    }

    return MonitorConfiguration::is_valid(error_msg);
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
