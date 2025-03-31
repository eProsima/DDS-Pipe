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


#include <memory>

#include <fastdds/rtps/transport/TCPv4TransportDescriptor.hpp>
#include <fastdds/rtps/transport/TCPv6TransportDescriptor.hpp>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.hpp>
#include <fastdds/rtps/transport/UDPv6TransportDescriptor.hpp>
#include <fastdds/rtps/participant/RTPSParticipant.hpp>
#include <fastdds/rtps/RTPSDomain.hpp>

#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Log.hpp>
#include <cpp_utils/utils.hpp>

#include <ddspipe_core/types/data/RpcPayloadData.hpp>
#include <ddspipe_core/types/data/RtpsPayloadData.hpp>
#include <ddspipe_core/types/dds/DomainId.hpp>
#include <ddspipe_core/types/topic/rpc/RpcTopic.hpp>

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/participant/rtps/CommonParticipant.hpp>
#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>
#include <ddspipe_participants/reader/rpc/SimpleReader.hpp>
#include <ddspipe_participants/reader/rtps/SimpleReader.hpp>
#include <ddspipe_participants/reader/rtps/SpecificQoSReader.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>
#include <ddspipe_participants/writer/rpc/SimpleWriter.hpp>
#include <ddspipe_participants/writer/rtps/MultiWriter.hpp>
#include <ddspipe_participants/writer/rtps/QoSSpecificWriter.hpp>
#include <ddspipe_participants/writer/rtps/SimpleWriter.hpp>

#include <utils/utils.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace rtps {

CommonParticipant::CommonParticipant(
        const std::shared_ptr<ParticipantConfiguration>& participant_configuration,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        const std::shared_ptr<core::DiscoveryDatabase>& discovery_database,
        const core::types::DomainId& domain_id)
    : configuration_(participant_configuration)
    , payload_pool_(payload_pool)
    , discovery_database_(discovery_database)
    , domain_id_(domain_id)
{
    // Do nothing
}

CommonParticipant::~CommonParticipant()
{
    if (rtps_participant_)
    {
        fastdds::rtps::RTPSDomain::removeRTPSParticipant(rtps_participant_);
    }
}

void CommonParticipant::init()
{
    participant_attributes_ = reckon_participant_attributes_();
    create_participant_(
        domain_id_,
        participant_attributes_);
}

void CommonParticipant::on_participant_discovery(
        fastdds::rtps::RTPSParticipant* participant,
        fastdds::rtps::ParticipantDiscoveryStatus reason,
        const fastdds::rtps::ParticipantBuiltinTopicData& info,
        bool&)
{
    if (info.guid.guidPrefix != participant->getGuid().guidPrefix)
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

void CommonParticipant::on_reader_discovery(
        fastdds::rtps::RTPSParticipant* participant,
        fastdds::rtps::ReaderDiscoveryStatus reason,
        const fastdds::rtps::SubscriptionBuiltinTopicData& info,
        bool&)
{
    if (info.guid.guidPrefix != participant->getGuid().guidPrefix)
    {
        core::types::Endpoint info_reader =
                detail::create_endpoint_from_info_<fastdds::rtps::SubscriptionBuiltinTopicData>(
            info, this->id());

        if (reason == fastdds::rtps::ReaderDiscoveryStatus::DISCOVERED_READER)
        {
            EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                    "Found in Participant " << configuration_->id << " new Reader " << info.guid << ".");

            this->discovery_database_->add_endpoint(info_reader);
        }
        else if (reason == fastdds::rtps::ReaderDiscoveryStatus::CHANGED_QOS_READER)
        {
            EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                    configuration_->id << " participant : " << "Reader " << info.guid <<
                    " changed TopicQoS.");

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
}

void CommonParticipant::on_writer_discovery(
        fastdds::rtps::RTPSParticipant* participant,
        fastdds::rtps::WriterDiscoveryStatus reason,
        const fastdds::rtps::PublicationBuiltinTopicData& info,
        bool&)
{
    if (info.guid.guidPrefix != participant->getGuid().guidPrefix)
    {
        core::types::Endpoint info_writer =
                detail::create_endpoint_from_info_<fastdds::rtps::PublicationBuiltinTopicData>(
            info, this->id());

        if (reason == fastdds::rtps::WriterDiscoveryStatus::DISCOVERED_WRITER)
        {
            EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                    "Found in Participant " << configuration_->id << " new Writer " << info.guid << ".");

            this->discovery_database_->add_endpoint(info_writer);
        }
        else if (reason == fastdds::rtps::WriterDiscoveryStatus::CHANGED_QOS_WRITER)
        {
            EPROSIMA_LOG_INFO(DDSPIPE_DISCOVERY,
                    configuration_->id << " participant : " << "Writer " << info.guid <<
                    " changed TopicQoS.");

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
}

core::types::Endpoint CommonParticipant::simulate_endpoint(
        const core::types::DdsTopic& topic,
        const core::types::ParticipantId& discoverer_id)
{
    core::types::Endpoint endpoint;
    endpoint.kind = core::types::EndpointKind::reader;
    endpoint.guid = core::types::Guid::new_unique_guid();
    endpoint.topic = topic;
    endpoint.discoverer_participant_id = discoverer_id;
    endpoint.topic.m_topic_discoverer = discoverer_id;

    return endpoint;
}

template<>
DDSPIPE_PARTICIPANTS_DllAPI
std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor>
CommonParticipant::create_descriptor(
        std::set<types::WhitelistType> whitelist)
{
    std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
            std::make_shared<eprosima::fastdds::rtps::UDPv4TransportDescriptor>();

    for (const types::WhitelistType& iface : whitelist)
    {
        udp_transport->interfaceWhiteList.emplace_back(iface);
        EPROSIMA_LOG_INFO(DDSPIPE_COMMON_PARTICIPANT,
                "Adding " << iface << " to UDP whitelist interfaces.");
    }

    return udp_transport;
}

template<>
DDSPIPE_PARTICIPANTS_DllAPI
std::shared_ptr<eprosima::fastdds::rtps::UDPv6TransportDescriptor>
CommonParticipant::create_descriptor(
        std::set<types::WhitelistType> whitelist)
{
    std::shared_ptr<eprosima::fastdds::rtps::UDPv6TransportDescriptor> udp_transport =
            std::make_shared<eprosima::fastdds::rtps::UDPv6TransportDescriptor>();

    for (const types::WhitelistType& iface : whitelist)
    {
        udp_transport->interfaceWhiteList.emplace_back(iface);
        EPROSIMA_LOG_INFO(DDSPIPE_COMMON_PARTICIPANT,
                "Adding " << iface << " to UDP whitelist interfaces.");
    }

    return udp_transport;
}

template<>
DDSPIPE_PARTICIPANTS_DllAPI
std::shared_ptr<eprosima::fastdds::rtps::TCPv4TransportDescriptor>
CommonParticipant::create_descriptor(
        std::set<types::WhitelistType> whitelist)
{
    std::shared_ptr<eprosima::fastdds::rtps::TCPv4TransportDescriptor> tcp_transport =
            std::make_shared<eprosima::fastdds::rtps::TCPv4TransportDescriptor>();

    for (const types::WhitelistType& iface : whitelist)
    {
        tcp_transport->interfaceWhiteList.emplace_back(iface);
        EPROSIMA_LOG_INFO(DDSPIPE_COMMON_PARTICIPANT,
                "Adding " << iface << " to TCP whitelist interfaces.");
    }

    return tcp_transport;
}

template<>
DDSPIPE_PARTICIPANTS_DllAPI
std::shared_ptr<eprosima::fastdds::rtps::TCPv6TransportDescriptor>
CommonParticipant::create_descriptor(
        std::set<types::WhitelistType> whitelist)
{
    std::shared_ptr<eprosima::fastdds::rtps::TCPv6TransportDescriptor> tcp_transport =
            std::make_shared<eprosima::fastdds::rtps::TCPv6TransportDescriptor>();

    for (const types::WhitelistType& iface : whitelist)
    {
        tcp_transport->interfaceWhiteList.emplace_back(iface);
        EPROSIMA_LOG_INFO(DDSPIPE_COMMON_PARTICIPANT,
                "Adding " << iface << " to TCP whitelist interfaces.");
    }

    return tcp_transport;
}

bool CommonParticipant::is_repeater() const noexcept
{
    return configuration_->is_repeater;
}

core::types::ParticipantId CommonParticipant::id() const noexcept
{
    return configuration_->id;
}

bool CommonParticipant::is_rtps_kind() const noexcept
{
    return true;
}

core::types::TopicQoS CommonParticipant::topic_qos() const noexcept
{
    return configuration_->topic_qos;
}

void CommonParticipant::create_participant_(
        const core::types::DomainId& domain,
        const fastdds::rtps::RTPSParticipantAttributes& participant_attributes)
{
    // Unset environment variables that conflict with configuration
    detail::unset_configuration_env_vars();

    EPROSIMA_LOG_INFO(DDSPIPE_RTPS_PARTICIPANT,
            "Creating Participant in domain " << domain);

    // Listener must be set in creation as no callbacks should be missed
    // It is safe to do so here as object is already created and callbacks do not require anything set in this method
    rtps_participant_ = fastdds::rtps::RTPSDomain::createParticipant(
        domain,
        participant_attributes,
        this);

    if (!rtps_participant_)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating RTPS Participant " << this->id());
    }

    EPROSIMA_LOG_INFO(DDSPIPE_RTPS_PARTICIPANT,
            "New Participant created with id " << this->id() <<
            " in domain " << domain << " with guid " << rtps_participant_->getGuid() <<
            (this->is_repeater() ? " (repeater)" : " (non repeater)"));
}

std::shared_ptr<core::IWriter> CommonParticipant::create_writer(
        const core::ITopic& topic)
{
    // Can only create DDS Topics
    const core::types::DdsTopic* dds_topic_ptr = dynamic_cast<const core::types::DdsTopic*>(&topic);
    if (!dds_topic_ptr)
    {
        logDebug(DDSPIPE_RTPS_PARTICIPANT, "Not creating Writer for topic " << topic.topic_name());
        return std::make_shared<BlankWriter>();
    }

    const core::types::DdsTopic& dds_topic = *dds_topic_ptr;

    if (topic.internal_type_discriminator() == core::types::INTERNAL_TOPIC_TYPE_RPC)
    {
        logDebug(DDSPIPE_RTPS_PARTICIPANT,
                "Creating RPC Writer for topic " << topic.topic_name());
        auto writer = std::make_shared<rpc::SimpleWriter>(
            this->id(),
            dds_topic,
            this->payload_pool_,
            rtps_participant_,
            this->configuration_->is_repeater);
        writer->init();

        return writer;
    }
    else if (topic.internal_type_discriminator() == core::types::INTERNAL_TOPIC_TYPE_RTPS)
    {
        if (dds_topic.topic_qos.has_partitions() || dds_topic.topic_qos.has_ownership())
        {
            // Notice that MultiWriter does not require an init call
            return std::make_shared<MultiWriter>(
                this->id(),
                dds_topic,
                this->payload_pool_,
                rtps_participant_,
                this->configuration_->is_repeater);
        }
        else
        {
            auto writer = std::make_shared<SimpleWriter>(
                this->id(),
                dds_topic,
                this->payload_pool_,
                rtps_participant_,
                this->configuration_->is_repeater);
            writer->init();

            return writer;
        }
    }
    else
    {
        logDevError(DDSPIPE_RTPS_PARTICIPANT, "Incorrect dds Topic in Writer creation.");
        return std::make_shared<BlankWriter>();
    }
}

std::shared_ptr<core::IReader> CommonParticipant::create_reader(
        const core::ITopic& topic)
{
    // Can only create DDS Topics
    const core::types::DdsTopic* dds_topic_ptr = dynamic_cast<const core::types::DdsTopic*>(&topic);

    if (!dds_topic_ptr)
    {
        logDebug(DDSPIPE_RTPS_PARTICIPANT, "Not creating Reader for topic " << topic.topic_name());
        return std::make_shared<BlankReader>();
    }

    const core::types::DdsTopic& dds_topic = *dds_topic_ptr;

    if (topic.internal_type_discriminator() == core::types::INTERNAL_TOPIC_TYPE_RPC)
    {
        logDebug(DDSPIPE_RTPS_PARTICIPANT,
                "Creating RPC Reader for topic " << topic.topic_name());

        auto reader = std::make_shared<rpc::SimpleReader>(
            this->id(),
            dds_topic,
            this->payload_pool_,
            rtps_participant_);
        reader->init();

        return reader;
    }
    else if (topic.internal_type_discriminator() == core::types::INTERNAL_TOPIC_TYPE_RTPS)
    {
        if (dds_topic.topic_qos.has_partitions() || dds_topic.topic_qos.has_ownership())
        {
            auto reader = std::make_shared<SpecificQoSReader>(
                this->id(),
                dds_topic,
                this->payload_pool_,
                rtps_participant_,
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
                rtps_participant_);
            reader->init();

            return reader;
        }
    }
    else
    {
        logDevError(DDSPIPE_RTPS_PARTICIPANT, "Incorrect dds Topic in Reader creation.");
        return std::make_shared<BlankReader>();
    }
}

fastdds::rtps::RTPSParticipantAttributes
CommonParticipant::add_participant_att_properties_(
        fastdds::rtps::RTPSParticipantAttributes& params) const
{
    // Ignore the local endpoints so that the reader and writer of the same participant don't match.
    params.properties.properties().emplace_back(
        "fastdds.ignore_local_endpoints",
        "true");

    // Set app properties
    params.properties.properties().emplace_back(
        "fastdds.application.id",
        configuration_->app_id,
        "true");
    params.properties.properties().emplace_back(
        "fastdds.application.metadata",
        configuration_->app_metadata,
        "true");

    return params;
}

fastdds::rtps::RTPSParticipantAttributes
CommonParticipant::reckon_participant_attributes_() const
{
    fastdds::rtps::RTPSParticipantAttributes params;

    // Add Participant name
    params.setName(configuration_->id.c_str());

    add_participant_att_properties_(params);

    return params;
}

} /* namespace rtps */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
