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
#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/TypeObjectFactory.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/attributes/RTPSParticipantAttributes.h>
#include <fastdds/dds/publisher/Publisher.hpp>

#include <ddspipe_core/types/dynamic_types/types.hpp>
#include <ddspipe_core/types/dynamic_types/types.hpp>


#include <ddspipe_participants/participant/rtps/CommonParticipant.hpp>
#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>
#include <ddspipe_participants/reader/rtps/SimpleReader.hpp>
#include <ddspipe_participants/reader/rtps/SpecificQoSReader.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>

#include <ddspipe_participants/participant/dynamic_types/DynTypesSubscriptionParticipant.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;
using namespace eprosima::fastrtps::types;

DynTypesSubscriptionParticipant::DynTypesSubscriptionParticipant(
        const std::shared_ptr<SimpleParticipantConfiguration>& participant_configuration,
        const std::shared_ptr<core::DiscoveryDatabase>& discovery_database)
    : dds::DdsCommonParticipant(participant_configuration)
    , type_object_reader_(std::make_shared<InternalReader>(
        participant_configuration->id))
{
}

std::shared_ptr<IWriter> DynTypesSubscriptionParticipant::create_writer(
        const ITopic& topic)
{
    // This participant does not write anything
    return std::make_shared<BlankWriter>();
}

std::shared_ptr<IReader> DynTypesSubscriptionParticipant::create_reader(
        const ITopic& topic)
{
    if (is_type_object_topic(topic))
    {
        return this->type_object_reader_;
    }

    // This participant does not read anything
    return std::make_shared<BlankReader>();
}

void DynTypesSubscriptionParticipant::on_type_discovery(
        eprosima::fastdds::dds::DomainParticipant* /* participant */,
        const fastrtps::rtps::SampleIdentity& /* request_sample_id */,
        const fastrtps::string_255& /* topic */,
        const fastrtps::types::TypeIdentifier* identifier,
        const fastrtps::types::TypeObject* object,
        fastrtps::types::DynamicType_ptr dyn_type)
{
    if (nullptr != dyn_type)
    {
        if (dyn_type->get_name() == "DdsRecorderCommand")
        {
            logError(DEBUG_COMMAND,
                "Participant " << this->id() << " received type " << dyn_type->get_name());
        }
        // Register type obj in singleton factory
        TypeObjectFactory::get_instance()->add_type_object(
            dyn_type->get_name(), identifier, object);
        internal_notify_type_object_(dyn_type);
    }
}

void DynTypesSubscriptionParticipant::on_type_information_received(
        eprosima::fastdds::dds::DomainParticipant* participant,
        const fastrtps::string_255 /* topic_name */,
        const fastrtps::string_255 type_name,
        const fastrtps::types::TypeInformation& type_information)
{
    std::string type_name_ = type_name.to_string();
    const TypeIdentifier* type_identifier = nullptr;
    const TypeObject* type_object = nullptr;
    DynamicType_ptr dynamic_type(nullptr);

    if (type_name_ == "DdsRecorderCommand")
    {
        logError(DEBUG_COMMAND,
            "Participant " << this->id() << " received type information for " << type_name_);
    }

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

void DynTypesSubscriptionParticipant::internal_notify_type_object_(
        eprosima::fastrtps::types::DynamicType_ptr dynamic_type)
{
    if (dynamic_type->get_name() == "DdsRecorderCommand")
    {
        logError(DEBUG_COMMAND,
                "Participant " << this->id() << " discovered type object " << dynamic_type->get_name());
    }
    else
    {
        logInfo(DDSPIPE_DYNTYPES_PARTICIPANT,
                "Participant " << this->id() << " discovered type object " << dynamic_type->get_name());
    }

    // Create data containing Dynamic Type
    auto data = std::make_unique<DynamicTypeData>();
    data->dynamic_type = dynamic_type; // TODO: add constructor with param

    // Insert new data in internal reader queue
    type_object_reader_->simulate_data_reception(std::move(data));
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
