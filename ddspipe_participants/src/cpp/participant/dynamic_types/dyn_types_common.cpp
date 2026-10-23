// Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
 * @file dyn_types_common.cpp
 */

#include <memory>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObject.hpp>

#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Formatter.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>
#include <ddspipe_core/types/dynamic_types/types.hpp>

#include "dyn_types_common.hpp"
#include "idl_types.hpp"

namespace eprosima {
namespace ddspipe {
namespace participants {

void load_configured_idl_types(
        const SimpleParticipantConfiguration& configuration,
        std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>& idl_types)
{
    const auto& idl_types_paths = configuration.idl_types_paths;

    // Other failures only affect some types, and are already logged
    if (!idl_types_paths.empty() &&
            register_idl_types({idl_types_paths.begin(), idl_types_paths.end()}, configuration.idl_include_paths,
            idl_types) == utils::ReturnCode::RETCODE_PRECONDITION_NOT_MET)
    {
        throw utils::InitializationException(STR_ENTRY << "Error loading the IDL types of Participant "
                                                       << configuration.id << ".");
    }
}

void notify_type_discovered(
        const fastdds::dds::xtypes::TypeInformation& type_info,
        const std::string& type_name,
        const std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>& idl_types,
        bool idl_types_priority,
        const core::types::ParticipantId& participant_id,
        std::set<std::string>& received_types,
        InternalReader& type_object_reader)
{
    // Check if it exists already
    if (received_types.find(type_name) != received_types.end())
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DYNTYPES_PARTICIPANT,
                "Type " << type_name << " was already received, aborting propagation.");
        return;
    }

    auto type_identifier = type_info.complete().typeid_with_size().type_id();

    // The types loaded from IDL are matched by name, and are used either by priority or because the endpoint
    // did not share its type
    const auto idl_type = idl_types.find(type_name);

    if (idl_type != idl_types.end() &&
            (idl_types_priority || fastdds::dds::xtypes::TK_NONE == type_identifier._d()))
    {
        type_identifier = idl_type->second;
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
            "Participant " << participant_id << " discovered type object " << dyn_type->get_name());

    monitor_type_discovered(type_name);

    // If not, add it to the received types set
    received_types.insert(type_name);

    // Create data containing Dynamic Type
    auto data = std::make_unique<core::types::DynamicTypeData>();
    data->dynamic_type = dyn_type; // TODO: add constructor with param
    data->type_identifier = type_identifier;

    // Insert new data in internal reader queue
    type_object_reader.simulate_data_reception(std::move(data));
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
