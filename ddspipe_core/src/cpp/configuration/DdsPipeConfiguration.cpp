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
 * @file DdsPipeConfiguration.cpp
 *
 */

#include <cpp_utils/Formatter.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/configuration/DdsPipeConfiguration.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

bool DdsPipeConfiguration::is_valid(
        utils::Formatter& error_msg) const noexcept
{
    if (remove_unused_entities && discovery_trigger != DiscoveryTrigger::READER)
    {
        error_msg << "A discovery-trigger different from reader is incompatible with remove-unused-entities.";
        return false;
    }

    return routes.is_valid(error_msg) && topic_routes.is_valid(error_msg);
}

bool DdsPipeConfiguration::is_valid(
        utils::Formatter& error_msg,
        const std::map<types::ParticipantId, bool>& participant_ids) const noexcept
{
    if (!is_valid(error_msg))
    {
        return false;
    }

    return routes.is_valid(error_msg, participant_ids) &&
           topic_routes.is_valid(error_msg, participant_ids);
}

RoutesConfiguration DdsPipeConfiguration::get_routes_config(
        const utils::Heritable<types::DistributedTopic>& topic) const noexcept
{
    if (topic_routes().count(topic) != 0)
    {
        // There is a topic route configuration. Use it, and ignore the generic one.
        return topic_routes()[topic];
    }
    else
    {
        // There isn't a topic route configuration. Use the generic one.
        return routes;
    }
}

std::vector<core::types::ManualTopic> DdsPipeConfiguration::get_manual_topics(
        const core::ITopic& topic) const noexcept
{
    // Filter the manual topics to only return the ones that match with the given topic.
    std::vector<core::types::ManualTopic> matching_manual_topics{};

    for (const auto& manual_topic : manual_topics)
    {
        if (manual_topic.first->matches(topic))
        {
            matching_manual_topics.push_back(manual_topic);
        }
    }

    return matching_manual_topics;
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
