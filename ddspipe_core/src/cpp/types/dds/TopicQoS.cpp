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
// limitations under the License.

/**
 * @file TopicQoS.cpp
 *
 */

#include <ddspipe_core/types/dds/TopicQoS.hpp>
#include <cpp_utils/utils.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

std::atomic<HistoryDepthType> TopicQoS::default_history_depth{HISTORY_DEPTH_DEFAULT};
std::atomic<float> TopicQoS::default_max_tx_rate{MAX_TX_RATE_DEFAULT};
std::atomic<float> TopicQoS::default_max_rx_rate{MAX_RX_RATE_DEFAULT};
std::atomic<unsigned int> TopicQoS::default_downsampling{DOWNSAMPLING_DEFAULT};

TopicQoS::TopicQoS()
{
    // Set the default durability
    durability_qos.set_value(DURABILITY_QOS_DEFAULT, utils::FuzzyLevelValues::fuzzy_level_default);

    // Set the default reliabitliy
    reliability_qos.set_value(RELIABILITY_QOS_DEFAULT, utils::FuzzyLevelValues::fuzzy_level_default);

    // Set the default ownership
    ownership_qos.set_value(OWNERSHIP_QOS_DEFAULT, utils::FuzzyLevelValues::fuzzy_level_default);

    // Set whether by default the Topic should use partitions
    use_partitions.set_value(USE_PARTITIONS_DEFAULT, utils::FuzzyLevelValues::fuzzy_level_default);

    // Set whether by default the Topic has a key
    keyed.set_value(KEYED_DEFAULT, utils::FuzzyLevelValues::fuzzy_level_default);

    // Set the default history depth
    history_depth.set_value(default_history_depth, utils::FuzzyLevelValues::fuzzy_level_default);

    // Set the default max transmision rate
    max_tx_rate.set_value(default_max_tx_rate, utils::FuzzyLevelValues::fuzzy_level_default);

    // Set the default max reception rate
    max_rx_rate.set_value(default_max_rx_rate, utils::FuzzyLevelValues::fuzzy_level_default);

    // Set the default downsampling
    downsampling.set_value(default_downsampling, utils::FuzzyLevelValues::fuzzy_level_default);
}

bool TopicQoS::operator ==(
        const TopicQoS& other) const noexcept
{
    return
        this->durability_qos == other.durability_qos &&
        this->reliability_qos == other.reliability_qos &&
        this->ownership_qos == other.ownership_qos &&
        this->use_partitions == other.use_partitions &&
        this->history_depth == other.history_depth &&
        this->keyed == other.keyed &&
        this->max_tx_rate == other.max_tx_rate &&
        this->max_rx_rate == other.max_rx_rate &&
        this->downsampling == other.downsampling;
}

bool TopicQoS::is_reliable() const noexcept
{
    return reliability_qos == ReliabilityKind::RELIABLE;
}

bool TopicQoS::is_transient_local() const noexcept
{
    return durability_qos == DurabilityKind::TRANSIENT_LOCAL;
}

bool TopicQoS::has_ownership() const noexcept
{
    return ownership_qos == OwnershipQosPolicyKind::EXCLUSIVE_OWNERSHIP_QOS;
}

bool TopicQoS::has_partitions() const noexcept
{
    return use_partitions;
}

void TopicQoS::set_qos(const TopicQoS& qos) noexcept
{
    if (!durability_qos.is_set() && qos.durability_qos.is_set()) {
        durability_qos.set_value(qos.durability_qos.get_value());
    }

    if (!reliability_qos.is_set() && qos.reliability_qos.is_set()) {
        reliability_qos.set_value(qos.reliability_qos.get_value());
    }

    if (!ownership_qos.is_set() && qos.ownership_qos.is_set()) {
        ownership_qos.set_value(qos.ownership_qos.get_value());
    }

    if (!use_partitions.is_set() && qos.use_partitions.is_set()) {
        use_partitions.set_value(qos.use_partitions.get_value());
    }

    if (!history_depth.is_set() && qos.history_depth.is_set()) {
        history_depth.set_value(qos.history_depth.get_value());
    }

    if (!keyed.is_set() && qos.keyed.is_set()) {
        keyed.set_value(qos.keyed.get_value());
    }

    if (!max_tx_rate.is_set() && qos.max_tx_rate.is_set()) {
        max_tx_rate.set_value(qos.max_tx_rate.get_value());
    }

    if (!max_rx_rate.is_set() && qos.max_rx_rate.is_set()) {
        max_rx_rate.set_value(qos.max_rx_rate.get_value());
    }

    if (!downsampling.is_set() && qos.downsampling.is_set()) {
        downsampling.set_value(qos.downsampling.get_value());
    }
}

std::ostream& operator <<(
        std::ostream& os,
        const DurabilityKind& kind)
{
    switch (kind)
    {
        case DurabilityKind::VOLATILE:
            os << "VOLATILE";
            break;

        case DurabilityKind::TRANSIENT_LOCAL:
            os << "TRANSIENT_LOCAL";
            break;

        case DurabilityKind::TRANSIENT:
            os << "TRANSIENT";
            break;

        case DurabilityKind::PERSISTENT:
            os << "PERSISTENT";
            break;

        default:
            utils::tsnh(utils::Formatter() << "Invalid Durability Kind.");
            break;
    }

    return os;
}

std::ostream& operator <<(
        std::ostream& os,
        const ReliabilityKind& kind)
{
    switch (kind)
    {
        case ReliabilityKind::RELIABLE:
            os << "RELIABLE";
            break;

        case ReliabilityKind::BEST_EFFORT:
            os << "BEST_EFFORT";
            break;

        default:
            utils::tsnh(utils::Formatter() << "Invalid Reliability Kind.");
            break;
    }

    return os;
}

std::ostream& operator <<(
        std::ostream& os,
        const OwnershipQosPolicyKind& kind)
{
    switch (kind)
    {
        case OwnershipQosPolicyKind::SHARED_OWNERSHIP_QOS:
            os << "SHARED";
            break;

        case OwnershipQosPolicyKind::EXCLUSIVE_OWNERSHIP_QOS:
            os << "EXCLUSIVE";
            break;

        default:
            utils::tsnh(utils::Formatter() << "Invalid Ownership Kind.");
            break;
    }

    return os;
}

std::ostream& operator <<(
        std::ostream& os,
        const TopicQoS& qos)
{
    os <<
        "TopicQoS{" << qos.durability_qos <<
        ";" << qos.reliability_qos <<
        ";" << qos.ownership_qos <<
        (qos.has_partitions() ? ";partitions" : "") <<
        (qos.keyed ? ";keyed" : "") <<
        ";depth(" << qos.history_depth << ")" <<
        ";max_tx_rate(" << qos.max_tx_rate << ")" <<
        ";max_rx_rate(" << qos.max_rx_rate << ")" <<
        ";downsampling(" << qos.downsampling << ")" <<
        "}";

    return os;
}

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
