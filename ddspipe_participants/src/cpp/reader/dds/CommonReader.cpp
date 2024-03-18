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
#include <ddspipe_core/monitoring/producers/StatusMonitorProducer.hpp>
#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>
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
    #if FASTRTPS_VERSION_MAJOR <= 2 && FASTRTPS_VERSION_MINOR < 12
    reader_ = dds_subscriber_->create_datareader_with_payload_pool(
        dds_topic_,
        reckon_reader_qos_(),
        payload_pool_,
        nullptr,
        eprosima::fastdds::dds::StatusMask::all());
    #else
    reader_ = dds_subscriber_->create_datareader(
        dds_topic_,
        reckon_reader_qos_(),
        nullptr,
        eprosima::fastdds::dds::StatusMask::all(),
        payload_pool_);
    #endif // if FASTRTPS_VERSION_MAJOR <= 2 && FASTRTPS_VERSION_MINOR < 13

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

    // The CommonReader notifies the reception of a message to the Monitor when a on_data_available event is received.
    // An on_data_available event can be received with more than one message, but figuring out the number of messages
    // received is not possible with the current API. Thus, the Monitor will be notified once for each on_data_available
    // and the number of messages received will be slightly inaccurate.
    monitor_msg_rx(topic_, participant_id_);

    on_data_available_();
}

void CommonReader::on_sample_lost(
        fastdds::dds::DataReader* reader,
        const fastdds::dds::SampleLostStatus& status)
{
    logWarning(DDSPIPE_DDS_READER,
            "SAMPLE_LOST | On reader " << *this << " a data sample was lost and will not be received");

    monitor_msg_lost(topic_, participant_id_);
}

void CommonReader::on_requested_incompatible_qos(
        fastdds::dds::DataReader* reader,
        const fastdds::dds::RequestedIncompatibleQosStatus& status)
{
    logWarning(DDSPIPE_DDS_READER,
            "TOPIC_MISMATCH_QOS | Reader " << *this << " found a remote Writer with incompatible QoS");

    monitor_qos_mismatch(topic_);
    monitor_error("QOS_MISMATCH");
}

void CommonReader::on_inconsistent_topic(
        fastdds::dds::Topic* topic,
        fastdds::dds::InconsistentTopicStatus status)
{
    logWarning(DDSPIPE_DDS_READER,
            "TOPIC_MISMATCH_TYPE | Reader " << *this <<
            " found a remote Writer with same topic name but incompatible type");

    monitor_type_mismatch(topic_);
    monitor_error("TYPE_MISMATCH");
}

CommonReader::CommonReader(
        const ParticipantId& participant_id,
        const DdsTopic& topic,
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
    // NOTE: we assume this function is always called from the same thread
    // NOTE: we assume this function is always called with nullptr data

    logInfo(DDSPIPE_DDS_READER, "Taking data in " << participant_id_ << " for topic " << topic_ << ".");

    // Check if there is data available
    if (!(reader_->get_unread_count() > 0))
    {
        return utils::ReturnCode::RETCODE_NO_DATA;
    }

    std::unique_ptr<RtpsPayloadData> rtps_data;
    fastdds::dds::SampleInfo info;

    do
    {
        rtps_data.reset(new RtpsPayloadData());

        auto ret = reader_->take_next_sample(rtps_data.get(), &info);

        // If the payload owner is not set, rtps_data won't release the payload on destruction
        rtps_data->payload_owner = payload_pool_.get();

        if (!ret)
        {
            // There has been an error taking the data. Exit.
            return ret;
        }
    } while (!should_accept_sample_(info));

    logInfo(DDSPIPE_DDS_READER, "Data taken in " << participant_id_ << " for topic " << topic_ << ".");

    // Verify that the rtps_data object is valid
    if (!rtps_data)
    {
        logError(DDSPIPE_DDS_READER, "The data taken by the reader is not valid.");
        return utils::ReturnCode::RETCODE_ERROR;
    }

    fill_received_data_(info, *rtps_data);

    // data is a unique_ptr; the memory will be handled correctly.
    data.reset(rtps_data.release());

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
        RtpsPayloadData& data_to_fill) const noexcept
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
