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
 * @file idl_types.cpp
 */

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/type_representation/ITypeObjectRegistry.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObjectUtils.hpp>

#include <cpp_utils/Log.hpp>

#include <ddspipe_participants/utils/idl_types.hpp>

#include "idl_types.hpp"

namespace eprosima {
namespace ddspipe {
namespace participants {

utils::ReturnCode register_idl_types(
        const std::vector<std::string>& idl_paths,
        const std::vector<std::string>& include_paths,
        std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>& idl_types)
{
    std::vector<fastdds::dds::DynamicType::_ref_type> types;
    const auto ret = load_idl_types(idl_paths, include_paths, types);

    auto& type_object_registry = fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry();

    for (const auto& type : types)
    {
        fastdds::dds::xtypes::TypeIdentifierPair type_identifiers;
        type_object_registry.register_typeobject_w_dynamic_type(type, type_identifiers);

        // NOTE: retrieve_complete_type_identifier sets the flag to true on success
        bool found = false;
        const auto& type_identifier =
                fastdds::dds::xtypes::TypeObjectUtils::retrieve_complete_type_identifier(type_identifiers, found);

        if (!found)
        {
            EPROSIMA_LOG_WARNING(DDSPIPE_IDL_TYPES, "Failed to register type " << type->get_name() << ".");
            continue;
        }

        idl_types[type->get_name().to_string()] = type_identifier;
    }

    return ret;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
