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
 * @file dyn_types_common.hpp
 */

#pragma once

#include <map>
#include <set>
#include <string>

#include <fastdds/dds/xtypes/type_representation/detail/dds_xtypes_typeobject.hpp>

#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_participants/configuration/SimpleParticipantConfiguration.hpp>
#include <ddspipe_participants/reader/auxiliar/InternalReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * Load and register the types of the configured IDL types paths, storing their complete type identifiers by type name
 *
 * @throw InitializationException if an IDL types path does not exist or cannot be read.
 */
void load_configured_idl_types(
        const SimpleParticipantConfiguration& configuration,
        std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>& idl_types);

//! Send the dynamic type of a discovered type through the type object reader, once per type name
void notify_type_discovered(
        const fastdds::dds::xtypes::TypeInformation& type_info,
        const std::string& type_name,
        const std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>& idl_types,
        bool idl_types_priority,
        const core::types::ParticipantId& participant_id,
        std::set<std::string>& received_types,
        InternalReader& type_object_reader);

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
