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
 * @file XmlDynTypesParticipant.cpp
 */

#include <memory>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObject.hpp>
#include <fastdds/rtps/attributes/RTPSParticipantAttributes.hpp>
#include <fastdds/rtps/RTPSDomain.hpp>

#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Formatter.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>
#include <ddspipe_core/types/dynamic_types/types.hpp>
#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>
#include <ddspipe_participants/reader/rtps/SpecificQoSReader.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>

#include <ddspipe_participants/participant/dynamic_types/XmlDynTypesParticipant.hpp>

#include "idl_types.hpp"

namespace eprosima {
namespace ddspipe {
namespace participants {

using namespace eprosima::ddspipe::core;

XmlDynTypesParticipant::XmlDynTypesParticipant(
        std::shared_ptr<XmlParticipantConfiguration> participant_configuration,
        std::shared_ptr<PayloadPool> payload_pool,
        std::shared_ptr<DiscoveryDatabase> discovery_database)
    : XmlParticipant(
        participant_configuration,
        payload_pool,
        discovery_database)
    , type_object_reader_(std::make_shared<InternalReader>(
                this->id()))
    , idl_types_(std::make_shared<std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>>())
{
    // Do nothing
}

std::shared_ptr<IWriter> XmlDynTypesParticipant::create_writer(
        const ITopic& topic)
{
    // This participant does not write anything
    return std::make_shared<BlankWriter>();
}

std::shared_ptr<IReader> XmlDynTypesParticipant::create_reader(
        const ITopic& topic)
{
    // If type object topic, return the internal reader for type objects
    if (core::types::is_type_object_topic(topic))
    {
        return type_object_reader_;
    }

    // If not type object, use the parent method
    return dds::XmlParticipant::create_reader(topic);
}

void XmlDynTypesParticipant::init()
{
    const auto& idl_types_paths = configuration_->idl_types_paths;

    // Other failures only affect some types, and are already logged
    if (!idl_types_paths.empty() &&
            register_idl_types({idl_types_paths.begin(), idl_types_paths.end()}, configuration_->idl_include_paths,
            *idl_types_) == utils::ReturnCode::RETCODE_PRECONDITION_NOT_MET)
    {
        throw utils::InitializationException(STR_ENTRY << "Error loading the IDL types of Participant " << id() << ".");
    }

    dds::XmlParticipant::init();
}

XmlDynTypesParticipant::XmlDynTypesDdsListener::XmlDynTypesDdsListener(
        std::shared_ptr<SimpleParticipantConfiguration> conf,
        std::shared_ptr<core::DiscoveryDatabase> ddb,
        std::shared_ptr<InternalReader> internal_reader)
    : XmlDynTypesDdsListener(
        conf,
        ddb,
        internal_reader,
        std::make_shared<const std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>>())
{
}

XmlDynTypesParticipant::XmlDynTypesDdsListener::XmlDynTypesDdsListener(
        std::shared_ptr<SimpleParticipantConfiguration> conf,
        std::shared_ptr<core::DiscoveryDatabase> ddb,
        std::shared_ptr<InternalReader> internal_reader,
        std::shared_ptr<const std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>> idl_types)
    : dds::CommonParticipant::DdsListener(conf, ddb)
    , type_object_reader_(internal_reader)
    , idl_types_(idl_types)
{
}

void XmlDynTypesParticipant::XmlDynTypesDdsListener::on_data_reader_discovery(
        fastdds::dds::DomainParticipant* participant,
        fastdds::rtps::ReaderDiscoveryStatus reason,
        const fastdds::dds::SubscriptionBuiltinTopicData& info,
        bool& should_be_ignored)
{
    if (info.guid.guidPrefix != participant->guid().guidPrefix)
    {
        // Get type information
        const auto type_info = info.type_information.type_information;
        const auto type_name = info.type_name.to_string();

        dds::CommonParticipant::DdsListener::on_data_reader_discovery(participant, reason, info, should_be_ignored);

        notify_type_discovered_(type_info, type_name);
    }
}

void XmlDynTypesParticipant::XmlDynTypesDdsListener::on_data_writer_discovery(
        fastdds::dds::DomainParticipant* participant,
        fastdds::rtps::WriterDiscoveryStatus reason,
        const fastdds::dds::PublicationBuiltinTopicData& info,
        bool& should_be_ignored)
{
    if (info.guid.guidPrefix != participant->guid().guidPrefix)
    {
        // Get type information
        const auto type_info = info.type_information.type_information;
        const auto type_name = info.type_name.to_string();

        dds::CommonParticipant::DdsListener::on_data_writer_discovery(participant, reason, info, should_be_ignored);

        notify_type_discovered_(type_info, type_name);
    }
}

void XmlDynTypesParticipant::XmlDynTypesDdsListener::notify_type_discovered_(
        const fastdds::dds::xtypes::TypeInformation& type_info,
        const std::string& type_name)
{
    // Check if it exists already
    if (received_types_.find(type_name) != received_types_.end())
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DYNTYPES_PARTICIPANT,
                "Type " << type_name << " was already received, aborting propagation.");
        return;
    }

    auto type_identifier = type_info.complete().typeid_with_size().type_id();

    // Endpoints that do not share their type are resolved by name with the types loaded from IDL
    if (fastdds::dds::xtypes::TK_NONE == type_identifier._d())
    {
        const auto idl_type = idl_types_->find(type_name);

        if (idl_type != idl_types_->end())
        {
            type_identifier = idl_type->second;
        }
    }

    fastdds::dds::xtypes::TypeObject dyn_type_object;
    if (fastdds::dds::RETCODE_OK !=
            fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_object(
                type_identifier,
                dyn_type_object))
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DYNTYPES_PARTICIPANT,
                "Failed to get type object of " << type_name << " type");
        return;
    }

    // Create Dynamic Type
    fastdds::dds::DynamicType::_ref_type dyn_type =
            fastdds::dds::DynamicTypeBuilderFactory::get_instance()->create_type_w_type_object(
        dyn_type_object)->build();
    if (!dyn_type)
    {
        EPROSIMA_LOG_WARNING(DDSPIPE_DYNTYPES_PARTICIPANT,
                "Failed to create Dynamic Type " << type_name);
        return;
    }

    // Notify type_identifier
    // NOTE: We assume each type_name corresponds to only one type_identifier
    EPROSIMA_LOG_INFO(DDSPIPE_DYNTYPES_PARTICIPANT,
            "Participant " << configuration_->id << " discovered type object " << dyn_type->get_name());

    monitor_type_discovered(type_name);

    // If not, add it to the received types set
    received_types_.insert(type_name);

    // Create data containing Dynamic Type
    auto data = std::make_unique<core::types::DynamicTypeData>();
    data->dynamic_type = dyn_type; // TODO: add constructor with param
    data->type_identifier = type_identifier;

    // Insert new data in internal reader queue
    type_object_reader_->simulate_data_reception(std::move(data));
}

std::unique_ptr<fastdds::dds::DomainParticipantListener> XmlDynTypesParticipant::create_listener_()
{
    return std::make_unique<XmlDynTypesDdsListener>(configuration_, discovery_database_, type_object_reader_,
                   idl_types_);
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
