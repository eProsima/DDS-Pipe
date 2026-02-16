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


#include <fastdds/rtps/common/CacheChange.hpp>
#include <fastdds/rtps/common/WriteParams.hpp>
#include <fastdds/rtps/participant/RTPSParticipant.hpp>
#include <fastdds/rtps/RTPSDomain.hpp>

#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Log.hpp>
#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_participants/efficiency/cache_change/CacheChangePool.hpp>
#include <ddspipe_participants/types/dds/RouterCacheChange.hpp>
#include <ddspipe_participants/writer/dds/CommonWriter.hpp>
#include <ddspipe_participants/writer/dds/filter/RepeaterDataFilter.hpp>
#include <ddspipe_participants/writer/dds/filter/SelfDataFilter.hpp>

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
    if (dds_publisher_)
    {
        if (writer_)
        {
            writer_->set_listener(nullptr);
            dds_publisher_->delete_datawriter(writer_);
        }

        dds_participant_->delete_publisher(dds_publisher_);
    }

    EPROSIMA_LOG_INFO(DDSPIPE_DDS_WRITER, "Deleting CommonWriter created in Participant "
            << participant_id_ << " for topic " << topic_);
}

void CommonWriter::init(
        const std::set<std::string>& partitions_set,
        const std::string& content_topicfilter_expression
)
{
    EPROSIMA_LOG_INFO(DDSPIPE_DDS_WRITER,
            "Initializing writer in " << participant_id_ << " for topic " << topic_ << ".");

    // Create publisher
    dds_publisher_ = dds_participant_->create_publisher(
        reckon_publisher_qos_()
        );

    if (!dds_publisher_)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Publisher for Participant "
                                     << participant_id_ << " in topic " << topic_ << ".");
    }

    // If the reader has an active filter
    // change the qos partition before creating the datareader.
    if (partitions_set.size() > 0)
    {
        // Get the current subscriber qos
        fastdds::dds::PublisherQos pub_qos = dds_publisher_->get_qos();
        // Remove all partitions from the qos
        pub_qos.partition().clear();
        // Add the filter partitions
        for (const std::string& partition: partitions_set)
        {
            pub_qos.partition().push_back(partition.c_str());
        }
        // Update the subscriber qos
        dds_publisher_->set_qos(pub_qos);
    }

    auto topic_tmp = dds_participant_->find_topic(topic_.topic_name(), 10);

    // Create the filtered topic with the expression given
    // if any filter is active the expression will be ""
    // no contentfilter is being applied
    filtered_topic_ = dds_participant_->create_contentfilteredtopic(
        topic_.topic_name() + "_filtered", topic_tmp,
        content_topicfilter_expression, {});
    if (nullptr == filtered_topic_)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating ContenTopicFilter for Participant "
                                     << participant_id_ << " in topic " << topic_ << ".");
    }

    writer_ = dds_publisher_->create_datawriter(
        dds_topic_, //filtered_topic_, //TODO. danip
        reckon_writer_qos_(),
        nullptr,
        eprosima::fastdds::dds::StatusMask::all(),
        payload_pool_);

    if (!writer_)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating DataWriter for Participant "
                                     << participant_id_ << " in topic " << topic_ << ".");
    }

    if (repeater_)
    {
        // Use filter writer of origin
        data_filter_ = std::make_shared<RepeaterDataFilter>();
    }
    else
    {
        // Use default filter
        data_filter_ = std::make_shared<SelfDataFilter>();
    }

    if (fastdds::dds::RETCODE_OK != writer_->set_sample_prefilter(data_filter_))
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error setting DataWriter prefilter for Participant "
                                     << participant_id_ << " in topic " << topic_ << ".");
    }
}

CommonWriter::CommonWriter(
        const ParticipantId& participant_id,
        const DdsTopic& topic,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        fastdds::dds::DomainParticipant* participant,
        fastdds::dds::Topic* topic_entity,
        const bool repeater)
    : BaseWriter(participant_id, topic.topic_qos.max_tx_rate)
    , dds_participant_(participant)
    , dds_topic_(topic_entity)
    , payload_pool_(new core::PayloadPoolMediator(payload_pool))
    , topic_(topic)
    , repeater_(repeater)
    , dds_publisher_(nullptr)
    , writer_(nullptr)
{
    // Do nothing
}

// Specific enable/disable do not need to be implemented
utils::ReturnCode CommonWriter::write_nts_(
        core::IRoutingData& data) noexcept
{
    EPROSIMA_LOG_INFO(DDSPIPE_DDS_WRITER, "Writing data in " << participant_id_ << " for topic " << topic_ << ".");

    auto& rtps_data = dynamic_cast<core::types::RtpsPayloadData&>(data);

    fastdds::rtps::WriteParams wparams;

    if (fill_to_send_data_(wparams, rtps_data) != utils::ReturnCode::RETCODE_OK)
    {
        EPROSIMA_LOG_ERROR(DDSPIPE_DDS_WRITER, "Error setting data to send.");
        return utils::ReturnCode::RETCODE_ERROR;
    }

    // WARNING: At the time of this writing, there is no DataWriter API to write both with parameters and instance handle.
    // However, the current implementation of write without instance handle always computes its value via type support
    // (if it corresponds to a keyed topic), so it is equivalent to write with instance handle (and can hence use the
    // write with params overload to cover all cases). Future developers should be aware of this and might need to
    // update this method if the DataWriter implementation changes at some point.
    return payload_pool_->write(writer_, &rtps_data, wparams);

    // TODO: handle dipose case -> DataWriter::write will always send ALIVE changes, so this case must be handled
    // with additional logic (e.g. by using unregister_instance instead of write).
}

void CommonWriter::update_partitions(
        const std::set<std::string>& partitions_set)
{
    fastdds::dds::PublisherQos pub_qos = dds_publisher_->get_qos();
    pub_qos.partition().clear();
    for (const std::string& partition: partitions_set)
    {
        pub_qos.partition().push_back(partition.c_str());
    }

    dds_publisher_->set_qos(pub_qos);
}

void CommonWriter::update_content_topic_filter(
        const std::string& expression)
{
    // content_topicfilter
    filtered_topic_->set_filter_expression(expression, {});
}

void CommonWriter::update_topic_partitions(
        const std::map<std::string, std::string>& partition_name)
{
    topic_.partition_name = partition_name;
}

fastdds::dds::PublisherQos CommonWriter::reckon_publisher_qos_() const noexcept
{
    // NOTE: this does not handle different partitions
    fastdds::dds::PublisherQos qos = dds_participant_->get_default_publisher_qos();

    if (topic_.topic_qos.has_partitions())
    {
        // NOTE: this should not happen as a topic with partitions should use QoSSpecificWriter
        qos.partition().push_back("*");
    }

    return qos;
}

fastdds::dds::DataWriterQos CommonWriter::reckon_writer_qos_() const noexcept
{
    fastdds::dds::DataWriterQos qos = dds_publisher_->get_default_datawriter_qos();

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

    // Set minimum deadline so it matches with everything
    qos.deadline().period = eprosima::fastdds::dds::Duration_t(0);

    return qos;
}

utils::ReturnCode CommonWriter::fill_to_send_data_(
        fastdds::rtps::WriteParams& to_send_params,
        const RtpsPayloadData& data) const noexcept
{
    if (repeater_)
    {
        auto write_data = std::make_shared<RepeaterWriteData>();
        write_data->last_writer_guid_prefix = data.source_guid.guidPrefix;
        to_send_params.user_write_data(write_data);
    }

    // Set source time stamp to be the original one
    to_send_params.source_timestamp(data.source_timestamp);

    // Set original writer guid
    to_send_params.original_writer_info(data.original_writer_info);

    return utils::ReturnCode::RETCODE_OK;
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
