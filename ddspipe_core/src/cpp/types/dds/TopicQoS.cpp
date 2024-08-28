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

utils::Fuzzy<TopicQoS> TopicQoS::default_topic_qos{};

TopicQoS::TopicQoS()
{
    set_default_qos();

    // This check must be done. If not, the constructor of the default Topic QoS would enter into a loop.
    if (default_topic_qos.is_set())
    {
        // The FuzzyLevel must be set so that specs overwrites the discovery values.
        set_qos(default_topic_qos, utils::FuzzyLevelValues::fuzzy_level_set);
    }
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

void TopicQoS::set_qos(
        const TopicQoS& qos,
        const utils::FuzzyLevelValues& fuzzy_level /*= utils::FuzzyLevelValues::fuzzy_level_fuzzy*/) noexcept
{
    if (durability_qos.get_level() < fuzzy_level && qos.durability_qos.is_set())
    {
        durability_qos.set_value(qos.durability_qos.get_value(), fuzzy_level);
    }

    if (reliability_qos.get_level() < fuzzy_level && qos.reliability_qos.is_set())
    {
        reliability_qos.set_value(qos.reliability_qos.get_value(), fuzzy_level);
    }

    if (ownership_qos.get_level() < fuzzy_level && qos.ownership_qos.is_set())
    {
        ownership_qos.set_value(qos.ownership_qos.get_value(), fuzzy_level);
    }

    if (use_partitions.get_level() < fuzzy_level && qos.use_partitions.is_set())
    {
        use_partitions.set_value(qos.use_partitions.get_value(), fuzzy_level);
    }

    if (history_depth.get_level() < fuzzy_level && qos.history_depth.is_set())
    {
        history_depth.set_value(qos.history_depth.get_value(), fuzzy_level);
    }

    if (keyed.get_level() < fuzzy_level && qos.keyed.is_set())
    {
        keyed.set_value(qos.keyed.get_value(), fuzzy_level);
    }

    if (max_tx_rate.get_level() < fuzzy_level && qos.max_tx_rate.is_set())
    {
        max_tx_rate.set_value(qos.max_tx_rate.get_value(), fuzzy_level);
    }

    if (max_rx_rate.get_level() < fuzzy_level && qos.max_rx_rate.is_set())
    {
        max_rx_rate.set_value(qos.max_rx_rate.get_value(), fuzzy_level);
    }

    if (downsampling.get_level() < fuzzy_level && qos.downsampling.is_set())
    {
        downsampling.set_value(qos.downsampling.get_value(), fuzzy_level);
    }
}

void TopicQoS::set_default_qos() noexcept
{
    this->durability_qos.set_value(DEFAULT_DURABILITY_QOS, utils::FuzzyLevelValues::fuzzy_level_default);
    this->reliability_qos.set_value(DEFAULT_RELIABILITY_QOS, utils::FuzzyLevelValues::fuzzy_level_default);
    this->ownership_qos.set_value(DEFAULT_OWNERSHIP_QOS, utils::FuzzyLevelValues::fuzzy_level_default);

    LivelinessQosPolicy default_liveliness_qos;
    default_liveliness_qos.kind = LivelinessQosPolicyKind::AUTOMATIC_LIVELINESS_QOS;
    default_liveliness_qos.lease_duration = DurationTimeInfinite;
    default_liveliness_qos.announcement_period = DurationTimeInfinite;
    this->liveliness_qos.set_value(default_liveliness_qos, utils::FuzzyLevelValues::fuzzy_level_default);

    this->use_partitions.set_value(DEFAULT_USE_PARTITIONS, utils::FuzzyLevelValues::fuzzy_level_default);
    this->history_depth.set_value(DEFAULT_HISTORY_DEPTH, utils::FuzzyLevelValues::fuzzy_level_default);
    this->keyed.set_value(DEFAULT_KEYED, utils::FuzzyLevelValues::fuzzy_level_default);
    this->max_tx_rate.set_value(DEFAULT_MAX_TX_RATE, utils::FuzzyLevelValues::fuzzy_level_default);
    this->max_rx_rate.set_value(DEFAULT_MAX_RX_RATE, utils::FuzzyLevelValues::fuzzy_level_default);
    this->downsampling.set_value(DEFAULT_DOWNSAMPLING, utils::FuzzyLevelValues::fuzzy_level_default);
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
        const utils::Fuzzy<DurabilityKind>& qos)
{
    os << "Fuzzy{Level(" << qos.get_level_as_str() << ") " << qos.get_reference() << "}";
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
        const utils::Fuzzy<ReliabilityKind>& qos)
{
    os << "Fuzzy{Level(" << qos.get_level_as_str() << ") " << qos.get_reference() << "}";
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
        const utils::Fuzzy<OwnershipQosPolicyKind>& qos)
{
    os << "Fuzzy{Level(" << qos.get_level_as_str() << ") " << qos.get_reference() << "}";
    return os;
}

std::ostream& operator <<(
        std::ostream& os,
        const TopicQoS& qos)
{
    os <<
        "TopicQoS{" <<
        "durability(" << qos.durability_qos << ")" <<
        ";reliability(" << qos.reliability_qos << ")" <<
        ";ownership(" << qos.ownership_qos << ")" <<
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
