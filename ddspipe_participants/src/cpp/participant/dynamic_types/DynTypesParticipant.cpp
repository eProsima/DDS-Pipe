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

#include <fastdds/rtps/attributes/RTPSParticipantAttributes.hpp>
#include <fastdds/rtps/RTPSDomain.hpp>

#include <ddspipe_core/types/dynamic_types/types.hpp>
#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>
#include <ddspipe_participants/reader/rtps/SimpleReader.hpp>
#include <ddspipe_participants/reader/rtps/SpecificQoSReader.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>

#include <ddspipe_participants/participant/dynamic_types/DynTypesParticipant.hpp>

#include "dyn_types_common.hpp"

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
    , idl_types_(std::make_shared<std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>>())
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

void DynTypesParticipant::init()
{
    load_configured_idl_types(*std::dynamic_pointer_cast<SimpleParticipantConfiguration>(configuration_), *idl_types_);

    rtps::SimpleParticipant::init();
}

DynTypesParticipant::DynTypesRtpsListener::DynTypesRtpsListener(
        std::shared_ptr<ParticipantConfiguration> conf,
        std::shared_ptr<core::DiscoveryDatabase> ddb,
        std::shared_ptr<InternalReader> internal_reader)
    : DynTypesRtpsListener(
        conf,
        ddb,
        internal_reader,
        std::make_shared<const std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>>())
{
}

DynTypesParticipant::DynTypesRtpsListener::DynTypesRtpsListener(
        std::shared_ptr<ParticipantConfiguration> conf,
        std::shared_ptr<core::DiscoveryDatabase> ddb,
        std::shared_ptr<InternalReader> internal_reader,
        std::shared_ptr<const std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>> idl_types)
    : rtps::CommonParticipant::RtpsListener(
        conf,
        ddb)
    , type_object_reader_(internal_reader)
    , idl_types_(idl_types)
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
    const auto configuration = std::dynamic_pointer_cast<SimpleParticipantConfiguration>(configuration_);

    notify_type_discovered(type_info, type_name, *idl_types_, configuration && configuration->idl_types_priority,
            configuration_->id, received_types_, *type_object_reader_);
}

std::unique_ptr<fastdds::rtps::RTPSParticipantListener> DynTypesParticipant::create_listener_()
{
    return std::make_unique<DynTypesRtpsListener>(configuration_, discovery_database_, type_object_reader_,
                   idl_types_);
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
