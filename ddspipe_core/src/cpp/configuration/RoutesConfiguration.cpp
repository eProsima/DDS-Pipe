// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
 * @file RoutesConfiguration.cpp
 *
 */

#include <cpp_utils/Formatter.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/configuration/RoutesConfiguration.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

bool RoutesConfiguration::is_valid(
        utils::Formatter& error_msg) const noexcept
{
    // Always valid:
    // - no duplicates in source and destination guaranteed due to nature of data structures (map and set).
    // - validation of requirements from previous point hence performed on creation.
    // - remaining checks (e.g. whether participants actually exist or source allowed in destinations) require
    //   information not available at this scope. Additional validation method added for this purpose.

    return true;
}

bool RoutesConfiguration::is_valid(
        utils::Formatter& error_msg,
        const std::map<ddspipe::core::types::ParticipantId, bool>& participant_ids) const noexcept
{
    if (!is_valid(error_msg))
    {
        return false;
    }

    // Additional checks with participants information
    for (const auto& route : routes)
    {
        const auto& src_id = route.first;
        const auto& dst_ids = route.second;

        // Check participant with this id exists
        if (participant_ids.count(src_id) == 0)
        {
            error_msg << "Invalid route: participant " << src_id << " is not defined. ";
            return false;
        }

        bool src_in_dst = false;
        bool is_repeater = participant_ids.at(src_id);

        for (const auto& dst_id : dst_ids)
        {
            // Check participant with this id exists
            if (participant_ids.count(dst_id) == 0)
            {
                error_msg << "Invalid route: participant " << dst_id << " is not defined. ";
                return false;
            }
            else if (dst_id == src_id)
            {
                // Destination can only be same as source if repeater
                if (!is_repeater)
                {
                    error_msg << "Invalid route: source found in destination for non-repeater participant " << src_id <<
                        " . ";
                    return false;
                }
                src_in_dst = true;
            }
        }

        if (!src_in_dst && is_repeater)
        {
            // Warn the user repeater routes must be set manually
            logWarning(
                DDSPIPE_ROUTES,
                "Source not found in destinations for repeater participant " << src_id
                                                                             << ". If willing to send data through this participant, add its name to the destinations (dst) list.");
        }
    }
    return true;
}

RoutesConfiguration::RoutesMap RoutesConfiguration::operator ()() const
{
    return routes;
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
