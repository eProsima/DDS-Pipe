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

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObject.hpp>
#include <fastdds/rtps/attributes/RTPSParticipantAttributes.hpp>
#include <fastdds/rtps/RTPSDomain.hpp>

#include <cpp_utils/Log.hpp>

#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>
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
    if (core::types::is_type_object_topic(topic))
    {
        return type_object_reader_;
    }

    // If not type object, use the parent method
    return rtps::SimpleParticipant::create_reader(topic);
}

std::shared_ptr<IReader> DynTypesParticipant::create_reader_with_filter(
        const ITopic& topic,
        const std::string filter)
{
    // If type object topic, return the internal reader for type objects
    if (core::types::is_type_object_topic(topic))
    {
        return type_object_reader_;
    }

    // If not type object, use the parent method
    return rtps::SimpleParticipant::create_reader_with_filter(topic, filter);
}

DynTypesParticipant::DynTypesRtpsListener::DynTypesRtpsListener(
        std::shared_ptr<ParticipantConfiguration> conf,
        std::shared_ptr<core::DiscoveryDatabase> ddb,
        std::shared_ptr<InternalReader> internal_reader)
    : rtps::CommonParticipant::RtpsListener(
        conf,
        ddb)
    , type_object_reader_(internal_reader)
{
}

void DynTypesParticipant::DynTypesRtpsListener::on_reader_discovery(
        fastdds::rtps::RTPSParticipant* participant,
        fastdds::rtps::ReaderDiscoveryStatus reason,
        const fastdds::rtps::SubscriptionBuiltinTopicData& info,
        bool& should_be_ignored)
{
    if (info.guid.guidPrefix != participant->getGuid().guidPrefix)
    {
        // Get type information
        const auto type_info = info.type_information.type_information;
        const auto type_name = info.type_name.to_string();

        rtps::CommonParticipant::RtpsListener::on_reader_discovery(participant, reason, info, should_be_ignored);

        notify_type_discovered_(type_info, type_name);
    }
}

void DynTypesParticipant::DynTypesRtpsListener::on_writer_discovery(
        fastdds::rtps::RTPSParticipant* participant,
        fastdds::rtps::WriterDiscoveryStatus reason,
        const fastdds::rtps::PublicationBuiltinTopicData& info,
        bool& should_be_ignored)
{
    if (info.guid.guidPrefix != participant->getGuid().guidPrefix)
    {
        // Get type information
        const auto type_info = info.type_information.type_information;
        const auto type_name = info.type_name.to_string();

        rtps::CommonParticipant::RtpsListener::on_writer_discovery(participant, reason, info, should_be_ignored);

        notify_type_discovered_(type_info, type_name);
    }
}

void DynTypesParticipant::DynTypesRtpsListener::notify_type_discovered_(
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

    const auto type_identifier = type_info.complete().typeid_with_size().type_id();
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

std::unique_ptr<fastdds::rtps::RTPSParticipantListener> DynTypesParticipant::create_listener_()
{
    return std::make_unique<DynTypesRtpsListener>(configuration_, discovery_database_, type_object_reader_);
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
