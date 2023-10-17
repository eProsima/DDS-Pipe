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
 * The QoS associated with Topic are:
 * - Reliability
 * - Durability
 * - Ownership
 * - Partitions
 * - History Depth (history kind is always KEEP_LAST)
 *
 * @warning partitions are considered as a QoS, thus a Topic can only have partitions, or not have any, but cannot
 * support empty partition and partitions.
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

    //! Whether the Topic has partitions, not empty partition
    DDSPIPE_CORE_DllAPI
    void set_qos(
            const TopicQoS& qos) noexcept;

    /////////////////////////
    // GLOBAL VARIABLES
    /////////////////////////

    /**
     * @brief Global value to store the default durability qos in this execution.
     *
     * This value can change along the execution.
     * Every new TopicQoS object will use this value as \c durability_qos default.
     */
    DDSPIPE_CORE_DllAPI
    static std::atomic<DurabilityKind> default_durability_qos;

    /**
     * @brief Global value to store the default reliability qos in this execution.
     *
     * This value can change along the execution.
     * Every new TopicQoS object will use this value as \c reliability_qos default.
     */
    DDSPIPE_CORE_DllAPI
    static std::atomic<ReliabilityKind> default_reliability_qos;

    /**
     * @brief Global value to store the default ownership qos in this execution.
     *
     * This value can change along the execution.
     * Every new TopicQoS object will use this value as \c ownership_qos default.
     */
    DDSPIPE_CORE_DllAPI
    static std::atomic<OwnershipQosPolicyKind> default_ownership_qos;

    /**
     * @brief Global value to store the default use_partitions in this execution.
     *
     * This value can change along the execution.
     * Every new TopicQoS object will use this value as \c use_partitions default.
     */
    DDSPIPE_CORE_DllAPI
    static std::atomic<bool> default_use_partitions;

    /**
     * @brief Global value to store the default keyed in this execution.
     *
     * This value can change along the execution.
     * Every new TopicQoS object will use this value as \c keyed default.
     */
    DDSPIPE_CORE_DllAPI
    static std::atomic<bool> default_keyed;

    /**
     * @brief Global value to store the default  in this execution.
     *
     * This value can change along the execution.
     * Every new TopicQoS object will use this value as \c history_depth default.
     */
    DDSPIPE_CORE_DllAPI
    static std::atomic<HistoryDepthType> default_history_depth;

    /**
     * @brief Global value to store the default max transmission rate in this execution.
     *
     * This value can change along the execution.
     * Every new TopicQoS object will use this value as \c max_tx_rate default.
     */
    DDSPIPE_CORE_DllAPI
    static std::atomic<float> default_max_tx_rate;

    /**
     * @brief Global value to store the default max reception rate in this execution.
     *
     * This value can change along the execution.
     * Every new TopicQoS object will use this value as \c max_rx_rate default.
     */
    DDSPIPE_CORE_DllAPI
    static std::atomic<float> default_max_rx_rate;

    /**
     * @brief Global value to store the default downsampling factor in this execution.
     *
     * This value can change along the execution.
     * Every new TopicQoS object will use this value as \c downsampling default.
     */
    DDSPIPE_CORE_DllAPI
    static std::atomic<unsigned int> default_downsampling;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    //! Durability kind
    utils::Fuzzy<DurabilityKind> durability_qos;

    //! Reliability kind
    utils::Fuzzy<ReliabilityKind> reliability_qos;

    //! Ownership kind of the topic
    utils::Fuzzy<OwnershipQosPolicyKind> ownership_qos;

    //! Whether the topics uses partitions
    utils::Fuzzy<bool> use_partitions;

    //! Whether the topic has key or not
    utils::Fuzzy<bool> keyed;

    //! Depth of the history
    utils::Fuzzy<HistoryDepthType> history_depth;

    //! Discard msgs if less than 1/rate seconds elapsed since the last sample was transmitted [Hz]. Default: 0 (no limit)
    utils::Fuzzy<float> max_tx_rate;

    //! Discard msgs if less than 1/rate seconds elapsed since the last sample was processed [Hz]. Default: 0 (no limit)
    utils::Fuzzy<float> max_rx_rate;

    //! Downsampling factor: keep 1 out of every *downsampling* samples received (downsampling=1 <=> no downsampling)
    utils::Fuzzy<unsigned int> downsampling;

    //////////////////////////////////////////
    // GLOBAL VARIABLES' DEFAULT VALUES
    //////////////////////////////////////////

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
