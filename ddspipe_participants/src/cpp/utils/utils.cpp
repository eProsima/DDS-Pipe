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

#include <fastdds/rtps/builtin/data/PublicationBuiltinTopicData.hpp>
#include <fastdds/rtps/builtin/data/SubscriptionBuiltinTopicData.hpp>

#include <cpp_utils/utils.hpp>

#include <ddspipe_core/types/data/RtpsPayloadData.hpp>

#include <ddspipe_participants/library/library_dll.h>

#include <utils/utils.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace detail {

bool is_keyed_(
        const eprosima::fastdds::rtps::GUID_t& guid)
{
    const eprosima::fastdds::rtps::octet identifier = guid.entityId.value[3];

    // - For writers: NO_KEY = 0x03, WITH_KEY = 0x02
    // - For readers: NO_KEY = 0x04, WITH_KEY = 0x07
    return (identifier & 0x0F) == 0x02 || (identifier & 0x0F) == 0x07;
}

template<class DiscoveryBuiltinTopicData>
core::types::Endpoint create_common_endpoint_from_info_(
        const DiscoveryBuiltinTopicData& info,
        const core::types::ParticipantId participant_discoverer_id)
{
    // Endpoint struct to fill
    core::types::Endpoint endpoint;

    // Parse GUID
    endpoint.guid = info.guid;

    // Parse TopicQoS
    // Durability
    endpoint.topic.topic_qos.durability_qos.set_value(info.durability.durabilityKind());
    // Reliability
    if (info.reliability.kind == fastdds::dds::BEST_EFFORT_RELIABILITY_QOS)
    {
        endpoint.topic.topic_qos.reliability_qos.set_value(fastdds::rtps::BEST_EFFORT);
    }
    else if (info.reliability.kind == fastdds::dds::RELIABLE_RELIABILITY_QOS)
    {
        endpoint.topic.topic_qos.reliability_qos.set_value(fastdds::rtps::RELIABLE);
    }
    else
    {
        utils::tsnh(
            utils::Formatter() <<
                "Invalid ReliabilityQoS value found while parsing DiscoveryInfo for Endpoint creation.");
    }
    // Set Topic with Partitions
    endpoint.topic.topic_qos.use_partitions.set_value(!info.partition.empty());
    // Set Topic with ownership
    endpoint.topic.topic_qos.ownership_qos.set_value(info.ownership.kind);
    // Set Topic key
    endpoint.topic.topic_qos.keyed.set_value(is_keyed_(info.guid));

    // Set TypeIdentifier
    endpoint.topic.type_ids.type_identifier1(info.type_information.type_information.complete().typeid_with_size().type_id());
    endpoint.topic.type_ids.type_identifier2(info.type_information.type_information.minimal().typeid_with_size().type_id());

    // Parse Topic
    core::types::DdsTopic info_topic;
    endpoint.topic.m_topic_name = std::string(info.topic_name);
    endpoint.topic.type_name = std::string(info.type_name);
    endpoint.topic.m_internal_type_discriminator = core::types::INTERNAL_TOPIC_TYPE_RTPS;
    endpoint.topic.m_topic_discoverer = participant_discoverer_id;

    // Parse specific QoS of the entity
    if (endpoint.topic.topic_qos.has_partitions())
    {
        endpoint.specific_qos.partitions = info.partition;
    }

    // Set participant that discovered
    endpoint.discoverer_participant_id = participant_discoverer_id;

    // NOTE: ownership is only for Writer
    return endpoint;
}

template<>
DDSPIPE_PARTICIPANTS_DllAPI
core::types::Endpoint create_endpoint_from_info_<fastdds::rtps::PublicationBuiltinTopicData>(
        const fastdds::rtps::PublicationBuiltinTopicData& info,
        const core::types::ParticipantId participant_discoverer_id)
{
    // Create Endpoint from common info
    core::types::Endpoint endpoint = create_common_endpoint_from_info_(info, participant_discoverer_id);

    if (endpoint.topic.topic_qos.has_ownership())
    {
        // Only for writers
        endpoint.specific_qos.ownership_strength = info.ownership_strength;
    }

    // Set type
    endpoint.kind = core::types::EndpointKind::writer;

    return endpoint;
}

template<>
DDSPIPE_PARTICIPANTS_DllAPI
core::types::Endpoint create_endpoint_from_info_<fastdds::rtps::SubscriptionBuiltinTopicData>(
        const fastdds::rtps::SubscriptionBuiltinTopicData& info,
        const core::types::ParticipantId participant_discoverer_id)
{
    // Create Endpoint from common info
    core::types::Endpoint endpoint = create_common_endpoint_from_info_(info, participant_discoverer_id);

    // Set type
    endpoint.kind = core::types::EndpointKind::reader;

    return endpoint;
}

core::types::SpecificEndpointQoS specific_qos_of_writer_(
        const core::DiscoveryDatabase& database,
        const core::types::Guid& guid)
{
    return database.get_endpoint(guid).specific_qos;
}

bool come_from_same_participant_(
        const fastdds::rtps::GUID_t src_guid,
        const fastdds::rtps::GUID_t target_guid) noexcept
{
    return src_guid.guidPrefix == target_guid.guidPrefix;
}

fastdds::rtps::GUID_t guid_from_instance_handle(
        const fastdds::rtps::InstanceHandle_t& ihandle) noexcept
{
    return fastdds::rtps::iHandle2GUID(ihandle);
}

} /* namespace detail */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
