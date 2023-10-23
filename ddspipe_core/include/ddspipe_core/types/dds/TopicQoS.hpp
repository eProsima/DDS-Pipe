// Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
// limitations under the License\.

#pragma once

#include <cpp_utils/types/Fuzzy.hpp>

#include <fastdds/dds/core/policy/QosPolicies.hpp>
#include <fastdds/rtps/common/Types.h>

#include <ddspipe_core/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

//! Durability kind enumeration
using DurabilityKind = eprosima::fastrtps::rtps::DurabilityKind_t;

//! Reliability kind enumeration
using ReliabilityKind = eprosima::fastrtps::rtps::ReliabilityKind_t;

//! History kind enumeration
using HistoryDepthType = unsigned int;

//! Ownership configuration
using OwnershipQosPolicyKind = eprosima::fastdds::dds::OwnershipQosPolicyKind;

/**
 * Collection of QoS related with a Topic.
 *
 * @warning partitions are considered a Topic QoS. A Topic can then only either have partitions or not have them, but it
 * cannot support empty partitions.
 *
 * @todo add keys to Topic QoS
 */
struct
TopicQoS
{
    /////////////////////////
    // CONSTRUCTORS
    /////////////////////////

    //! Default TopicQoS with reader less restrictive parameters
    DDSPIPE_CORE_DllAPI
    TopicQoS();

    /////////////////////////
    // OPERATORS
    /////////////////////////

    //! Equality operator
    DDSPIPE_CORE_DllAPI
    bool operator ==(
            const TopicQoS& other) const noexcept;

    /////////////////////////
    // AUXILIARY METHODS
    /////////////////////////

    //! Whether the Topic is RELIABLE, not BEST_EFFORT
    DDSPIPE_CORE_DllAPI
    bool is_reliable() const noexcept;

    //! Whether the Topic is TRANSIENT_LOCAL, not VOLATILE
    DDSPIPE_CORE_DllAPI
    bool is_transient_local() const noexcept;

    //! Whether the Topic has EXCLUSIVE_OWNERSHIP, not SHARED_OWNERSHIP
    DDSPIPE_CORE_DllAPI
    bool has_ownership() const noexcept;

    //! Whether the Topic has partitions, not empty partition
    DDSPIPE_CORE_DllAPI
    bool has_partitions() const noexcept;

    /**
     * @brief Set the Topic QoSs that have not been set and are set in \c qos .
     *
     * @note A Topic QoS is considered as set when it has a FuzzyLevel higher than DEFAULT.
     */
    DDSPIPE_CORE_DllAPI
    void set_qos(
            const TopicQoS& qos,
            const utils::FuzzyLevelValues& fuzzy_level = utils::FuzzyLevelValues::fuzzy_level_fuzzy) noexcept;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    //! Durability kind
    utils::Fuzzy<DurabilityKind> durability_qos{DEFAULT_DURABILITY_QOS, utils::FuzzyLevelValues::fuzzy_level_default};

    //! Reliability kind
    utils::Fuzzy<ReliabilityKind> reliability_qos{DEFAULT_RELIABILITY_QOS,
                                                  utils::FuzzyLevelValues::fuzzy_level_default};

    //! Ownership kind of the topic
    utils::Fuzzy<OwnershipQosPolicyKind> ownership_qos{DEFAULT_OWNERSHIP_QOS,
                                                       utils::FuzzyLevelValues::fuzzy_level_default};

    //! Whether the topics uses partitions
    utils::Fuzzy<bool> use_partitions{DEFAULT_USE_PARTITIONS, utils::FuzzyLevelValues::fuzzy_level_default};

    //! Whether the topic has key or not
    utils::Fuzzy<bool> keyed{DEFAULT_KEYED, utils::FuzzyLevelValues::fuzzy_level_default};

    //! Depth of the history
    utils::Fuzzy<HistoryDepthType> history_depth{DEFAULT_KEYED, utils::FuzzyLevelValues::fuzzy_level_default};

    //! Discard msgs if less than 1/rate seconds elapsed since the last sample was transmitted [Hz]. Default: 0 (no limit)
    utils::Fuzzy<float> max_tx_rate{DEFAULT_MAX_TX_RATE, utils::FuzzyLevelValues::fuzzy_level_default};

    //! Discard msgs if less than 1/rate seconds elapsed since the last sample was processed [Hz]. Default: 0 (no limit)
    utils::Fuzzy<float> max_rx_rate{DEFAULT_MAX_RX_RATE, utils::FuzzyLevelValues::fuzzy_level_default};

    //! Downsampling factor: keep 1 out of every *downsampling* samples received (downsampling=1 <=> no downsampling)
    utils::Fuzzy<unsigned int> downsampling{DEFAULT_DOWNSAMPLING, utils::FuzzyLevelValues::fuzzy_level_default};

    /////////////////////////
    // GLOBAL VARIABLES
    /////////////////////////

    //! Global value to store the default Topic QoS in this execution.
    DDSPIPE_CORE_DllAPI
    static utils::Fuzzy<TopicQoS> default_topic_qos;

    /////////////////////////
    // DEFAULT VALUES
    /////////////////////////

    //! Durability kind (Default = VOLATILE)
    DDSPIPE_CORE_DllAPI
    static constexpr const DurabilityKind DEFAULT_DURABILITY_QOS = DurabilityKind::VOLATILE;

    //! Reliability kind (Default = BEST_EFFORT)
    DDSPIPE_CORE_DllAPI
    static constexpr const ReliabilityKind DEFAULT_RELIABILITY_QOS = ReliabilityKind::BEST_EFFORT;

    //! Ownership kind (Default = SHARED_OWNERSHIP)
    DDSPIPE_CORE_DllAPI
    static constexpr const OwnershipQosPolicyKind DEFAULT_OWNERSHIP_QOS = OwnershipQosPolicyKind::SHARED_OWNERSHIP_QOS;

    //! Whether the topic uses partitions (Default = False)
    DDSPIPE_CORE_DllAPI
    static constexpr const bool DEFAULT_USE_PARTITIONS = false;

    //! History depth (Default = 5000)
    DDSPIPE_CORE_DllAPI
    static constexpr const HistoryDepthType DEFAULT_HISTORY_DEPTH = 5000;

    //! Whether the topic has a key (Default = False)
    DDSPIPE_CORE_DllAPI
    static constexpr const bool DEFAULT_KEYED = false;

    //! Max Tx Rate (Default = 0)
    DDSPIPE_CORE_DllAPI
    static constexpr const float DEFAULT_MAX_TX_RATE = 0;

    //! Max Rx Rate (Default = 0)
    DDSPIPE_CORE_DllAPI
    static constexpr const float DEFAULT_MAX_RX_RATE = 0;

    //! Downsampling (Default = 1)
    DDSPIPE_CORE_DllAPI
    static constexpr const unsigned int DEFAULT_DOWNSAMPLING = 1;
};

/**
 * @brief \c DurabilityKind to stream serialization
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const DurabilityKind& kind);

/**
 * @brief \c ReliabilityKind to stream serialization
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const ReliabilityKind& kind);

/**
 * @brief \c OwnershipQosPolicyKind to stream serialization
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const OwnershipQosPolicyKind& qos);

/**
 * @brief \c TopicQoS to stream serialization
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const TopicQoS& qos);

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
