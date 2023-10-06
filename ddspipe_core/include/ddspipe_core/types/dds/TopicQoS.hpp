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
    void set_qos(const TopicQoS& qos) noexcept;

    /////////////////////////
    // GLOBAL VARIABLES
    /////////////////////////

    /**
     * @brief Global value to store the default history depth in this execution.
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

    //! Durability kind (Default = VOLATILE)
    utils::Fuzzy<DurabilityKind> durability_qos;

    //! Reliability kind (Default = BEST_EFFORT)
    utils::Fuzzy<ReliabilityKind> reliability_qos;

    //! Ownership kind of the topic
    utils::Fuzzy<OwnershipQosPolicyKind> ownership_qos;

    //! Whether the topics uses partitions
    utils::Fuzzy<bool> use_partitions;

    /**
     * @brief History Qos
     *
     * @note Default value would be taken from \c default_history_depth in object creation.
     * @note It only stores the depth because in pipe it will always be keep last, as RTPS has not resource limits.
     */
    utils::Fuzzy<HistoryDepthType> history_depth;

    //! Whether the topic has key or not
    utils::Fuzzy<bool> keyed;

    //! Discard msgs if less than 1/rate seconds elapsed since the last sample was transmitted [Hz]. Default: 0 (no limit)
    utils::Fuzzy<float> max_tx_rate;

    //! Discard msgs if less than 1/rate seconds elapsed since the last sample was processed [Hz]. Default: 0 (no limit)
    utils::Fuzzy<float> max_rx_rate;

    //! Downsampling factor: keep 1 out of every *downsampling* samples received (downsampling=1 <=> no downsampling)
    utils::Fuzzy<unsigned int> downsampling;


    //! Durability kind (Default = VOLATILE)
    static constexpr DurabilityKind DURABILITY_QOS_DEFAULT = DurabilityKind::VOLATILE;

    //! Reliability kind (Default = BEST_EFFORT)
    static constexpr ReliabilityKind RELIABILITY_QOS_DEFAULT = ReliabilityKind::BEST_EFFORT;

    //! Ownership kind (Default = SHARED_OWNERSHIP)
    static constexpr OwnershipQosPolicyKind OWNERSHIP_QOS_DEFAULT = OwnershipQosPolicyKind::SHARED_OWNERSHIP_QOS;

    //! Whether the topic uses partitions (Default = False)
    static constexpr bool USE_PARTITIONS_DEFAULT = false;

    //! History depth (Default = 5000)
    static constexpr HistoryDepthType HISTORY_DEPTH_DEFAULT = 5000;

    //! Whether the topic has a key (Default = False)
    static constexpr bool KEYED_DEFAULT = false;

    //! Max Tx Rate (Default = 0)
    static constexpr float MAX_TX_RATE_DEFAULT = 0;

    //! Max Rx Rate (Default = 0)
    static constexpr float MAX_RX_RATE_DEFAULT = 0;

    //! Downsampling (Default = 1)
    static constexpr unsigned int DOWNSAMPLING_DEFAULT = 1;
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
