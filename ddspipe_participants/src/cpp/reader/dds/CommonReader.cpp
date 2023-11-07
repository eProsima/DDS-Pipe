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

#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Log.hpp>
#include <cpp_utils/math/math_extension.hpp>

#include <ddspipe_core/interface/IRoutingData.hpp>
#include <ddspipe_core/types/data/RtpsPayloadData.hpp>

#include <ddspipe_participants/reader/dds/CommonReader.hpp>
#include <ddspipe_participants/types/dds/TopicDataType.hpp>

#include <utils/utils.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

using namespace eprosima::ddspipe::core::types;

CommonReader::~CommonReader()
{
    // This variables should be set, otherwise the creation should have fail
    // Anyway, the if case is used for safety reasons
    if (dds_subscriber_)
    {
        if (reader_)
        {
            reader_->set_listener(nullptr);
            dds_subscriber_->delete_datareader(reader_);
        }

        dds_participant_->delete_subscriber(dds_subscriber_);
    }

    logInfo(DDSPIPE_DDS_READER, "Deleting CommonReader created in Participant " <<
            participant_id_ << " for topic " << topic_);
}

void CommonReader::init()
{
    logInfo(DDSPIPE_DDS_READER, "Initializing reader in " << participant_id_ << " for topic " << topic_ << ".");

    // Create subscriber
    dds_subscriber_ = dds_participant_->create_subscriber(
        reckon_subscriber_qos_()
        );

    if (!dds_subscriber_)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Subscriber for Participant " <<
                      participant_id_ << " in topic " << topic_ << ".");
    }

    // Create CommonReader
    // Listener must be set in creation as no callbacks should be missed
    // It is safe to do so here as object is already created and callbacks do not require anything set in this method
    reader_ = dds_subscriber_->create_datareader(
        dds_topic_,
        reckon_reader_qos_(),
        nullptr,
        eprosima::fastdds::dds::StatusMask::all(),
        payload_pool_);

    if (!reader_)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating DataReader for Participant " <<
                      participant_id_ << " in topic " << topic_ << ".");
    }

    // Set listener after entity creation to avoid SEGFAULT (produced when callback using reader_ is
    // invoked before the variable is fully set)
    reader_->set_listener(this);

}

void CommonReader::on_data_available(
        fastdds::dds::DataReader* /* reader */)
{
    logInfo(DDSPIPE_DDS_READER, "On data available in reader in " << participant_id_ << " for topic " << topic_ << ".");

    on_data_available_();
}

CommonReader::CommonReader(
        const core::types::ParticipantId& participant_id,
        const core::types::DdsTopic& topic,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        fastdds::dds::DomainParticipant* participant,
        fastdds::dds::Topic* topic_entity)
    : BaseReader(participant_id, topic.topic_qos.max_rx_rate, topic.topic_qos.downsampling)
    , dds_participant_(participant)
    , dds_topic_(topic_entity)
    , payload_pool_(payload_pool)
    , topic_(topic)
    , dds_subscriber_(nullptr)
    , reader_(nullptr)
{
    // Do nothing
}

utils::ReturnCode CommonReader::take_nts_(
        std::unique_ptr<core::IRoutingData>& data) noexcept
{
    // NOTE: we assume this function is called always from same thread
    // NOTE: we assume this function is called always with nullptr data

    logInfo(DDSPIPE_DDS_READER, "Taking data in " << participant_id_ << " for topic " << topic_ << ".");

    // Check if there is data available
    if (!(reader_->get_unread_count() > 0))
    {
        return utils::ReturnCode::RETCODE_NO_DATA;
    }

    RtpsPayloadData* rtps_data;
    fastdds::dds::SampleInfo info;

    // Loop for read messages until we receive one that does not come from same participant
    // NOTE: not reading local messages must be done in this loop because:
    // 1. there is no way in DDS to ignore this reader from writer as there were in RTPS
    // 2. ignore_local_endpoints would be override by xml configuration
    while (true)
    {
        // Ensure that the previous Payload gets destroyed to avoid memory leaks.
        rtps_data = new core::types::RtpsPayloadData();
        data.reset(rtps_data);

        auto ret = reader_->take_next_sample(rtps_data, &info);

        // Save the payload owner so that the memory is freed correctly.
        rtps_data->payload_owner = payload_pool_.get();

        // If error reading data
        if (!ret)
        {
            return ret;
        }

        // Check if the sample is acceptable
        if (should_accept_sample_(info))
        {
            break;
        }
    }

    logInfo(DDSPIPE_DDS_READER, "Data taken in " << participant_id_ << " for topic " << topic_ << ".");

    fill_received_data_(info, *rtps_data);

    return utils::ReturnCode::RETCODE_OK;
}

void CommonReader::enable_nts_() noexcept
{
    // If the topic is reliable, the reader will keep the samples received when it was disabled.
    // However, if the topic is best_effort, the reader will discard the samples received when it was disabled.
    if (topic_.topic_qos.is_reliable())
    {
        on_data_available_();
    }
}

fastdds::dds::SubscriberQos CommonReader::reckon_subscriber_qos_() const
{
    fastdds::dds::SubscriberQos qos = dds_participant_->get_default_subscriber_qos();
    if (topic_.topic_qos.has_partitions())
    {
        qos.partition().push_back("*");
    }
    return qos;
}

fastdds::dds::DataReaderQos CommonReader::reckon_reader_qos_() const
{
    fastdds::dds::DataReaderQos qos = dds_subscriber_->get_default_datareader_qos();

    // IMPORTANT
    // As we do not have access to TypeSupport, we do not know the size of the key
    // In order to be able to get correctly the key from a sample, it must be added in the InlineQoS
    // There is a QoS in the reader that forces the writer to send such InlineQoS: expects_inline_qos
    // Sending the key within the InlineQoS is not standard required, so it may not be compatible with other vendors
    // This extra info has a computational cost, so it is only done when the topic is keyed
    if (topic_.topic_qos.keyed)
    {
        qos.expects_inline_qos(true);
    }

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

    if (topic_.topic_qos.history_depth == 0U)
    {
        qos.history().kind = eprosima::fastdds::dds::HistoryQosPolicyKind::KEEP_ALL_HISTORY_QOS;
    }
    else
    {
        qos.history().kind = eprosima::fastdds::dds::HistoryQosPolicyKind::KEEP_LAST_HISTORY_QOS;
        qos.history().depth = topic_.topic_qos.history_depth;
    }

    return qos;
}

bool CommonReader::should_accept_sample_(
        const fastdds::dds::SampleInfo& info) noexcept
{
    // Reject samples sent by a Writer from the same Participant this Reader belongs to
    if (detail::come_from_same_participant_(
                detail::guid_from_instance_handle(info.publication_handle),
                this->dds_participant_->guid()))
    {
        return false;
    }

    return BaseReader::should_accept_sample_();
}

void CommonReader::fill_received_data_(
        const fastdds::dds::SampleInfo& info,
        core::types::RtpsPayloadData& data_to_fill) const noexcept
{
    // Store the new data that has arrived in the Track data
    // Get the writer guid
    data_to_fill.source_guid = detail::guid_from_instance_handle(info.publication_handle);
    // Get source timestamp
    data_to_fill.source_timestamp = info.source_timestamp;
    // Get Participant receiver
    data_to_fill.participant_receiver = participant_id_;

    // Set Instance Handle to data_to_fill
    if (topic_.topic_qos.keyed)
    {
        data_to_fill.instanceHandle = info.instance_handle;

        // Set change kind
        switch (info.instance_state)
        {
            case fastdds::dds::InstanceStateKind::ALIVE_INSTANCE_STATE:
                data_to_fill.kind = ChangeKind::ALIVE;
                break;

            case fastdds::dds::InstanceStateKind::NOT_ALIVE_DISPOSED_INSTANCE_STATE:
                data_to_fill.kind = ChangeKind::NOT_ALIVE_DISPOSED;
                break;

            default:
                data_to_fill.kind = ChangeKind::NOT_ALIVE_UNREGISTERED;
                break;
        }
    }
    else
    {
        data_to_fill.kind = ChangeKind::ALIVE;
    }
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
