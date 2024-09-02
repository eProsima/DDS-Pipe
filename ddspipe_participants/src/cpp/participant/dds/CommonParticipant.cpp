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

#include <fastdds/rtps/transport/TCPv4TransportDescriptor.hpp>
#include <fastdds/rtps/transport/TCPv6TransportDescriptor.hpp>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.hpp>
#include <fastdds/rtps/transport/UDPv6TransportDescriptor.hpp>

#include <fastdds/dds/core/ReturnCode.hpp>
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
    if (nullptr != dds_participant_)
    {
        // Delete DDS entities contained within the DomainParticipant
        dds_participant_->delete_contained_entities();

        // Delete DomainParticipant
        eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(dds_participant_);
    }
}

void CommonParticipant::init()
{
    EPROSIMA_LOG_INFO(DDSPIPE_DDS_PARTICIPANT, "Initializing DDS Participant " << id() << ".");

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

    if (!dds_participant_)
    {
        throw utils::InitializationException(STR_ENTRY << "Error creating DDS Participant " << id() << ".");
    }

    if (dds_participant_->enable() != fastdds::dds::RETCODE_OK)
    {
        throw utils::InitializationException(STR_ENTRY << "Error enabling DDS Participant " << id() << ".");
    }

    EPROSIMA_LOG_INFO(DDSPIPE_RTPS_PARTICIPANT,
            "New Participant created with id " << this->id() <<
            " in domain " << domain_id_ << " with guid " << dds_participant_->guid() <<
            (this->is_repeater() ? " (repeater)" : " (non repeater)"));
}

template<>
DDSPIPE_PARTICIPANTS_DllAPI
std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor>
CommonParticipant::create_descriptor(
        std::set<types::IpType> whitelist)
{
    std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
            std::make_shared<eprosima::fastdds::rtps::UDPv4TransportDescriptor>();

    for (const types::IpType& ip : whitelist)
    {
        if (types::Address::is_ipv4_correct(ip))
        {
            udp_transport->interfaceWhiteList.emplace_back(ip);
            EPROSIMA_LOG_INFO(DDSPIPE_COMMON_PARTICIPANT,
                    "Adding " << ip << " to UDP whitelist interfaces.");
        }
        else
        {
            // Invalid address, continue with next one
            EPROSIMA_LOG_WARNING(DDSPIPE_COMMON_PARTICIPANT,
                    "Not valid IPv4. Discarding UDP whitelist interface " << ip << ".");
        }
    }

    return udp_transport;
}

template<>
DDSPIPE_PARTICIPANTS_DllAPI
std::shared_ptr<eprosima::fastdds::rtps::UDPv6TransportDescriptor>
CommonParticipant::create_descriptor(
        std::set<types::IpType> whitelist)
{
    std::shared_ptr<eprosima::fastdds::rtps::UDPv6TransportDescriptor> udp_transport =
            std::make_shared<eprosima::fastdds::rtps::UDPv6TransportDescriptor>();

    for (const types::IpType& ip : whitelist)
    {
        if (types::Address::is_ipv6_correct(ip))
        {
            udp_transport->interfaceWhiteList.emplace_back(ip);
            EPROSIMA_LOG_INFO(DDSPIPE_COMMON_PARTICIPANT,
                    "Adding " << ip << " to UDP whitelist interfaces.");
        }
        else
        {
            // Invalid address, continue with next one
            EPROSIMA_LOG_WARNING(DDSPIPE_COMMON_PARTICIPANT,
                    "Not valid IPv6. Discarding UDP whitelist interface " << ip << ".");
        }
    }

    return udp_transport;
}

template<>
DDSPIPE_PARTICIPANTS_DllAPI
std::shared_ptr<eprosima::fastdds::rtps::TCPv4TransportDescriptor>
CommonParticipant::create_descriptor(
        std::set<types::IpType> whitelist)
{
    std::shared_ptr<eprosima::fastdds::rtps::TCPv4TransportDescriptor> tcp_transport =
            std::make_shared<eprosima::fastdds::rtps::TCPv4TransportDescriptor>();

    for (const types::IpType& ip : whitelist)
    {
        if (types::Address::is_ipv4_correct(ip))
        {
            tcp_transport->interfaceWhiteList.emplace_back(ip);
            EPROSIMA_LOG_INFO(DDSPIPE_COMMON_PARTICIPANT,
                    "Adding " << ip << " to TCP whitelist interfaces.");
        }
        else
        {
            // Invalid address, continue with next one
            EPROSIMA_LOG_WARNING(DDSPIPE_COMMON_PARTICIPANT,
                    "Not valid IPv4. Discarding TCP whitelist interface " << ip << ".");
        }
    }

    return tcp_transport;
}

template<>
DDSPIPE_PARTICIPANTS_DllAPI
std::shared_ptr<eprosima::fastdds::rtps::TCPv6TransportDescriptor>
CommonParticipant::create_descriptor(
        std::set<types::IpType> whitelist)
{
    std::shared_ptr<eprosima::fastdds::rtps::TCPv6TransportDescriptor> tcp_transport =
            std::make_shared<eprosima::fastdds::rtps::TCPv6TransportDescriptor>();

    for (const types::IpType& ip : whitelist)
    {
        if (types::Address::is_ipv6_correct(ip))
        {
            tcp_transport->interfaceWhiteList.emplace_back(ip);
            EPROSIMA_LOG_INFO(DDSPIPE_COMMON_PARTICIPANT,
                    "Adding " << ip << " to TCP whitelist interfaces.");
        }
        else
        {
            // Invalid address, continue with next one
            EPROSIMA_LOG_WARNING(DDSPIPE_COMMON_PARTICIPANT,
                    "Not valid IPv6. Discarding TCP whitelist interface " << ip << ".");
        }
    }

    return tcp_transport;
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

    fastdds::dds::Topic* fastdds_topic;
    try
    {
        // Get the DDS Topic associated (create it if it does not exist)
        fastdds_topic = topic_related_(dds_topic);
    }
    catch (const utils::InitializationException& e)
    {
        EPROSIMA_LOG_WARNING(
            DDSPIPE_DDS_PARTICIPANT,
            e.what()
                << " Execution continue but this topic will not be published in Participant " << id() << ".");
        return std::make_shared<BlankWriter>();
    }

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
    fastdds::dds::Topic* fastdds_topic;
    try
    {
        // Get the DDS Topic associated (create it if it does not exist)
        fastdds_topic = topic_related_(dds_topic);
    }
    catch (const utils::InitializationException& e)
    {
        EPROSIMA_LOG_WARNING(
            DDSPIPE_DDS_PARTICIPANT,
            e.what()
                << ". Execution continue but this topic will not be subscribed in Participant " << id() << ".");
        return std::make_shared<BlankReader>();
    }

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

void CommonParticipant::on_participant_discovery(
        fastdds::dds::DomainParticipant* participant,
        fastdds::rtps::ParticipantDiscoveryStatus reason,
        const fastdds::rtps::ParticipantBuiltinTopicData& info,
        bool& /*should_be_ignored*/)
{
    if (info.guid.guidPrefix != participant->guid().guidPrefix)
    {
        if (reason == fastdds::rtps::ParticipantDiscoveryStatus::DISCOVERED_PARTICIPANT)
        {
            EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                    "Found in Participant " << configuration_->id << " new Participant " << info.guid << ".");
        }
        else if (reason == fastdds::rtps::ParticipantDiscoveryStatus::CHANGED_QOS_PARTICIPANT)
        {
            EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                    configuration_->id << " participant : " << "Participant " << info.guid <<
                    " changed QoS.");
        }
        else if (reason == fastdds::rtps::ParticipantDiscoveryStatus::REMOVED_PARTICIPANT)
        {
            EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                    configuration_->id << " participant : " << "Participant " << info.guid << " removed.");
        }
        else if (reason == fastdds::rtps::ParticipantDiscoveryStatus::DROPPED_PARTICIPANT)
        {
            EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                    configuration_->id << " participant : " << "Participant " << info.guid << " dropped.");
        }
        else if (reason == fastdds::rtps::ParticipantDiscoveryStatus::IGNORED_PARTICIPANT)
        {
            EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                    configuration_->id << " participant : " << "Participant " << info.guid << " ignored.");
        }
    }
}

void CommonParticipant::on_data_reader_discovery(
        fastdds::dds::DomainParticipant*,
        fastdds::rtps::ReaderDiscoveryStatus reason,
        const fastdds::dds::SubscriptionBuiltinTopicData& info,
        bool& /*should_be_ignored*/)
{
    // If reader is from other participant, store it in discovery database
    if (detail::come_from_same_participant_(info.guid, this->dds_participant_->guid()))
    {
        // Come from this participant, do nothing
        return;
    }

    // Calculate endpoint info
    core::types::Endpoint info_reader =
            detail::create_endpoint_from_info_<fastdds::dds::SubscriptionBuiltinTopicData>(info, id());

    // If new endpoint discovered
    if (reason == fastdds::rtps::ReaderDiscoveryStatus::DISCOVERED_READER)
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                "Found in Participant " << configuration_->id << " new Reader " << info.guid << ".");

        // TODO check logic because if an endpoint is lost by liveliness it may be inserted again when already in database
        this->discovery_database_->add_endpoint(info_reader);
    }
    else if (reason == fastdds::rtps::ReaderDiscoveryStatus::CHANGED_QOS_READER)
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                configuration_->id << " participant : " << "Reader " << info.guid << " changed TopicQoS.");

        this->discovery_database_->update_endpoint(info_reader);
    }
    else if (reason == fastdds::rtps::ReaderDiscoveryStatus::REMOVED_READER)
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                configuration_->id << " participant : " << "Reader " << info.guid << " removed.");

        info_reader.active = false;
        this->discovery_database_->update_endpoint(info_reader);
    }
    else if (reason == fastdds::rtps::ReaderDiscoveryStatus::IGNORED_READER)
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                configuration_->id << " participant : " << "Reader " << info.guid << " ignored.");

        // Do not notify discovery database (design choice that might be changed in the future)
    }
}

void CommonParticipant::on_data_writer_discovery(
        fastdds::dds::DomainParticipant*,
        fastdds::rtps::WriterDiscoveryStatus reason,
        const fastdds::dds::PublicationBuiltinTopicData& info,
        bool& /*should_be_ignored*/)
{
    // If writer is from other participant, store it in discovery database
    if (detail::come_from_same_participant_(info.guid, this->dds_participant_->guid()))
    {
        // Come from this participant, do nothing
        return;
    }

    // Calculate endpoint info
    core::types::Endpoint info_writer =
            detail::create_endpoint_from_info_<fastdds::dds::PublicationBuiltinTopicData>(info, id());

    // If new endpoint discovered
    if (reason == fastdds::rtps::WriterDiscoveryStatus::DISCOVERED_WRITER)
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                "Found in Participant " << configuration_->id << " new Writer " << info.guid << ".");

        // TODO check logic because if an endpoint is lost by liveliness it may be inserted again when already in database
        this->discovery_database_->add_endpoint(info_writer);
    }
    else if (reason == fastdds::rtps::WriterDiscoveryStatus::CHANGED_QOS_WRITER)
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                configuration_->id << " participant : " << "Writer " << info.guid << " changed TopicQoS.");

        this->discovery_database_->update_endpoint(info_writer);
    }
    else if (reason == fastdds::rtps::WriterDiscoveryStatus::REMOVED_WRITER)
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                configuration_->id << " participant : " << "Writer " << info.guid << " removed.");

        info_writer.active = false;
        this->discovery_database_->update_endpoint(info_writer);
    }
    else if (reason == fastdds::rtps::WriterDiscoveryStatus::IGNORED_WRITER)
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                configuration_->id << " participant : " << "Writer " << info.guid << " ignored.");

        // Do not notify discovery database (design choice that might be changed in the future)
    }
}

CommonParticipant::CommonParticipant(
        const std::shared_ptr<SimpleParticipantConfiguration>& participant_configuration,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        const std::shared_ptr<core::DiscoveryDatabase>& discovery_database,
        const core::types::DomainId& domain_id,
        const fastdds::dds::DomainParticipantQos& participant_qos)
    : configuration_(participant_configuration)
    , payload_pool_(payload_pool)
    , discovery_database_(discovery_database)
    , domain_id_(domain_id)
    , participant_qos_(participant_qos)
{
    // Do nothing
}

fastdds::dds::DomainParticipantQos CommonParticipant::add_qos_properties_(
        fastdds::dds::DomainParticipantQos& qos) const
{
    // Enforce ignore local endpoints on XML participants
    qos.properties().properties().emplace_back(
        "fastdds.ignore_local_endpoints",
        "true");

    // Set app properties
    qos.properties().properties().emplace_back(
        "fastdds.application.id",
        participant_configuration->app_id,
        "true");
    qos.properties().properties().emplace_back(
        "fastdds.application.metadata",
        participant_configuration->app_metadata,
        "true");

    return qos;
}

fastdds::dds::DomainParticipantQos CommonParticipant::reckon_participant_qos_() const
{
    auto qos = fastdds::dds::DomainParticipantFactory::get_instance()->get_default_participant_qos();

    add_qos_properties_(qos);

    return qos;
}

fastdds::dds::DomainParticipant* CommonParticipant::create_dds_participant_()
{
    // Set listener mask so reader read its own messages
    fastdds::dds::StatusMask mask;
    mask << fastdds::dds::StatusMask::publication_matched();
    mask << fastdds::dds::StatusMask::subscription_matched();
    EPROSIMA_LOG_INFO(DDSPIPE_RTPS_PARTICIPANT,
            "Creating Participant in domain " << domain_id_);

    return eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(
        domain_id_,
        participant_qos_,
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
                    payload_pool_,
                    topic.type_name,
                    topic.type_identifiers,
                    topic.topic_qos.keyed))
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
