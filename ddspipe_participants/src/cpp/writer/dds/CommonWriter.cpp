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


#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/participant/RTPSParticipant.h>
#include <fastrtps/rtps/common/CacheChange.h>

#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Log.hpp>
#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_participants/efficiency/cache_change/CacheChangePool.hpp>
#include <ddspipe_participants/writer/dds/CommonWriter.hpp>
#include <ddspipe_participants/types/dds/RouterCacheChange.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

using namespace eprosima::ddspipe::core::types;

std::atomic<utils::Duration_ms> CommonWriter::wait_all_acked_timeout{0};

CommonWriter::~CommonWriter()
{
    // This variables should be set, otherwise the creation should have fail
    // Anyway, the if case is used for safety reasons
    if (writer_)
    {
        writer_->set_listener(nullptr);
        dds_publisher_->delete_datawriter(writer_);

        dds_participant_->delete_publisher(dds_publisher_);
    }

    logInfo(DDSPIPE_DDS_WRITER, "Deleting CommonWriter created in Participant " <<
            participant_id_ << " for topic " << topic_);
}

void CommonWriter::init()
{
    // Create publisher
    dds_publisher_ = dds_participant_->create_publisher(
        reckon_publisher_qos_()
    );

    if (!dds_publisher_)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Publisher for Participant " <<
                      participant_id_ << " in topic " << topic_ << ".");
    }

    // Create CommonReader
    // Listener must be set in creation as no callbacks should be missed
    // It is safe to do so here as object is already created and callbacks do not require anything set in this method
    writer_ = dds_publisher_->create_datawriter(
        dds_topic_,
        reckon_writer_qos_(),
        nullptr);

    if (!writer_)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating DataWriter for Participant " <<
                      participant_id_ << " in topic " << topic_ << ".");
    }
}

CommonWriter::CommonWriter(
        const ParticipantId& participant_id,
        const DdsTopic& topic,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        fastdds::dds::DomainParticipant* participant,
        fastdds::dds::Topic* topic_entity)
    : BaseWriter(participant_id)
    , dds_participant_(participant)
    , dds_topic_(topic_entity)
    , payload_pool_(payload_pool)
    , topic_(topic)
    , dds_publisher_(nullptr)
    , writer_(nullptr)
{
    // Do nothing
}

// Specific enable/disable do not need to be implemented
utils::ReturnCode CommonWriter::write_nts_(
        core::IRoutingData& data) noexcept
{
    return utils::ReturnCode::RETCODE_ERROR;
    // TODO
}

fastdds::dds::PublisherQos CommonWriter::reckon_publisher_qos_() const noexcept
{
    // NOTE: this does not handle different partitions
    fastdds::dds::PublisherQos qos = dds_participant_->get_default_publisher_qos();

    if (topic_.topic_qos.has_partitions())
    {
        qos.partition().push_back("*");
    }

    return qos;
}

fastdds::dds::DataWriterQos CommonWriter::reckon_writer_qos_() const noexcept
{
    fastdds::dds::DataWriterQos qos = dds_publisher_->get_default_datawriter_qos();

    dds_publisher_->get_default_datawriter_qos();

    qos.durability().kind =
        (topic_.topic_qos.is_transient_local())
            ? fastdds::dds::DurabilityQosPolicyKind::TRANSIENT_LOCAL_DURABILITY_QOS
            : fastdds::dds::DurabilityQosPolicyKind::VOLATILE_DURABILITY_QOS;

    qos.reliability().kind =
        (topic_.topic_qos.is_reliable())
            ? fastdds::dds::ReliabilityQosPolicyKind::RELIABLE_RELIABILITY_QOS
            : fastdds::dds::ReliabilityQosPolicyKind::BEST_EFFORT_RELIABILITY_QOS;

    qos.ownership().kind =
        (topic_.topic_qos.has_ownership())
            ? fastdds::dds::OwnershipQosPolicyKind::EXCLUSIVE_OWNERSHIP_QOS
            : fastdds::dds::OwnershipQosPolicyKind::SHARED_OWNERSHIP_QOS;

    qos.history().depth = topic_.topic_qos.history_depth;

    // Set minimum deadline so it matches with everything
    qos.deadline().period = eprosima::fastrtps::Duration_t(0);

    return qos;
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
