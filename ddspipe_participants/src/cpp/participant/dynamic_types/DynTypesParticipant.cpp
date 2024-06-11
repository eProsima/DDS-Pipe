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
#include <fastdds/rtps/attributes/RTPSParticipantAttributes.h>
#include <fastdds/rtps/builtin/data/ReaderProxyData.h>
#include <fastdds/rtps/builtin/data/WriterProxyData.h>
#include <fastdds/rtps/RTPSDomain.h>

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

    notify_type_discovered_(type_info, type_name);
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

    notify_type_discovered_(type_info, type_name);
}

void DynTypesParticipant::notify_type_discovered_(
            const fastdds::dds::xtypes::TypeInformation& type_info,
            const fastcdr::string_255& type_name)
{
    const auto type_identifier = type_info.complete().typeid_with_size().type_id();
    fastdds::dds::xtypes::TypeObject dyn_type_object;
    if (fastdds::dds::RETCODE_OK != fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_object(
            type_identifier,
            dyn_type_object))
    {
        logWarning(DDSPIPE_DYNTYPES_PARTICIPANT,
            "Failed to get type object of " << type_name << "type");
        return;
    }

    // Create Dynamic Type
    fastdds::dds::DynamicType::_ref_type dyn_type = fastdds::dds::DynamicTypeBuilderFactory::get_instance()->create_type_w_type_object(
                dyn_type_object)->build();
    if (!dyn_type)
    {
        logWarning(DDSPIPE_DYNTYPES_PARTICIPANT,
            "Failed to create Dynamic Type " << type_name);
        return;
    }

    // Notify type_identifier and its associated tyme_name.
    // NOTE: We assume each type_name corresponds to only one type_identifier
    logInfo(DDSPIPE_DYNTYPES_PARTICIPANT,
            "Participant " << this->id() << " discovered type object " << dyn_type->get_name());

    monitor_type_discovered(type_name.to_string());

    // Create data containing Dynamic Type
    auto data = std::make_unique<DynamicTypeData>();
    data->dynamic_type = dyn_type; // TODO: add constructor with param
    data->type_name = type_name;
    data->type_id = type_identifier;

    // Insert new data in internal reader queue
    type_object_reader_->simulate_data_reception(std::move(data));
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
