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
        eprosima::fastrtps::types::TypeObjectFactory::get_instance()->add_type_object(
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
    std::function<void(const std::string&, const eprosima::fastrtps::types::DynamicType_ptr)> callback(
        [this]
            (const std::string& /* type_name */, const eprosima::fastrtps::types::DynamicType_ptr type)
        {
            this->internal_notify_type_object_(type);
        });

    // Registering type and creating reader
    participant->register_remote_type(
        type_information,
        type_name.to_string(),
        callback);
}

void DynTypesParticipant::internal_notify_type_object_(
        eprosima::fastrtps::types::DynamicType_ptr dynamic_type)
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
    eprosima::fastdds::dds::DomainParticipantQos pqos;
    pqos.name(this->id());

    // Set Type LookUp to ON
    pqos.wire_protocol().builtin.typelookup_config.use_server = false;
    pqos.wire_protocol().builtin.typelookup_config.use_client = true;

    // CREATE THE PARTICIPANT
    dds_participant_ = eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(
        std::dynamic_pointer_cast<SimpleParticipantConfiguration>(this->configuration_)->domain,
        pqos,
        this);

    if (dds_participant_ == nullptr)
    {
        throw utils::InitializationException("Error creating DDS Participant.");
    }
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
