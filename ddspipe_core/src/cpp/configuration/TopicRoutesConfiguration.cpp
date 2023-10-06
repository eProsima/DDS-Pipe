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
 * @file TopicRoutesConfiguration.cpp
 *
 */

#include <cpp_utils/Formatter.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/configuration/TopicRoutesConfiguration.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

bool TopicRoutesConfiguration::is_valid(
        utils::Formatter& error_msg) const noexcept
{
    // NOTE: No duplicated topic routes guaranteed due to nature of data structure (map).

    for (const auto& routes : topic_routes)
    {
        if (!routes.second.is_valid(error_msg))
        {
            return false;
        }
    }

    return true;
}

bool TopicRoutesConfiguration::is_valid(
        utils::Formatter& error_msg,
        const std::map<types::ParticipantId, bool>& participant_ids) const noexcept
{
    if (!is_valid(error_msg))
    {
        return false;
    }

    // Additional checks with participants information
    for (const auto& routes : topic_routes)
    {
        if (!routes.second.is_valid(error_msg, participant_ids))
        {
            return false;
        }
    }
    return true;
}

TopicRoutesConfiguration::TopicRoutesMap TopicRoutesConfiguration::operator ()() const
{
    return topic_routes;
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
