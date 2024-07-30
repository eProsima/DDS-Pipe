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

#pragma once

#include <fastdds/rtps/builtin/data/PublicationBuiltinTopicData.hpp>
#include <fastdds/rtps/builtin/data/SubscriptionBuiltinTopicData.hpp>
#include <fastdds/rtps/common/InstanceHandle.hpp>

#include <ddspipe_core/dynamic/DiscoveryDatabase.hpp>
#include <ddspipe_core/types/dds/Endpoint.hpp>

#include <ddspipe_participants/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace detail {

/**
 * @brief Checks if the writer/reader is keyed based on its \c GUID_t .
 */
bool is_keyed_(
        const eprosima::fastdds::rtps::GUID_t& guid);

/**
 * @brief Common part to create any endpoint from info object
 */
template<class DiscoveryBuiltinTopicData>
core::types::Endpoint create_common_endpoint_from_info_(
        const DiscoveryBuiltinTopicData& info,
        const core::types::ParticipantId participant_discoverer_id);

/**
 * @brief Create an endpoint from info object
 *
 * Specialized for \c PublicationBuiltinTopicData and \c SubscriptionBuiltinTopicData .
 */
template<class DiscoveryBuiltinTopicData>
core::types::Endpoint create_endpoint_from_info_(
        const DiscoveryBuiltinTopicData& info,
        const core::types::ParticipantId participant_discoverer_id);

/**
 * @brief Get the QoS from a Writer from the \c DiscoveryDatabase .
 */
core::types::SpecificEndpointQoS specific_qos_of_writer_(
        const core::DiscoveryDatabase& database,
        const core::types::Guid& guid);

bool come_from_same_participant_(
        const fastdds::rtps::GUID_t src_guid,
        const fastdds::rtps::GUID_t target_guid) noexcept;

fastdds::rtps::GUID_t guid_from_instance_handle(
        const fastdds::rtps::InstanceHandle_t& ihandle) noexcept;

} /* namespace detail */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
