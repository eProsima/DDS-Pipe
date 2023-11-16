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

#include <memory>

#include <cpp_utils/Log.hpp>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <ddspipe_core/types/dynamic_types/types.hpp>
#include <ddspipe_core/types/data/RtpsPayloadData.hpp>

#include <ddspipe_participants/participant/rtps/SimpleParticipant.hpp>
#include <ddspipe_participants/participant/dds/CommonParticipant.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>
#include <ddspipe_participants/writer/dds/MultiWriter.hpp>
#include <ddspipe_participants/writer/dds/SimpleWriter.hpp>
#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>
#include <ddspipe_participants/reader/dds/SimpleReader.hpp>
#include <ddspipe_participants/reader/dds/SpecificQoSReader.hpp>
#include <ddspipe_participants/types/dds/TopicDataType.hpp>

#include <utils/utils.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;

CommonParticipant::~CommonParticipant()
{
    // In case init has been done, remove everything
    if (dds_participant_)
    {
        dds_participant_->set_listener(nullptr);

        for (auto& topic : dds_topics_)
        {
            dds_participant_->delete_topic(topic.second);
        }

        eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(dds_participant_);
    }
}

void CommonParticipant::init()
{
    logInfo(DDSPIPE_DDS_PARTICIPANT, "Initializing DDS Participant " << id() << ".");

    // Force DDS entities to be created disabled
    // NOTE: this is very dangerous because we are modifying a global variable (and a not thread safe one) in a
    // local function.
    // However, this is required, otherwise we could fail in two points:
    // - receive in this object, maybe in same thread a discovery callback, which could use this variable
    //    (e.g to check if the Participant called is this one)
    // - lose a discovery callback
    fastdds::dds::DomainParticipantFactoryQos original_fact_qos;
    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->get_qos(
        original_fact_qos);

    fastdds::dds::DomainParticipantFactoryQos fact_qos;
    fact_qos.entity_factory().autoenable_created_entities = false;
    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->set_qos(
        fact_qos);

    // CREATE THE PARTICIPANT
    dds_participant_ = create_dds_participant_();

    // Restore default DomainParticipantQoS (create enabled entities) after creating and enabling this participant
    // WARNING: not thread safe at the moment of this writing, see note above.
    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->set_qos(
        original_fact_qos);

    if (dds_participant_ == nullptr)
    {
        throw utils::InitializationException(STR_ENTRY << "Error creating DDS Participant " << id() << ".");
    }

    if (dds_participant_->enable() != utils::ReturnCode::RETCODE_OK)
    {
        throw utils::InitializationException(STR_ENTRY << "Error enabling DDS Participant " << id() << ".");
    }
}

core::types::ParticipantId CommonParticipant::id() const noexcept
{
    return configuration_->id;
}

bool CommonParticipant::is_rtps_kind() const noexcept
{
    return false;
}

bool CommonParticipant::is_repeater() const noexcept
{
    return false;
}

core::types::TopicQoS CommonParticipant::topic_qos() const noexcept
{
    return configuration_->topic_qos;
}

std::shared_ptr<core::IWriter> CommonParticipant::create_writer(
        const core::ITopic& topic)
{
    // Can only create DDS Topics
    const core::types::DdsTopic* topic_ptr = dynamic_cast<const core::types::DdsTopic*>(&topic);
    if (!topic_ptr)
    {
        logDebug(DDSPIPE_DDS_PARTICIPANT, "Not creating Writer for topic " << topic.topic_name());
        return std::make_shared<BlankWriter>();
    }
    const core::types::DdsTopic& dds_topic = *topic_ptr;

    // Check that it is RTPS topic
    if (dds_topic.internal_type_discriminator() != core::types::INTERNAL_TOPIC_TYPE_RTPS)
    {
        logDebug(DDSPIPE_DDS_PARTICIPANT, "Not creating Writer for non RTPS topic " << dds_topic.topic_name());
        return std::make_shared<BlankWriter>();
    }

    // Get the DDS Topic associated (create it if it does not exist)
    fastdds::dds::Topic* fastdds_topic = topic_related_(dds_topic);

    if (dds_topic.topic_qos.has_partitions() || dds_topic.topic_qos.has_ownership())
    {
        // Notice that MultiWriter does not require an init call
        return std::make_shared<MultiWriter>(
            this->id(),
            dds_topic,
            this->payload_pool_,
            dds_participant_,
            fastdds_topic);
    }
    else
    {
        auto writer = std::make_shared<SimpleWriter>(
            this->id(),
            dds_topic,
            this->payload_pool_,
            dds_participant_,
            fastdds_topic);
        writer->init();

        return writer;
    }
}

std::shared_ptr<core::IReader> CommonParticipant::create_reader(
        const core::ITopic& topic)
{
    // Can only create DDS Topics
    const core::types::DdsTopic* topic_ptr = dynamic_cast<const core::types::DdsTopic*>(&topic);

    if (!topic_ptr)
    {
        logDebug(DDSPIPE_DDS_PARTICIPANT, "Not creating Reader for topic " << topic.topic_name());
        return std::make_shared<BlankReader>();
    }

    const core::types::DdsTopic& dds_topic = *topic_ptr;

    // Check that it is RTPS topic
    if (dds_topic.internal_type_discriminator() != core::types::INTERNAL_TOPIC_TYPE_RTPS)
    {
        logDebug(DDSPIPE_DDS_PARTICIPANT, "Not creating Reader for non RTPS topic " << dds_topic.topic_name());
        return std::make_shared<BlankReader>();
    }

    // Get the DDS Topic associated (create it if it does not exist)
    fastdds::dds::Topic* fastdds_topic = topic_related_(dds_topic);

    if (dds_topic.topic_qos.has_partitions() || dds_topic.topic_qos.has_ownership())
    {
        // Notice that MultiReader does not require an init call
        auto reader = std::make_shared<SpecificQoSReader>(
            this->id(),
            dds_topic,
            this->payload_pool_,
            dds_participant_,
            fastdds_topic,
            discovery_database_);
        reader->init();

        return reader;
    }
    else
    {
        auto reader = std::make_shared<SimpleReader>(
            this->id(),
            dds_topic,
            this->payload_pool_,
            dds_participant_,
            fastdds_topic);
        reader->init();

        return reader;
    }
}

void CommonParticipant::on_subscriber_discovery(
        fastdds::dds::DomainParticipant*,
        fastrtps::rtps::ReaderDiscoveryInfo&& info)
{
    // If reader is from other participant, store it in discovery database
    if (detail::come_from_same_participant_(info.info.guid(), this->dds_participant_->guid()))
    {
        // Come from this participant, do nothing
        return;
    }

    // Calculate endpoint info
    core::types::Endpoint info_reader =
            detail::create_endpoint_from_info_<fastrtps::rtps::ReaderDiscoveryInfo>(info, id());

    // If new endpoint discovered
    if (info.status == fastrtps::rtps::ReaderDiscoveryInfo::DISCOVERY_STATUS::DISCOVERED_READER)
    {
        // TODO check logic because if an endpoint is lost by liveliness it may be inserted again when already in database
        this->discovery_database_->add_endpoint(info_reader);
    }
    else if (info.status == fastrtps::rtps::ReaderDiscoveryInfo::DISCOVERY_STATUS::CHANGED_QOS_READER)
    {
        this->discovery_database_->update_endpoint(info_reader);
    }
    else
    {
        info_reader.active = false;
        this->discovery_database_->update_endpoint(info_reader);
    }
}

void CommonParticipant::on_publisher_discovery(
        fastdds::dds::DomainParticipant*,
        fastrtps::rtps::WriterDiscoveryInfo&& info)
{
    // If writer is from other participant, store it in discovery database
    if (detail::come_from_same_participant_(info.info.guid(), this->dds_participant_->guid()))
    {
        // Come from this participant, do nothing
        return;
    }

    // Calculate endpoint info
    core::types::Endpoint info_writer =
            detail::create_endpoint_from_info_<fastrtps::rtps::WriterDiscoveryInfo>(info, id());

    // If new endpoint discovered
    if (info.status == fastrtps::rtps::WriterDiscoveryInfo::DISCOVERY_STATUS::DISCOVERED_WRITER)
    {
        // TODO check logic because if an endpoint is lost by liveliness it may be inserted again when already in database
        this->discovery_database_->add_endpoint(info_writer);
    }
    else
    {
        this->discovery_database_->update_endpoint(info_writer);
    }
}

CommonParticipant::CommonParticipant(
        const std::shared_ptr<SimpleParticipantConfiguration>& participant_configuration,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        const std::shared_ptr<core::DiscoveryDatabase>& discovery_database)
    : configuration_(participant_configuration)
    , payload_pool_(payload_pool)
    , discovery_database_(discovery_database)
{
    // Do nothing
}

fastdds::dds::DomainParticipantQos CommonParticipant::reckon_participant_qos_() const
{
    auto qos = fastdds::dds::DomainParticipantFactory::get_instance()->get_default_participant_qos();

    qos.properties().properties().emplace_back(
        "fastdds.ignore_local_endpoints",
        "true");

    // Set app properties
    qos.properties().properties().emplace_back(
        "fastdds.application.id",
        configuration_->app_id,
        "true");
    qos.properties().properties().emplace_back(
        "fastdds.application.metadata",
        configuration_->app_metadata,
        "true");

    return qos;
}

fastdds::dds::DomainParticipant* CommonParticipant::create_dds_participant_()
{
    // Set listener mask so reader read its own messages
    fastdds::dds::StatusMask mask;
    mask << fastdds::dds::StatusMask::publication_matched();
    mask << fastdds::dds::StatusMask::subscription_matched();

    return eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(
        configuration_->domain,
        reckon_participant_qos_(),
        this,
        mask);
}

fastdds::dds::Topic* CommonParticipant::topic_related_(
        const core::types::DdsTopic& topic)
{
    // Lock access to topics map
    std::lock_guard<TopicsMapType> _(dds_topics_);

    // Check if topic already exist
    auto it = dds_topics_.find(topic);
    if (it != dds_topics_.end())
    {
        return it->second;
    }

    // If type is not registered, register it
    if (type_names_registered_.find(topic.type_name) == type_names_registered_.end())
    {
        logDebug(DDSPIPE_DDS_PARTICIPANT, "Registering type "
                << topic.type_name << " in dds participant "
                << id() << ".");

        dds_participant_->register_type(
            eprosima::fastdds::dds::TypeSupport(
                new TopicDataType(
                    topic.type_name,
                    topic.topic_qos.keyed,
                    payload_pool_))
            );
    }

    logDebug(DDSPIPE_DDS_PARTICIPANT, "Creating topic "
            << topic.m_topic_name << " and type "
            << topic.type_name << " in dds participant "
            << id() << ".");

    // Create the new topic
    fastdds::dds::Topic* dds_topic = dds_participant_->create_topic(
        topic.m_topic_name,
        topic.type_name,
        dds_participant_->get_default_topic_qos()
        );

    if (dds_topic == nullptr)
    {
        throw utils::InitializationException(STR_ENTRY
                      << "Error creating DDS Topic " << topic << ".");
    }

    // Store topic in map
    dds_topics_[topic] = dds_topic;

    return dds_topic;
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
