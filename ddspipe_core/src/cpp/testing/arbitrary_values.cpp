// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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


#include <cpp_utils/exception/InitializationException.hpp>

#include <ddspipe_core/testing/arbitrary_values.hpp>
#include <ddspipe_core/types/dds/DomainId.hpp>
#include <ddspipe_core/types/dds/Endpoint.hpp>
#include <ddspipe_core/types/dds/Guid.hpp>
#include <ddspipe_core/types/dds/GuidPrefix.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace testing {

using namespace eprosima::ddspipe::core::types;

////////////////////////////////////////////////
// DEFAULT ARBITRARY
////////////////////////////////////////////////

template <>
TopicQoS arbitrary(
        unsigned int seed /* = 0 */)
{
    TopicQoS object;

    if (seed % 2)
    {
        object.reliability_qos = ReliabilityKind::BEST_EFFORT;
    }
    else
    {
        object.reliability_qos = ReliabilityKind::RELIABLE;
    }

    seed /= 2;

    switch ((seed / 2) % 4)
    {
        case 0:
            object.durability_qos = DurabilityKind::VOLATILE;
            break;

        case 1:
            object.durability_qos = DurabilityKind::TRANSIENT_LOCAL;
            break;

        case 2:
            object.durability_qos = DurabilityKind::TRANSIENT;
            break;

        case 3:
            object.durability_qos = DurabilityKind::PERSISTENT;
            break;

        default:
            break;
    }

    seed /= 4;

    if (seed % 2)
    {
        object.ownership_qos = OwnershipQosPolicyKind::SHARED_OWNERSHIP_QOS;
    }
    else
    {
        object.ownership_qos = OwnershipQosPolicyKind::EXCLUSIVE_OWNERSHIP_QOS;
    }

    seed /= 2;

    object.use_partitions = ((seed % 2) == 0);

    seed /= 2;

    object.keyed = ((seed % 2) == 0);

    seed /= 2;

    object.downsampling = (seed % 2) + 1;

    seed /= 2;

    object.max_reception_rate = seed % 2;

    seed /= 2;

    object.history_depth = seed + 1;

    return object;
}

template <>
DdsTopic arbitrary(
        unsigned int seed /* = 0 */)
{
    DdsTopic object;
    object.m_topic_name = "TopicName_" + std::to_string(seed);
    object.type_name = "TopicType_" + std::to_string(seed);
    object.topic_qos = arbitrary<TopicQoS>(seed);
    return object;
}

template <>
WildcardDdsFilterTopic arbitrary(
        unsigned int seed /* = 0 */)
{
    WildcardDdsFilterTopic object;

    if (seed % 2)
    {
        object.topic_name.set_value("TopicName_" + std::to_string(seed));
    }

    seed /= 2;

    if (seed % 2)
    {
        object.type_name.set_value("TopicName_" + std::to_string(seed));
    }

    return object;
}

template <>
GuidPrefix arbitrary(
        unsigned int seed /* = 0 */)
{
    // TODO do it more generic and arbitrary
    return GuidPrefix(static_cast<uint32_t>(seed));
}

////////////////////////////////////////////////
// SPECIFIC ARBITRARY
////////////////////////////////////////////////

template <>
TopicQoS arbitrary(
        unsigned int seed,
        bool limit_durability)
{
    TopicQoS object = arbitrary<TopicQoS>(seed);

    // Reliability
    seed /= 2;

    if (seed % 2)
    {
        object.durability_qos = DurabilityKind::VOLATILE;
    }
    else
    {
        object.durability_qos = DurabilityKind::TRANSIENT_LOCAL;
    }

    return object;
}

} /* namespace testing */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
