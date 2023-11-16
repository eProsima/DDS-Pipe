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

#include <fastdds/rtps/transport/TCPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/TCPv6TransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv6TransportDescriptor.h>
#include <fastrtps/rtps/participant/RTPSParticipant.h>
#include <fastrtps/rtps/RTPSDomain.h>

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
        const core::types::DomainId& domain_id,
        const fastrtps::rtps::RTPSParticipantAttributes& participant_attributes)
    : configuration_(participant_configuration)
    , payload_pool_(payload_pool)
    , discovery_database_(discovery_database)
    , domain_id_(domain_id)
    , participant_attributes_(participant_attributes)
{
    // Do nothing
}

CommonParticipant::~CommonParticipant()
{
    if (rtps_participant_)
    {
        fastrtps::rtps::RTPSDomain::removeRTPSParticipant(rtps_participant_);
    }
}

void CommonParticipant::init()
{
    create_participant_(
        domain_id_,
        participant_attributes_);
}

void CommonParticipant::onParticipantDiscovery(
        fastrtps::rtps::RTPSParticipant* participant,
        fastrtps::rtps::ParticipantDiscoveryInfo&& info)
{
    if (info.info.m_guid.guidPrefix != participant->getGuid().guidPrefix)
    {
        if (info.status == fastrtps::rtps::ParticipantDiscoveryInfo::DISCOVERED_PARTICIPANT)
        {
            logInfo(DDSPIPE_DISCOVERY,
                    "Found in Participant " << configuration_->id << " new Participant " << info.info.m_guid << ".");
        }
        else if (info.status == fastrtps::rtps::ParticipantDiscoveryInfo::CHANGED_QOS_PARTICIPANT)
        {
            logInfo(DDSPIPE_DISCOVERY, "Participant " << info.info.m_guid << " changed QoS.");
        }
        else if (info.status == fastrtps::rtps::ParticipantDiscoveryInfo::REMOVED_PARTICIPANT)
        {
            logInfo(DDSPIPE_DISCOVERY, "Participant " << info.info.m_guid << " removed.");
        }
        else
        {
            logInfo(DDSPIPE_DISCOVERY, "Participant " << info.info.m_guid << " dropped.");
        }
    }
}

void CommonParticipant::onReaderDiscovery(
        fastrtps::rtps::RTPSParticipant* participant,
        fastrtps::rtps::ReaderDiscoveryInfo&& info)
{
    if (info.info.guid().guidPrefix != participant->getGuid().guidPrefix)
    {
        core::types::Endpoint info_reader = detail::create_endpoint_from_info_<fastrtps::rtps::ReaderDiscoveryInfo>(
            info, this->id());

        if (info.status == fastrtps::rtps::ReaderDiscoveryInfo::DISCOVERED_READER)
        {
            logInfo(DDSPIPE_DISCOVERY,
                    "Found in Participant " << configuration_->id << " new Reader " << info.info.guid() << ".");

            this->discovery_database_->add_endpoint(info_reader);
        }
        else if (info.status == fastrtps::rtps::ReaderDiscoveryInfo::CHANGED_QOS_READER)
        {
            logInfo(DDSPIPE_DISCOVERY, "Reader " << info.info.guid() << " changed TopicQoS.");

            this->discovery_database_->update_endpoint(info_reader);
        }
        else if (info.status == fastrtps::rtps::ReaderDiscoveryInfo::REMOVED_READER)
        {
            logInfo(DDSPIPE_DISCOVERY, "Reader " << info.info.guid() << " removed.");

            info_reader.active = false;
            this->discovery_database_->update_endpoint(info_reader);
        }
        else
        {
            logInfo(DDSPIPE_DISCOVERY, "Reader " << info.info.guid() << " dropped.");

            info_reader.active = false;
            this->discovery_database_->update_endpoint(info_reader);
        }
    }
}

void CommonParticipant::onWriterDiscovery(
        fastrtps::rtps::RTPSParticipant* participant,
        fastrtps::rtps::WriterDiscoveryInfo&& info)
{
    if (info.info.guid().guidPrefix != participant->getGuid().guidPrefix)
    {
        core::types::Endpoint info_writer = detail::create_endpoint_from_info_<fastrtps::rtps::WriterDiscoveryInfo>(
            info, this->id());

        if (info.status == fastrtps::rtps::WriterDiscoveryInfo::DISCOVERED_WRITER)
        {
            logInfo(DDSPIPE_DISCOVERY,
                    "Found in Participant " << configuration_->id << " new Writer " << info.info.guid() << ".");

            this->discovery_database_->add_endpoint(info_writer);
        }
        else if (info.status == fastrtps::rtps::WriterDiscoveryInfo::CHANGED_QOS_WRITER)
        {
            logInfo(DDSPIPE_DISCOVERY, "Writer " << info.info.guid() << " changed TopicQoS.");

            this->discovery_database_->update_endpoint(info_writer);
        }
        else if (info.status == fastrtps::rtps::WriterDiscoveryInfo::REMOVED_WRITER)
        {
            logInfo(DDSPIPE_DISCOVERY, "Writer " << info.info.guid() << " removed.");

            info_writer.active = false;
            this->discovery_database_->update_endpoint(info_writer);
        }
        else
        {
            logInfo(DDSPIPE_DISCOVERY, "Writer " << info.info.guid() << " dropped.");

            info_writer.active = false;
            this->discovery_database_->update_endpoint(info_writer);
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
        std::set<types::IpType> whitelist)
{
    std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
            std::make_shared<eprosima::fastdds::rtps::UDPv4TransportDescriptor>();

    for (const types::IpType& ip : whitelist)
    {
        if (types::Address::is_ipv4_correct(ip))
        {
            udp_transport->interfaceWhiteList.emplace_back(ip);
            logInfo(DDSPIPE_COMMON_PARTICIPANT,
                    "Adding " << ip << " to UDP whitelist interfaces.");
        }
        else
        {
            // Invalid address, continue with next one
            logWarning(DDSPIPE_COMMON_PARTICIPANT,
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
            logInfo(DDSPIPE_COMMON_PARTICIPANT,
                    "Adding " << ip << " to UDP whitelist interfaces.");
        }
        else
        {
            // Invalid address, continue with next one
            logWarning(DDSPIPE_COMMON_PARTICIPANT,
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
            logInfo(DDSPIPE_COMMON_PARTICIPANT,
                    "Adding " << ip << " to TCP whitelist interfaces.");
        }
        else
        {
            // Invalid address, continue with next one
            logWarning(DDSPIPE_COMMON_PARTICIPANT,
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
            logInfo(DDSPIPE_COMMON_PARTICIPANT,
                    "Adding " << ip << " to TCP whitelist interfaces.");
        }
        else
        {
            // Invalid address, continue with next one
            logWarning(DDSPIPE_COMMON_PARTICIPANT,
                    "Not valid IPv6. Discarding TCP whitelist interface " << ip << ".");
        }
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
        const fastrtps::rtps::RTPSParticipantAttributes& participant_attributes)
{
    logInfo(DDSPIPE_RTPS_PARTICIPANT,
            "Creating Participant in domain " << domain);

    // Listener must be set in creation as no callbacks should be missed
    // It is safe to do so here as object is already created and callbacks do not require anything set in this method
    rtps_participant_ = fastrtps::rtps::RTPSDomain::createParticipant(
        domain,
        participant_attributes,
        this);

    if (!rtps_participant_)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating RTPS Participant " << this->id());
    }

    logInfo(DDSPIPE_RTPS_PARTICIPANT,
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

fastrtps::rtps::RTPSParticipantAttributes
CommonParticipant::reckon_participant_attributes_(
        const ParticipantConfiguration* participant_configuration)
{
    fastrtps::rtps::RTPSParticipantAttributes params;

    // Add Participant name
    params.setName(participant_configuration->id.c_str());

    // Ignore the local endpoints so that the reader and writer of the same participant don't match.
    params.properties.properties().emplace_back(
        "fastdds.ignore_local_endpoints",
        "true");

    // Set app properties
    params.properties.properties().emplace_back(
        "fastdds.application.id",
        participant_configuration->app_id,
        "true");
    params.properties.properties().emplace_back(
        "fastdds.application.metadata",
        participant_configuration->app_metadata,
        "true");

    return params;
}

} /* namespace rtps */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
