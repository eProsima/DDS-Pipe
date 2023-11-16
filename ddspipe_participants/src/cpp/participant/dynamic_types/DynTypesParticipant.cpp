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

/**
 * @file DynTypesParticipant.cpp
 */

#include <memory>

#include <cpp_utils/Log.hpp>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastrtps/rtps/participant/RTPSParticipant.h>
#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/TypeObjectFactory.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/attributes/RTPSParticipantAttributes.h>
#include <ddspipe_core/types/dynamic_types/types.hpp>

#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>
#include <ddspipe_participants/reader/rtps/SimpleReader.hpp>
#include <ddspipe_participants/reader/rtps/SpecificQoSReader.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>

#include <ddspipe_participants/participant/dynamic_types/DynTypesParticipant.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;
using namespace eprosima::fastrtps::types;

DynTypesParticipant::DynTypesParticipant(
        std::shared_ptr<SimpleParticipantConfiguration> participant_configuration,
        std::shared_ptr<PayloadPool> payload_pool,
        std::shared_ptr<DiscoveryDatabase> discovery_database)
    : SimpleParticipant(
        participant_configuration,
        payload_pool,
        discovery_database)
    , type_object_reader_(std::make_shared<InternalReader>(
                this->id()))
{
    // Do nothing
}

DynTypesParticipant::~DynTypesParticipant()
{
    dds_participant_->set_listener(nullptr);
    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(dds_participant_);

}

void DynTypesParticipant::init()
{
    CommonParticipant::init();
    initialize_internal_dds_participant_();
}

std::shared_ptr<IWriter> DynTypesParticipant::create_writer(
        const ITopic& topic)
{
    // This participant does not write anything
    return std::make_shared<BlankWriter>();
}

std::shared_ptr<IReader> DynTypesParticipant::create_reader(
        const ITopic& topic)
{
    // If type object topic, return the internal reader for type objects
    if (is_type_object_topic(topic))
    {
        return this->type_object_reader_;
    }

    // If not type object, use the parent method
    return rtps::SimpleParticipant::create_reader(topic);
}

void DynTypesParticipant::on_type_discovery(
        eprosima::fastdds::dds::DomainParticipant* /* participant */,
        const fastrtps::rtps::SampleIdentity& /* request_sample_id */,
        const fastrtps::string_255& /* topic */,
        const fastrtps::types::TypeIdentifier* identifier,
        const fastrtps::types::TypeObject* object,
        fastrtps::types::DynamicType_ptr dyn_type)
{
    if (nullptr != dyn_type)
    {
        // Register type obj in singleton factory
        TypeObjectFactory::get_instance()->add_type_object(
            dyn_type->get_name(), identifier, object);
        internal_notify_type_object_(dyn_type);
    }
}

void DynTypesParticipant::on_type_information_received(
        eprosima::fastdds::dds::DomainParticipant* participant,
        const fastrtps::string_255 /* topic_name */,
        const fastrtps::string_255 type_name,
        const fastrtps::types::TypeInformation& type_information)
{
    std::string type_name_ = type_name.to_string();
    const TypeIdentifier* type_identifier = nullptr;
    const TypeObject* type_object = nullptr;
    DynamicType_ptr dynamic_type(nullptr);

    // Check if complete identifier already present in factory
    type_identifier = TypeObjectFactory::get_instance()->get_type_identifier(type_name_, true);
    if (type_identifier)
    {
        type_object = TypeObjectFactory::get_instance()->get_type_object(type_name_, true);
    }

    // If complete not found, try with minimal
    if (!type_object)
    {
        type_identifier = TypeObjectFactory::get_instance()->get_type_identifier(type_name_, false);
        if (type_identifier)
        {
            type_object = TypeObjectFactory::get_instance()->get_type_object(type_name_, false);
        }
    }

    // Build dynamic type if type identifier and object found in factory
    if (type_identifier && type_object)
    {
        dynamic_type = TypeObjectFactory::get_instance()->build_dynamic_type(type_name_, type_identifier, type_object);
    }

    // Request type object through TypeLookup if not present in factory, or if type building failed
    if (!dynamic_type)
    {
        std::function<void(const std::string&, const DynamicType_ptr)> callback(
            [this]
                (const std::string& /* type_name */, const DynamicType_ptr type)
            {
                this->internal_notify_type_object_(type);
            });
        // Registering type and creating reader
        participant->register_remote_type(
            type_information,
            type_name_,
            callback);
    }
    else
    {
        internal_notify_type_object_(dynamic_type);
    }
}

void DynTypesParticipant::internal_notify_type_object_(
        DynamicType_ptr dynamic_type)
{
    logInfo(DDSPIPE_DYNTYPES_PARTICIPANT,
            "Participant " << this->id() << " discovered type object " << dynamic_type->get_name());

    // Create data containing Dynamic Type
    auto data = std::make_unique<DynamicTypeData>();
    data->dynamic_type = dynamic_type; // TODO: add constructor with param

    // Insert new data in internal reader queue
    type_object_reader_->simulate_data_reception(std::move(data));
}

void DynTypesParticipant::initialize_internal_dds_participant_()
{

    std::shared_ptr<SimpleParticipantConfiguration> configuration =
            std::dynamic_pointer_cast<SimpleParticipantConfiguration>(this->configuration_);

    eprosima::fastdds::dds::DomainParticipantQos pqos;
    pqos.name(this->id());

    // Set app properties
    pqos.properties().properties().emplace_back(
        "fastdds.application.id",
        configuration->app_id,
        "true");
    pqos.properties().properties().emplace_back(
        "fastdds.application.metadata",
        configuration->app_metadata,
        "true");

    // Set Type LookUp to ON
    pqos.wire_protocol().builtin.typelookup_config.use_server = false;
    pqos.wire_protocol().builtin.typelookup_config.use_client = true;

    // Configure Participant transports
    if (configuration->transport == core::types::TransportDescriptors::builtin)
    {
        if (!configuration->whitelist.empty())
        {
            pqos.transport().use_builtin_transports = false;

            std::shared_ptr<eprosima::fastdds::rtps::SharedMemTransportDescriptor> shm_transport =
                    std::make_shared<eprosima::fastdds::rtps::SharedMemTransportDescriptor>();
            pqos.transport().user_transports.push_back(shm_transport);

            std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
                    create_descriptor<eprosima::fastdds::rtps::UDPv4TransportDescriptor>(configuration->whitelist);
            pqos.transport().user_transports.push_back(udp_transport);
        }
    }
    else if (configuration->transport == core::types::TransportDescriptors::shm_only)
    {
        pqos.transport().use_builtin_transports = false;

        std::shared_ptr<eprosima::fastdds::rtps::SharedMemTransportDescriptor> shm_transport =
                std::make_shared<eprosima::fastdds::rtps::SharedMemTransportDescriptor>();
        pqos.transport().user_transports.push_back(shm_transport);
    }
    else if (configuration->transport == core::types::TransportDescriptors::udp_only)
    {
        pqos.transport().use_builtin_transports = false;

        std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
                create_descriptor<eprosima::fastdds::rtps::UDPv4TransportDescriptor>(configuration->whitelist);
        pqos.transport().user_transports.push_back(udp_transport);
    }

    // Participant discovery filter configuration
    switch (configuration->ignore_participant_flags)
    {
        case core::types::IgnoreParticipantFlags::no_filter:
            pqos.wire_protocol().builtin.discovery_config.ignoreParticipantFlags =
                    eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::NO_FILTER;
            break;
        case core::types::IgnoreParticipantFlags::filter_different_host:
            pqos.wire_protocol().builtin.discovery_config.ignoreParticipantFlags =
                    eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::FILTER_DIFFERENT_HOST;
            break;
        case core::types::IgnoreParticipantFlags::filter_different_process:
            pqos.wire_protocol().builtin.discovery_config.ignoreParticipantFlags =
                    eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::FILTER_DIFFERENT_PROCESS;
            break;
        case core::types::IgnoreParticipantFlags::filter_same_process:
            pqos.wire_protocol().builtin.discovery_config.ignoreParticipantFlags =
                    eprosima::fastrtps::rtps::ParticipantFilteringFlags_t::FILTER_SAME_PROCESS;
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
    fastdds::dds::DomainParticipantFactoryQos fact_qos;
    fact_qos.entity_factory().autoenable_created_entities = false;
    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->set_qos(
        fact_qos);

    // CREATE THE PARTICIPANT
    dds_participant_ = eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(
        configuration->domain,
        pqos);

    dds_participant_->set_listener(this);

    dds_participant_->enable();

    // Restore default DomainParticipantQoS (create enabled entities) after creating and enabling this participant
    // WARNING: not thread safe at the moment of this writing, see note above.
    fact_qos.entity_factory().autoenable_created_entities = true;
    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->set_qos(
        fact_qos);

    if (dds_participant_ == nullptr)
    {
        throw utils::InitializationException("Error creating DDS Participant.");
    }
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
