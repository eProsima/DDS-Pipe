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

#include <fastdds/dds/subscriber/qos/ReaderQos.hpp>
#include <fastdds/rtps/participant/RTPSParticipant.hpp>
#include <fastdds/rtps/RTPSDomain.hpp>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Log.hpp>
#include <cpp_utils/qos/qos_utils.hpp>

#include <ddspipe_core/interface/IRoutingData.hpp>
#include <ddspipe_core/monitoring/producers/StatusMonitorProducer.hpp>
#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>
#include <ddspipe_core/types/data/RtpsPayloadData.hpp>

#include <ddspipe_participants/reader/rtps/CommonReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace rtps {

using namespace eprosima::ddspipe::core::types;
using eprosima::ddspipe::core::types::operator <<;

CommonReader::CommonReader(
        const ParticipantId& participant_id,
        const DdsTopic& topic,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        fastdds::rtps::RTPSParticipant* rtps_participant,
        const fastdds::rtps::HistoryAttributes& history_attributes,
        const fastdds::rtps::ReaderAttributes& reader_attributes,
        const fastdds::rtps::TopicDescription& topic_description,
        const fastdds::dds::ReaderQos& reader_qos,
        const std::set<std::string> blocked_filtered_guidlist,
        bool has_filter)
    : BaseReader(participant_id, topic.topic_qos.max_rx_rate, topic.topic_qos.downsampling)
    , rtps_participant_(rtps_participant)
    , payload_pool_(payload_pool)
    , topic_(topic)
    , rtps_reader_(nullptr)
    , rtps_history_(nullptr)
    , history_attributes_(history_attributes)
    , reader_attributes_(reader_attributes)
    , topic_description_(topic_description)
    , reader_qos_(reader_qos)
    , blocked_filtered_guidlist_(blocked_filtered_guidlist)
{
    // Do nothing.
}

CommonReader::~CommonReader()
{
    // This variables should be set, otherwise the creation should have fail
    // Anyway, the if case is used for safety reasons

    // Delete reader
    if (rtps_reader_)
    {
        // Unset listener before destruction (not necessary in principle, but just in case)
        rtps_reader_->set_listener(nullptr);
        fastdds::rtps::RTPSDomain::removeRTPSReader(rtps_reader_);
    }

    // Delete History
    if (rtps_history_)
    {
        delete rtps_history_;
    }

    EPROSIMA_LOG_INFO(DDSPIPE_RTPS_READER, "Deleting CommonReader created in Participant " <<
            participant_id_ << " for topic " << topic_);
}

void CommonReader::init()
{
    internal_entities_creation_(
        history_attributes_,
        reader_attributes_,
        topic_description_,
        reader_qos_);
}

void CommonReader::internal_entities_creation_(
        const fastdds::rtps::HistoryAttributes& history_attributes,
        const fastdds::rtps::ReaderAttributes& reader_attributes,
        const fastdds::rtps::TopicDescription& topic_description,
        const fastdds::dds::ReaderQos& reader_qos)
{
    // Copy reader attributes because fast needs it non const (do not ask why)
    fastdds::rtps::ReaderAttributes non_const_reader_attributes = reader_attributes;

    // Create History
    rtps_history_ = new fastdds::rtps::ReaderHistory(history_attributes);

    // Create CommonReader
    // Listener must be set in creation as no callbacks should be missed
    // It is safe to do so here as object is already created and callbacks do not require anything set in this method
    // Also, no data races can ocurr as no callback will be called until this reader is registered
    rtps_reader_ = fastdds::rtps::RTPSDomain::createRTPSReader(
        rtps_participant_,
        non_const_reader_attributes,
        payload_pool_,
        rtps_history_,
        this);

    if (!rtps_reader_)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Simple RTPSReader for Participant " <<
                      participant_id_ << " in topic " << topic_ << ".");
    }

    // Register reader with topic
    if (!rtps_participant_->register_reader(rtps_reader_, topic_description, reader_qos))
    {
        // In case it fails, remove reader and throw exception
        fastdds::rtps::RTPSDomain::removeRTPSReader(rtps_reader_);
        throw utils::InitializationException(utils::Formatter() << "Error registering topic " << topic_ <<
                      " for Simple RTPSReader in Participant " << participant_id_);
    }

    EPROSIMA_LOG_INFO(DDSPIPE_RTPS_READER,
            "New CommonReader created in Participant " << participant_id_ << " for topic " <<
            topic_ << " with guid " << rtps_reader_->getGuid());
}

core::types::Guid CommonReader::guid() const noexcept
{
    return rtps_reader_->getGuid();
}

fastdds::RecursiveTimedMutex& CommonReader::get_rtps_mutex() const noexcept
{
    return rtps_reader_->getMutex();
}

uint64_t CommonReader::get_unread_count() const noexcept
{
    return rtps_reader_->get_unread_count();
}

core::types::DdsTopic CommonReader::topic() const noexcept
{
    return topic_;
}

// void CommonReader::update_content_topic_filter(std::string expression)
// {
//     return;
//     //filtered_topic_->set_filter_expression(expression, {});
// }

utils::ReturnCode CommonReader::take_nts_(
        std::unique_ptr<core::IRoutingData>& data) noexcept
{
    // Check if there is data available
    if (!(rtps_reader_->get_unread_count() > 0))
    {
        return utils::ReturnCode::RETCODE_NO_DATA;
    }

    // Read first change of the history
    auto received_change = rtps_reader_->next_untaken_cache();
    if (!received_change)
    {
        // Error reading.
        return utils::ReturnCode::RETCODE_ERROR;
    }

    // If data received is not correct, discard it and remove it from history
    auto ret = is_data_correct_(received_change);
    if (ret != utils::ReturnCode::RETCODE_OK)
    {
        // Remove the change in the History and release it in the reader
        rtps_reader_->get_history()->remove_change(received_change);
        return ret;
    }

    // Store the new data that has arrived in the Track data
    auto data_ptr = create_data_(*received_change);
    fill_received_data_(*received_change, *data_ptr);

    std::ostringstream guid_ss;
    guid_ss << data_ptr->source_guid;
    std::string source_guid_str = guid_ss.str();

    // check if the guid from the current data is filtered by the allowed partition list
    if (blocked_filtered_guidlist_.find(source_guid_str) != blocked_filtered_guidlist_.end())
    {
        // partition not allowed
        rtps_reader_->get_history()->remove_change(received_change);
        return utils::ReturnCode::RETCODE_NOT_ENABLED;
    }

    data.reset(data_ptr);

    // Remove the change in the History and release it in the reader
    rtps_reader_->get_history()->remove_change(received_change);

    return utils::ReturnCode::RETCODE_OK;
}

RtpsPayloadData* CommonReader::create_data_(
        const fastdds::rtps::CacheChange_t& received_change) const noexcept
{
    return new RtpsPayloadData();
}

void CommonReader::fill_received_data_(
        const fastdds::rtps::CacheChange_t& received_change,
        RtpsPayloadData& data_to_fill) const noexcept
{
    // Store the new data that has arrived in the Track data
    // Get the writer guid
    data_to_fill.source_guid = received_change.writerGUID;

    auto original_source_guid = received_change.write_params.original_writer_info().original_writer_guid();
    if (original_source_guid == fastdds::rtps::GUID_t::unknown())
    {
        // If the original source guid is unknown, this is the first time a router receives this data
        // and the current source is the original
        data_to_fill.original_writer_info.original_writer_guid(data_to_fill.source_guid);
    }
    else
    {
        // If the original source guid is known, set it
        data_to_fill.original_writer_info = received_change.write_params.original_writer_info();
    }

    // Get source timestamp
    data_to_fill.source_timestamp = received_change.sourceTimestamp;
    // Get Participant receiver
    data_to_fill.participant_receiver = participant_id_;

    // Store it in DdsPipe PayloadPool if size is bigger than 0
    // NOTE: in case of keyed topics an empty payload is possible
    if (received_change.serializedPayload.length > 0)
    {
        payload_pool_->get_payload(
            received_change.serializedPayload,
            data_to_fill.payload);

        data_to_fill.payload_owner = payload_pool_.get();
    }

    // Set Instance Handle to data_to_fill
    if (topic_.topic_qos.keyed)
    {
        data_to_fill.instanceHandle = received_change.instanceHandle;
    }

    // Set change kind
    data_to_fill.kind = received_change.kind;

    // Note: do not fill writer specific properties in this data_to_fill from this kind of Readers.
    // Implement specific class for filling it.

    logDebug(DDSPIPE_RTPS_COMMONREADER_LISTENER,
            "Data transmiting to track from Reader " << *this << " with payload " <<
            data_to_fill.payload << " from remote writer " << data_to_fill.source_guid);

}

void CommonReader::enable_nts_() noexcept
{
    // If the topic is reliable, the reader will keep the samples received when it was disabled.
    // However, if the topic is best_effort, the reader will discard the samples received when it was disabled.
    if (topic_.topic_qos.is_reliable())
    {
        std::lock_guard<eprosima::fastdds::RecursiveTimedMutex> lock(get_rtps_mutex());
        on_data_available_();
    }
}

bool CommonReader::should_accept_change_(
        const fastdds::rtps::CacheChange_t* change) noexcept
{
    // Reject samples sent by a Writer from the same Participant this Reader belongs to
    if (come_from_this_participant_(change))
    {
        return false;
    }

    return should_accept_sample_();
}

bool CommonReader::come_from_this_participant_(
        const fastdds::rtps::CacheChange_t* change) const noexcept
{
    return come_from_this_participant_(change->writerGUID);
}

bool CommonReader::come_from_this_participant_(
        const fastdds::rtps::GUID_t guid) const noexcept
{
    return guid.guidPrefix == rtps_reader_->getGuid().guidPrefix;
}

fastdds::rtps::HistoryAttributes CommonReader::reckon_history_attributes_(
        const core::types::DdsTopic& topic) noexcept
{
    fastdds::rtps::HistoryAttributes att;
    att.memoryPolicy =
            eprosima::fastdds::rtps::MemoryManagementPolicy_t::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;

    att.maximumReservedCaches = topic.topic_qos.history_depth;
    if (att.maximumReservedCaches > 0 && att.initialReservedCaches > att.maximumReservedCaches)
    {
        att.initialReservedCaches = att.maximumReservedCaches;
    }
    att.extraReservedCaches = 0;

    return att;
}

fastdds::rtps::ReaderAttributes CommonReader::reckon_reader_attributes_(
        const core::types::DdsTopic& topic) noexcept
{
    fastdds::rtps::ReaderAttributes att;

    // Set Durability
    att.endpoint.durabilityKind = topic.topic_qos.durability_qos;

    // Set Reliability
    att.endpoint.reliabilityKind = topic.topic_qos.reliability_qos;

    // Set if topic has key
    if (topic.topic_qos.keyed)
    {
        att.endpoint.topicKind = eprosima::fastdds::rtps::WITH_KEY;

        // If the topic has a key, request inline qos (containing the instance handle)
        att.expects_inline_qos = true;
    }
    else
    {
        att.endpoint.topicKind = eprosima::fastdds::rtps::NO_KEY;
    }

    // Ownership and Partitions are not part of RTPS, thus they are set in properties

    return att;
}

fastdds::rtps::TopicDescription CommonReader::reckon_topic_description_(
        const core::types::DdsTopic& topic) noexcept
{
    fastdds::rtps::TopicDescription topic_description;

    topic_description.topic_name = topic.m_topic_name;
    topic_description.type_name = topic.type_name;

    // Check TypeIdentifierPair in DdsTopic is not empty
    if (topic.type_identifiers == fastdds::dds::xtypes::TypeIdentifierPair())
    {
        return topic_description;
    }

    // Set TypeInformation of the discovered type
    fastdds::dds::xtypes::TypeInformation type_information;

    auto try_get_type_information = [&](const fastdds::dds::xtypes::TypeIdentifierPair& identifiers) -> bool
            {
                return fastdds::dds::RETCODE_OK ==
                       fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().
                               get_type_information(
                    identifiers, type_information);
            };

    if (!try_get_type_information(topic.type_identifiers))
    {
        // If get_type_information fails with TypeIdentifierPair try just with complete
        fastdds::dds::xtypes::TypeIdentifierPair complete_only;
        complete_only.type_identifier1(topic.type_identifiers.type_identifier1());

        if (!try_get_type_information(complete_only))
        {
            // If get_type_information fails with complete TypeIdentifier try just with minimal
            fastdds::dds::xtypes::TypeIdentifierPair minimal_only;
            minimal_only.type_identifier2(topic.type_identifiers.type_identifier2());

            if (!try_get_type_information(minimal_only))
            {
                EPROSIMA_LOG_WARNING(DDSPIPE_RTPS_COMMONREADER_LISTENER,
                        "Failed to get TypeInformation for type " << topic.type_name);

                return topic_description;
            }
        }
    }

    topic_description.type_information = type_information;

    return topic_description;
}

fastdds::dds::ReaderQos CommonReader::reckon_reader_qos_(
        const core::types::DdsTopic& topic) noexcept
{
    fastdds::dds::ReaderQos properties;

    // Set Durability
    properties.m_durability.kind =
            (topic.topic_qos.is_transient_local()
            ? eprosima::fastdds::dds::DurabilityQosPolicyKind_t::TRANSIENT_LOCAL_DURABILITY_QOS
            : eprosima::fastdds::dds::DurabilityQosPolicyKind_t::VOLATILE_DURABILITY_QOS);

    // Set Reliability
    properties.m_reliability.kind =
            (topic.topic_qos.is_reliable()
            ? eprosima::fastdds::dds::ReliabilityQosPolicyKind::RELIABLE_RELIABILITY_QOS
            : eprosima::fastdds::dds::ReliabilityQosPolicyKind::BEST_EFFORT_RELIABILITY_QOS);

    // If topic with partitions, set this CommonReader in *
    if (topic.topic_qos.use_partitions)
    {
        properties.m_partition.push_back("*");
    }

    // If topic is with ownership
    properties.m_ownership.kind = topic.topic_qos.ownership_qos;

    return properties;
}

fastdds::dds::ReaderQos CommonReader::filter_reader_qos_(
        const core::types::DdsTopic& topic,
        const std::set<std::string> partitions) noexcept
{
    fastdds::dds::ReaderQos properties;

    // Set Durability
    properties.m_durability.kind =
            (topic.topic_qos.is_transient_local()
            ? eprosima::fastdds::dds::DurabilityQosPolicyKind_t::TRANSIENT_LOCAL_DURABILITY_QOS
            : eprosima::fastdds::dds::DurabilityQosPolicyKind_t::VOLATILE_DURABILITY_QOS);

    // Set Reliability
    properties.m_reliability.kind =
            (topic.topic_qos.is_reliable()
            ? eprosima::fastdds::dds::ReliabilityQosPolicyKind::RELIABLE_RELIABILITY_QOS
            : eprosima::fastdds::dds::ReliabilityQosPolicyKind::BEST_EFFORT_RELIABILITY_QOS);

    // Add the filter partitions
    for (std::string p: partitions)
    {
        properties.m_partition.push_back(p.c_str());
    }

    // If topic is with ownership
    properties.m_ownership.kind = topic.topic_qos.ownership_qos;

    return properties;
}

void CommonReader::on_new_cache_change_added(
        fastdds::rtps::RTPSReader* reader,
        const fastdds::rtps::CacheChange_t* const change) noexcept
{
    monitor_msg_rx(topic_, participant_id_);

    if (should_accept_change_(change))
    {
        // Do not remove previous received changes so they can be read when the reader is enabled
        if (enabled_)
        {
            // Call Track callback (by calling BaseReader callback method)
            logDebug(DDSPIPE_RTPS_COMMONREADER_LISTENER,
                    "Data arrived to Reader " << *this << " with payload " << change->serializedPayload << " from " <<
                    change->writerGUID);
            on_data_available_();
        }
        else
        {
            // Remove received change if the CommonReader is disbled and the topic is not reliable
            if (!topic_.topic_qos.is_reliable())
            {
                reader->get_history()->remove_change((fastdds::rtps::CacheChange_t*)change);
                logDebug(DDSPIPE_RTPS_COMMONREADER_LISTENER,
                        "Change removed from history");
            }
        }
    }
    else
    {
        EPROSIMA_LOG_INFO(
            DDSPIPE_RTPS_COMMONREADER_LISTENER,
            "Rejected received data in reader " << *this << ".");

        // Change rejected, do not send it forward and remove it
        // TODO: do this more elegantly

        // WARNING: Removing an unacceptable change here is valid given that Fast-DDS internal reader's mutex is locked.
        // If the mutex wasn't locked, the track's transmit thread could take an unacceptable sample before it gets
        // deleted here.
        reader->get_history()->remove_change((fastdds::rtps::CacheChange_t*)change);
    }
}

void CommonReader::on_reader_matched(
        fastdds::rtps::RTPSReader*,
        const fastdds::rtps::MatchingInfo& info) noexcept
{
    if (!come_from_this_participant_(info.remoteEndpointGuid))
    {
        if (info.status == fastdds::rtps::MatchingStatus::MATCHED_MATCHING)
        {
            EPROSIMA_LOG_INFO(DDSPIPE_RTPS_COMMONREADER_LISTENER,
                    "Reader " << *this << " in topic " << topic_.serialize() <<
                    " matched with a new Writer with guid " <<
                    info.remoteEndpointGuid);
        }
        else
        {
            EPROSIMA_LOG_INFO(DDSPIPE_RTPS_COMMONREADER_LISTENER,
                    "Reader " << *this << " in topic " << topic_.serialize() << " unmatched with Writer " <<
                    info.remoteEndpointGuid);
        }
    }
}

void CommonReader::on_requested_incompatible_qos(
        fastdds::rtps::RTPSReader*,
        eprosima::fastdds::dds::PolicyMask qos) noexcept
{
    EPROSIMA_LOG_WARNING(DDSPIPE_RTPS_COMMONREADER_LISTENER,
            "TOPIC_MISMATCH_QOS | Reader " << *this << " found a remote Writer with incompatible QoS: " <<
            utils::qos_policy_mask_to_string(qos));

    monitor_qos_mismatch(topic_);
    monitor_error("QOS_MISMATCH");
}

void CommonReader::on_sample_lost(
        fastdds::rtps::RTPSReader*,
        int32_t sample_lost_since_last_update) noexcept
{
    EPROSIMA_LOG_WARNING(DDSPIPE_RTPS_COMMONREADER_LISTENER,
            "SAMPLE_LOST | On reader " << *this << " a data sample was lost and will not be received");

    monitor_msg_lost(topic_, participant_id_);
}

void CommonReader::on_sample_rejected(
        fastdds::rtps::RTPSReader*,
        eprosima::fastdds::dds::SampleRejectedStatusKind reason,
        const fastdds::rtps::CacheChange_t* const change) noexcept
{
    std::string reason_str;
    switch (reason)
    {
        case eprosima::fastdds::dds::SampleRejectedStatusKind::NOT_REJECTED:
            reason_str = "NOT_REJECTED";
            break;
        case eprosima::fastdds::dds::SampleRejectedStatusKind::REJECTED_BY_INSTANCES_LIMIT:
            reason_str = "REJECTED_BY_INSTANCES_LIMIT";
            break;
        case eprosima::fastdds::dds::SampleRejectedStatusKind::REJECTED_BY_SAMPLES_LIMIT:
            reason_str = "REJECTED_BY_SAMPLES_LIMIT";
            break;
        case eprosima::fastdds::dds::SampleRejectedStatusKind::REJECTED_BY_SAMPLES_PER_INSTANCE_LIMIT:
            reason_str = "REJECTED_BY_SAMPLES_PER_INSTANCE_LIMIT";
            break;
        default:
            reason_str = "UNKNOWN";
            break;
    }
    EPROSIMA_LOG_INFO(DDSPIPE_RTPS_COMMONREADER_LISTENER,
            "Reader " << *this << " rejected a sample from " << change->writerGUID
                      << ". Reason: " << reason_str);
}

void CommonReader::on_incompatible_type(
        fastdds::rtps::RTPSReader* reader) noexcept
{
    EPROSIMA_LOG_WARNING(DDSPIPE_RTPS_COMMONREADER_LISTENER,
            "TOPIC_MISMATCH_TYPE | Reader " << *this <<
            " discovered a Writer with a matching Topic name but with an incompatible type");

    monitor_type_mismatch(topic_);
    monitor_error("TYPE_MISMATCH");
}

utils::ReturnCode CommonReader::is_data_correct_(
        const fastdds::rtps::CacheChange_t* received_change) const noexcept
{
    // Check that the guid is consistent
    if (received_change->writerGUID == fastdds::rtps::GUID_t::unknown())
    {
        EPROSIMA_LOG_WARNING(DDSPIPE_RTPS_COMMONREADER_LISTENER,
                "Error taking data without correct writer GUID.");

        return utils::ReturnCode::RETCODE_ERROR;
    }

    // Check that the data is consistent
    if (!(received_change->serializedPayload.max_size > 0))
    {
        // Data with 0 bytes is only correct if keyed topic and if data is being disposed
        if (!(topic_.topic_qos.keyed && received_change->kind != eprosima::fastdds::rtps::ChangeKind_t::ALIVE))
        {
            EPROSIMA_LOG_WARNING(DDSPIPE_RTPS_COMMONREADER_LISTENER,
                    "Error taking data with length " << received_change->serializedPayload.length << ".");

            return utils::ReturnCode::RETCODE_ERROR;
        }
    }

    return utils::ReturnCode::RETCODE_OK;
}

} /* namespace rtps */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
