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
    TopicQoS qos;

    if (seed % 2)
    {
        qos.reliability_qos = ReliabilityKind::BEST_EFFORT;
    }
    else
    {
        qos.reliability_qos = ReliabilityKind::RELIABLE;
    }

    seed /= 2;

    switch ((seed / 2) % 4)
    {
        case 0:
            qos.durability_qos = DurabilityKind::VOLATILE;
            break;

        case 1:
            qos.durability_qos = DurabilityKind::TRANSIENT_LOCAL;
            break;

        case 2:
            qos.durability_qos = DurabilityKind::TRANSIENT;
            break;

        case 3:
            qos.durability_qos = DurabilityKind::PERSISTENT;
            break;

        default:
            break;
    }

    seed /= 4;

    if (seed % 2)
    {
        qos.ownership_qos = OwnershipQosPolicyKind::SHARED_OWNERSHIP_QOS;
    }
    else
    {
        qos.ownership_qos = OwnershipQosPolicyKind::EXCLUSIVE_OWNERSHIP_QOS;
    }

    seed /= 2;

    qos.use_partitions = ((seed % 2) == 0);

    seed /= 2;

    qos.keyed = ((seed % 2) == 0);

    seed /= 2;

    qos.downsampling = (seed % 2) + 1;

    seed /= 2;

    qos.max_reception_rate = seed % 2;

    seed /= 2;

    qos.history_depth = seed;

    return qos;
}

template <>
DdsTopic arbitrary(
        unsigned int seed /* = 0 */)
{
    DdsTopic topic;
    topic.m_topic_name = "TopicName_" + std::to_string(seed);
    topic.type_name = "TopicType_" + std::to_string(seed);
    topic.topic_qos = arbitrary<TopicQoS>(seed);
    return topic;
}

template <>
WildcardDdsFilterTopic arbitrary(
        unsigned int seed /* = 0 */)
{
    WildcardDdsFilterTopic topic;

    if (seed % 2)
    {
        topic.topic_name.set_value("TopicName_" + std::to_string(seed));
    }

    seed /= 2;

    if (seed % 2)
    {
        topic.type_name.set_value("TopicName_" + std::to_string(seed));
    }

    return topic;
}

////////////////////////////////////////////////
// SPECIFIC ARBITRARY
////////////////////////////////////////////////

template <>
TopicQoS arbitrary(
        unsigned int seed,
        bool limit_durability)
{
    TopicQoS qos = arbitrary<TopicQoS>(seed);

    // Reliability
    seed /= 2;

    if (seed % 2)
    {
        qos.durability_qos = DurabilityKind::VOLATILE;
    }
    else
    {
        qos.durability_qos = DurabilityKind::TRANSIENT_LOCAL;
    }

    return qos;
}

} /* namespace testing */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
