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

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/qos/WriterQos.hpp>
#include <fastdds/dds/xtypes/type_representation/detail/dds_xtypes_typeobject.hpp>
#include <fastdds/rtps/builtin/data/TopicDescription.hpp>
#include <fastdds/rtps/common/CacheChange.hpp>
#include <fastdds/rtps/participant/RTPSParticipant.hpp>
#include <fastdds/rtps/RTPSDomain.hpp>

#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Log.hpp>
#include <cpp_utils/qos/qos_utils.hpp>
#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_participants/efficiency/cache_change/CacheChangePool.hpp>
#include <ddspipe_participants/types/dds/RouterCacheChange.hpp>
#include <ddspipe_participants/writer/rtps/CommonWriter.hpp>
#include <ddspipe_participants/writer/rtps/filter/RepeaterDataFilter.hpp>
#include <ddspipe_participants/writer/rtps/filter/SelfDataFilter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace rtps {

using namespace eprosima::ddspipe::core::types;
using eprosima::ddspipe::core::types::operator <<;

std::atomic<utils::Duration_ms> CommonWriter::wait_all_acked_timeout{0};

CommonWriter::CommonWriter(
        const ParticipantId& participant_id,
        const DdsTopic& topic,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        fastdds::rtps::RTPSParticipant* rtps_participant,
        const bool repeater,
        const fastdds::rtps::HistoryAttributes& history_attributes,
        const fastdds::rtps::WriterAttributes& writer_attributes,
        const fastdds::rtps::TopicDescription& topic_description,
        const fastdds::dds::WriterQos& writer_qos,
        const utils::PoolConfiguration& pool_configuration)
    : BaseWriter(participant_id, topic.topic_qos.max_tx_rate)
    , rtps_participant_(rtps_participant)
    , repeater_(repeater)
    , topic_(topic)
    , payload_pool_(payload_pool)
    , rtps_writer_(nullptr)
    , rtps_history_(nullptr)
    , history_attributes_(history_attributes)
    , writer_attributes_(writer_attributes)
    , topic_description_(topic_description)
    , writer_qos_(writer_qos)
    , pool_configuration_(pool_configuration)
{
    // Do nothing
}

CommonWriter::~CommonWriter()
{
    // This variables should be set, otherwise the creation should have fail
    // Anyway, the if case is used for safety reasons

    // Delete writer
    if (rtps_writer_)
    {
        // Unset listener before destruction (not necessary in principle, but just in case)
        rtps_writer_->set_listener(nullptr);

        if (wait_all_acked_timeout > 0)
        {
            rtps_writer_->wait_for_all_acked(wait_all_acked_timeout.load());
        }

        // Delete the CommonWriter the History is cleaned
        fastdds::rtps::RTPSDomain::removeRTPSWriter(rtps_writer_);
    }

    // Delete History
    if (rtps_history_)
    {
        delete rtps_history_;
    }

    EPROSIMA_LOG_INFO(DDSPIPE_RTPS_COMMONWRITER, "Deleting CommonWriter created in Participant " <<
            participant_id_ << " for topic " << topic_);
}

void CommonWriter::init()
{
    internal_entities_creation_(
        history_attributes_,
        writer_attributes_,
        topic_description_,
        writer_qos_,
        pool_configuration_);
}

void CommonWriter::on_writer_matched(
        fastdds::rtps::RTPSWriter*,
        const fastdds::rtps::MatchingInfo& info) noexcept
{
    if (!come_from_this_participant_(info.remoteEndpointGuid))
    {
        if (info.status == fastdds::rtps::MatchingStatus::MATCHED_MATCHING)
        {
            EPROSIMA_LOG_INFO(DDSPIPE_RTPS_COMMONWRITER_LISTENER,
                    "Writer " << *this << " in topic " << topic_.serialize() << " matched with a new Reader with guid " <<
                    info.remoteEndpointGuid);
        }
        else
        {
            EPROSIMA_LOG_INFO(DDSPIPE_RTPS_COMMONWRITER_LISTENER,
                    "Writer " << *this << " in topic " << topic_.serialize() << " unmatched with Reader " <<
                    info.remoteEndpointGuid);
        }
    }
}

void CommonWriter::on_writer_change_received_by_all(
        fastdds::rtps::RTPSWriter* /*writer*/,
        fastdds::rtps::CacheChange_t* change)
{
    if (writer_qos_.m_reliability.kind == fastdds::dds::BEST_EFFORT_RELIABILITY_QOS ||
            writer_qos_.m_durability.kind == fastdds::dds::VOLATILE_DURABILITY_QOS)
    {
        rtps_history_->remove_change_g(change);
    }
}

void CommonWriter::on_offered_incompatible_qos(
        fastdds::rtps::RTPSWriter*,
        fastdds::dds::PolicyMask qos) noexcept
{
    EPROSIMA_LOG_WARNING(DDSPIPE_RTPS_COMMONWRITER_LISTENER,
            "Writer " << *this << " found a remote Reader with incompatible QoS: " <<
            utils::qos_policy_mask_to_string(qos));
}

bool CommonWriter::come_from_this_participant_(
        const fastdds::rtps::GUID_t guid) const noexcept
{
    return guid.guidPrefix == rtps_writer_->getGuid().guidPrefix;
}

// Specific enable/disable do not need to be implemented
utils::ReturnCode CommonWriter::write_nts_(
        core::IRoutingData& data) noexcept
{
    auto& rtps_data = dynamic_cast<RtpsPayloadData&>(data);

    if (rtps_history_->isFull())
    {
        // Remove the oldest cache change when the max history size is reached.
        // NOTE: This should be done as a first step, otherwise the creation of a new change would fail.
        rtps_history_->remove_min_change();
    }

    // Take new Change from history
    fastdds::rtps::CacheChange_t* new_change;

    if (topic_.topic_qos.keyed)
    {
        new_change = rtps_history_->create_change(
            rtps_data.kind,
            rtps_data.instanceHandle);
    }
    else
    {
        new_change = rtps_history_->create_change(rtps_data.kind);
    }

    // If still is not able to get a change, return an error code
    if (!new_change)
    {
        return utils::ReturnCode::RETCODE_ERROR;
    }

    logDebug(DDSPIPE_RTPS_COMMONWRITER,
            "CommonWriter " << *this << " sending payload " << rtps_data.payload << " from " <<
            rtps_data.source_guid);

    // Get params to write (if set)
    fastdds::rtps::WriteParams write_params;

    // Fill cache change with specific data to send
    auto ret = fill_to_send_data_(new_change, write_params, rtps_data);
    if (ret != utils::ReturnCode::RETCODE_OK)
    {
        EPROSIMA_LOG_ERROR(DDSPIPE_RTPS_COMMONWRITER, "Error setting change to send.");
        return ret;
    }

    // Send data by adding it to CommonWriter History
    rtps_history_->add_change(new_change, write_params);

    // In the case of BEST_EFFORT, add_change calls on_writer_change_received_by_all (which removes the change).

    // At this point, write params is now the output of adding change
    fill_sent_data_(write_params, rtps_data);

    return utils::ReturnCode::RETCODE_OK;
}

utils::ReturnCode CommonWriter::fill_to_send_data_(
        fastdds::rtps::CacheChange_t* to_send_change_to_fill,
        fastdds::rtps::WriteParams& to_send_params,
        const RtpsPayloadData& data) const noexcept
{
    if (repeater_)
    {
        // Add origin to change in case the cache change is RouterCacheChange (only in repeater mode)
        core::types::RouterCacheChange& change_ref =
                static_cast<core::types::RouterCacheChange&>(*to_send_change_to_fill);
        change_ref.last_writer_guid_prefix = data.source_guid.guidPrefix;
    }

    // Set keys in case topic has keys
    if (topic_.topic_qos.keyed)
    {
        to_send_change_to_fill->instanceHandle = data.instanceHandle;
    }

    // Get the Payload without copy only if it has length
    if (data.payload.length > 0)
    {
        if (!payload_pool_->get_payload(
                    data.payload,
                    to_send_change_to_fill->serializedPayload))
        {
            logDevError(DDSPIPE_RTPS_COMMONWRITER, "Error getting Payload.");
            return utils::ReturnCode::RETCODE_ERROR;
        }
    }

    // Set Change kind
    to_send_change_to_fill->kind = data.kind;

    // Set source time stamp to be the original one
    to_send_params.source_timestamp(data.source_timestamp);

    // Set original writer guid
    to_send_params.original_writer_guid(data.original_source_guid);

    return utils::ReturnCode::RETCODE_OK;
}

void CommonWriter::fill_sent_data_(
        const fastdds::rtps::WriteParams& params,
        core::types::RtpsPayloadData& data_to_fill) const noexcept
{
    // Do nothing
}

void CommonWriter::internal_entities_creation_(
        const fastdds::rtps::HistoryAttributes& history_attributes,
        const fastdds::rtps::WriterAttributes& writer_attributes,
        const fastdds::rtps::TopicDescription& topic_description,
        const fastdds::dds::WriterQos& writer_qos,
        const utils::PoolConfiguration& pool_configuration)
{
    // Copy writer attributes because fast needs it non const (do not ask why)
    fastdds::rtps::WriterAttributes non_const_writer_attributes = writer_attributes;

    // Create CommonWriter
    // Listener must be set in creation as no callbacks should be missed
    // It is safe to do so here as object is already created and callbacks do not require anything set in this method
    if (repeater_)
    {
        // Create History
        rtps_history_ = new fastdds::rtps::WriterHistory(
            history_attributes,
            payload_pool_,
            std::make_shared<core::CacheChangePool>(pool_configuration));

        logDebug(DDSPIPE_RTPS_COMMONWRITER, "CommonWriter created with repeater filter");
    }
    else
    {
        // Create History
        rtps_history_ = new fastdds::rtps::WriterHistory(
            history_attributes,
            payload_pool_);
    }

    rtps_writer_ = fastdds::rtps::RTPSDomain::createRTPSWriter(
        rtps_participant_,
        non_const_writer_attributes,
        rtps_history_,
        this);

    if (!rtps_writer_)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Simple RTPSWriter for Participant " <<
                      participant_id_ << " in topic " << topic_ << ".");
    }

    // Register writer with topic
    if (!rtps_participant_->register_writer(rtps_writer_, topic_description, writer_qos))
    {
        // In case it fails, remove writer and throw exception
        fastdds::rtps::RTPSDomain::removeRTPSWriter(rtps_writer_);
        throw utils::InitializationException(utils::Formatter() << "Error registering topic " << topic_ <<
                      " for Simple RTPSWriter in Participant " << participant_id_);
    }

    if (repeater_)
    {
        // Use filter writer of origin
        data_filter_ = std::make_unique<RepeaterDataFilter>();
    }
    else
    {
        // Use default filter
        data_filter_ = std::make_unique<SelfDataFilter>();
    }

    rtps_writer_->reader_data_filter(data_filter_.get());

    EPROSIMA_LOG_INFO(
        DDSPIPE_RTPS_COMMONWRITER,
        "New CommonWriter created in Participant " << participant_id_ <<
            " for topic " << topic_ <<
            " with guid " << rtps_writer_->getGuid());
}

fastdds::rtps::HistoryAttributes CommonWriter::reckon_history_attributes_(
        const core::types::DdsTopic& topic) noexcept
{
    fastdds::rtps::HistoryAttributes att;

    att.memoryPolicy =
            fastdds::rtps::MemoryManagementPolicy_t::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;

    att.maximumReservedCaches = topic.topic_qos.history_depth;
    if (att.maximumReservedCaches > 0 && att.initialReservedCaches > att.maximumReservedCaches)
    {
        att.initialReservedCaches = att.maximumReservedCaches;
    }
    att.extraReservedCaches = 0;

    return att;
}

fastdds::rtps::WriterAttributes CommonWriter::reckon_writer_attributes_(
        const core::types::DdsTopic& topic) noexcept
{
    fastdds::rtps::WriterAttributes att;

    // Set Durability
    att.endpoint.durabilityKind = topic.topic_qos.durability_qos;

    // Set Reliability
    att.endpoint.reliabilityKind = topic.topic_qos.reliability_qos;

    // Set if topic has key
    if (topic.topic_qos.keyed)
    {
        att.endpoint.topicKind = fastdds::rtps::WITH_KEY;
    }
    else
    {
        att.endpoint.topicKind = fastdds::rtps::NO_KEY;
    }

    // Other attributes as partitions and ownership are not used in this writer

    // Set write mode
    // ATTENTION: Changing this will change the logic of removing changes added. Please be careful.
    att.mode = fastdds::rtps::RTPSWriterPublishMode::SYNCHRONOUS_WRITER;

    return att;
}

fastdds::rtps::TopicDescription CommonWriter::reckon_topic_description_(
        const core::types::DdsTopic& topic) noexcept
{
    fastdds::rtps::TopicDescription topic_description;

    topic_description.type_name = topic.type_name;
    topic_description.topic_name = topic.m_topic_name;

    // Check TypeIdentifierPair in DdsTopic is not empty
    fastdds::dds::xtypes::TypeIdentifierPair empty_type_identifiers;
    if (topic.type_identifiers == empty_type_identifiers)
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

fastdds::dds::WriterQos CommonWriter::reckon_writer_qos_(
        const core::types::DdsTopic& topic) noexcept
{
    fastdds::dds::WriterQos qos;

    // Set Durability
    qos.m_durability.kind =
            (topic.topic_qos.is_transient_local()
            ? fastdds::dds::DurabilityQosPolicyKind_t::TRANSIENT_LOCAL_DURABILITY_QOS
            : fastdds::dds::DurabilityQosPolicyKind_t::VOLATILE_DURABILITY_QOS);

    // Set Reliability
    qos.m_reliability.kind =
            (topic.topic_qos.is_reliable()
            ? fastdds::dds::ReliabilityQosPolicyKind::RELIABLE_RELIABILITY_QOS
            : fastdds::dds::ReliabilityQosPolicyKind::BEST_EFFORT_RELIABILITY_QOS);

    // Set minimum deadline so it matches with everything
    qos.m_deadline.period = fastdds::dds::Duration_t(0);

    // Partitions and specific ownership strength are not set in common.

    return qos;
}

utils::PoolConfiguration CommonWriter::reckon_cache_change_pool_configuration_(
        const core::types::DdsTopic& topic) noexcept
{
    utils::PoolConfiguration config;
    config.maximum_size = topic.topic_qos.history_depth;
    config.initial_size = 20;
    config.batch_size = 20;

    return config;
}

} /* namespace rtps */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
