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

#include <tuple>
#include <unordered_set>

#include <cpp_utils/Log.hpp>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObject.hpp>
#include <fastdds/rtps/attributes/RTPSParticipantAttributes.h>
#include <fastdds/rtps/builtin/data/ReaderProxyData.h>
#include <fastdds/rtps/builtin/data/WriterProxyData.h>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <fastrtps/rtps/participant/RTPSParticipant.h>
#include <fastrtps/rtps/RTPSDomain.h>

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
using namespace eprosima::ddspipe::core::types;
using namespace eprosima::fastdds::dds::xtypes;

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
    // Do nothing
}

void DynTypesParticipant::init()
{
    CommonParticipant::init();
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

void DynTypesParticipant::onReaderDiscovery(
            fastrtps::rtps::RTPSParticipant* participant,
            fastrtps::rtps::ReaderDiscoveryInfo&& info)
{
    fastrtps::rtps::ReaderProxyData proxy_copy(info.info);
    // Get type information
    const auto type_info = proxy_copy.type_information().type_information;
    const auto type_name = proxy_copy.typeName();
    rtps::CommonParticipant::onReaderDiscovery(participant, std::move(info));
    on_type_discovery_(participant, type_info, type_name);
}

void DynTypesParticipant::onWriterDiscovery(
            fastrtps::rtps::RTPSParticipant* participant,
            fastrtps::rtps::WriterDiscoveryInfo&& info)
{
    fastrtps::rtps::WriterProxyData proxy_copy(info.info);
    // Get type information
    const auto type_info = proxy_copy.type_information().type_information;
    const auto type_name = proxy_copy.typeName();
    rtps::CommonParticipant::onWriterDiscovery(participant, std::move(info));
    on_type_discovery_(participant, type_info, type_name);
}

void DynTypesParticipant::on_type_discovery_(
            fastrtps::rtps::RTPSParticipant* participant,
            const fastdds::dds::xtypes::TypeInformation& type_info,
            const fastcdr::string_255& type_name)
{
    auto type_identifier = type_info.complete().typeid_with_size().type_id();
    fastdds::dds::xtypes::TypeObject dyn_type_object;
    if (fastdds::dds::RETCODE_OK != fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_object(
            type_identifier,
            dyn_type_object))
    {
        return;
    }

    // TODO
    fastdds::dds::DynamicType::_ref_type dyn_type = fastdds::dds::DynamicTypeBuilderFactory::get_instance()->create_type_w_type_object(
                dyn_type_object)->build();
    if (!dyn_type)
    {
        return;
    }
    else
    {
        // Save type_identifier and its associated tyme_name. NOTE: We assume each type_name corresponds to only one type_identifier
        auto type_name_and_id = std::make_tuple(type_name, type_identifier);
        internal_notify_type_object_(dyn_type, type_name_and_id);
    }
}

void DynTypesParticipant::internal_notify_type_object_(
        fastdds::dds::DynamicType::_ref_type dynamic_type,
        const std::tuple<fastcdr::string_255, fastdds::dds::xtypes::TypeIdentifier>& type_name_and_id)
{
    logInfo(DDSPIPE_DYNTYPES_PARTICIPANT,
            "Participant " << this->id() << " discovered type object " << dynamic_type->get_name());

    monitor_type_discovered((dynamic_type->get_name()).to_string());

    // Create data containing Dynamic Type
    auto data = std::make_unique<DynamicTypeData>();
    data->dynamic_type = dynamic_type; // TODO: add constructor with param
    data->type_ids_tuple = type_name_and_id;

    // Insert new data in internal reader queue
    type_object_reader_->simulate_data_reception(std::move(data));
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
