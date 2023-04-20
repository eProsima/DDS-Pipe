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
#include <fastrtps/rtps/participant/RTPSParticipant.h>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>
#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/TypeObjectFactory.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/attributes/RTPSParticipantAttributes.h>

#include <ddspipe_core/types/dynamic_types/types.hpp>

#include <ddspipe_participants/participant/rtps/SimpleParticipant.hpp>
#include <ddspipe_participants/participant/dds/DdsCommonParticipant.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;
using namespace eprosima::fastrtps::types;

DdsCommonParticipant::DdsCommonParticipant(
        const std::shared_ptr<SimpleParticipantConfiguration>& participant_configuration)
    : participant_configuration_(participant_configuration)
{
    // Do nothing
}

DdsCommonParticipant::~DdsCommonParticipant()
{
    dds_participant_->set_listener(nullptr);

    dds_participant_->delete_publisher(dds_publisher_);
    dds_participant_->delete_subscriber(dds_subscriber_);

    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(dds_participant_);
}

core::types::ParticipantId DdsCommonParticipant::id() const noexcept
{
    return participant_configuration_->id;
}

bool DdsCommonParticipant::is_rtps_kind() const noexcept
{
    return false;
}

bool DdsCommonParticipant::is_repeater() const noexcept
{
    return false;
}

void DdsCommonParticipant::init()
{
    eprosima::fastdds::dds::DomainParticipantQos pqos;
    pqos.name(this->id());

    // Set Type LookUp to ON
    // TODO this should not be true in all cases, but lets keep it for now
    pqos.wire_protocol().builtin.typelookup_config.use_server = true;
    pqos.wire_protocol().builtin.typelookup_config.use_client = true;

    // Configure Participant transports
    if (participant_configuration_->transport == participants::types::TransportProtocol::builtin)
    {
        if (!participant_configuration_->whitelist.empty())
        {
            pqos.transport().use_builtin_transports = false;

            std::shared_ptr<eprosima::fastdds::rtps::SharedMemTransportDescriptor> shm_transport =
                    std::make_shared<eprosima::fastdds::rtps::SharedMemTransportDescriptor>();
            pqos.transport().user_transports.push_back(shm_transport);

            std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
                    rtps::SimpleParticipant::configure_upd_transport_(participant_configuration_->whitelist);
            pqos.transport().user_transports.push_back(udp_transport);
        }
    }
    else if (participant_configuration_->transport == participants::types::TransportProtocol::shm)
    {
        pqos.transport().use_builtin_transports = false;

        std::shared_ptr<eprosima::fastdds::rtps::SharedMemTransportDescriptor> shm_transport =
                std::make_shared<eprosima::fastdds::rtps::SharedMemTransportDescriptor>();
        pqos.transport().user_transports.push_back(shm_transport);
    }
    else if (participant_configuration_->transport == participants::types::TransportProtocol::udp)
    {
        pqos.transport().use_builtin_transports = false;

        std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
                rtps::SimpleParticipant::configure_upd_transport_(participant_configuration_->whitelist);
            pqos.transport().user_transports.push_back(udp_transport);
        pqos.transport().user_transports.push_back(udp_transport);
    }

    // Participant discovery filter participant_configuration_
    switch (participant_configuration_->ignore_participant_flags)
    {
        case core::types::IgnoreParticipantFlags::no_filter:
            pqos.wire_protocol().builtin.discovery_config.ignoreParticipantFlags = eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::NO_FILTER;
            break;
        case core::types::IgnoreParticipantFlags::filter_different_host:
            pqos.wire_protocol().builtin.discovery_config.ignoreParticipantFlags = eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::FILTER_DIFFERENT_HOST;
            break;
        case core::types::IgnoreParticipantFlags::filter_different_process:
            pqos.wire_protocol().builtin.discovery_config.ignoreParticipantFlags = eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::FILTER_DIFFERENT_PROCESS;
            break;
        case core::types::IgnoreParticipantFlags::filter_same_process:
            pqos.wire_protocol().builtin.discovery_config.ignoreParticipantFlags = eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::FILTER_SAME_PROCESS;
            break;
        case core::types::IgnoreParticipantFlags::filter_different_and_same_process:
            pqos.wire_protocol().builtin.discovery_config.ignoreParticipantFlags =
                    static_cast<eprosima::fastrtps::rtps::ParticipantFilteringFlags_t>(
                eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::FILTER_DIFFERENT_PROCESS |
                eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::FILTER_SAME_PROCESS);
            break;
        default:
            break;
    }

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
    dds_participant_ = eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(
        participant_configuration_->domain,
        pqos,
        this);

    dds_participant_->enable();

    // Restore default DomainParticipantQoS (create enabled entities) after creating and enabling this participant
    // WARNING: not thread safe at the moment of this writing, see note above.
    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->set_qos(
        original_fact_qos);

    if (dds_participant_ == nullptr)
    {
        throw utils::InitializationException("Error creating DDS Participant.");
    }

    dds_publisher_ = dds_participant_->create_publisher(fastdds::dds::PublisherQos());
    dds_subscriber_ = dds_participant_->create_subscriber(fastdds::dds::SubscriberQos());
}

core::types::Endpoint DdsCommonParticipant::simulate_endpoint_(
        const DdsTopic& topic) const
{
    core::types::Endpoint endpoint;
    endpoint.kind = core::types::EndpointKind::reader;
    endpoint.guid = core::types::Guid::new_unique_guid();
    endpoint.topic = topic;
    endpoint.discoverer_participant_id = this->id();

    return endpoint;
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
