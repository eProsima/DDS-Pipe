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

#include <cstddef>
#include <memory>

#include <fastdds/config.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/type_representation/ITypeObjectRegistry.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObjectUtils.hpp>

#include <cpp_utils/Log.hpp>
#include <cpp_utils/utils.hpp>

#include <ddspipe_core/types/dynamic_types/types.hpp>

#include "idl_types.hpp"

// for_each_type_w_uri is available since Fast DDS 3.4.0
#define DDSPIPE_IDL_TYPES_SUPPORTED \
    (FASTDDS_VERSION_MAJOR > 3 || (FASTDDS_VERSION_MAJOR == 3 && FASTDDS_VERSION_MINOR >= 4))

namespace eprosima {
namespace ddspipe {
namespace participants {

utils::ReturnCode load_idl_types_into(
        const std::string& idl_path,
        InternalReader& type_object_reader)
{
    if (!utils::is_file_accessible(idl_path.c_str(), utils::FileAccessMode::exist))
    {
        EPROSIMA_LOG_ERROR(DDSPIPE_DYNTYPES_PARTICIPANT,
                "IDL types path " << idl_path << " does not exist.");
        return utils::ReturnCode::RETCODE_PRECONDITION_NOT_MET;
    }

#if DDSPIPE_IDL_TYPES_SUPPORTED
    auto& type_object_registry = fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry();
    std::size_t loaded_types = 0;

    const auto ret = fastdds::dds::DynamicTypeBuilderFactory::get_instance()->for_each_type_w_uri(
        idl_path,
        {},
        [&](fastdds::dds::DynamicTypeBuilder::_ref_type builder)
        {
            // Only structures and unions can be the type of a topic
            const auto kind = builder->get_kind();

            if (kind != fastdds::dds::xtypes::TK_STRUCTURE && kind != fastdds::dds::xtypes::TK_UNION)
            {
                return true;
            }

            const auto dynamic_type = builder->build();

            if (!dynamic_type)
            {
                EPROSIMA_LOG_WARNING(DDSPIPE_DYNTYPES_PARTICIPANT,
                "Failed to build type " << builder->get_name() << " from IDL types path " << idl_path << ".");
                return true;
            }

            // Consumers look up the type's dependencies in the registry
            fastdds::dds::xtypes::TypeIdentifierPair type_identifiers;
            type_object_registry.register_typeobject_w_dynamic_type(dynamic_type, type_identifiers);

            // NOTE: retrieve_complete_type_identifier sets the flag to true on success
            bool found = false;
            const auto& type_identifier =
            fastdds::dds::xtypes::TypeObjectUtils::retrieve_complete_type_identifier(type_identifiers, found);

            if (!found)
            {
                EPROSIMA_LOG_WARNING(DDSPIPE_DYNTYPES_PARTICIPANT,
                "Failed to register type " << dynamic_type->get_name() << " from IDL types path "
                                           << idl_path << ".");
                return true;
            }

            EPROSIMA_LOG_INFO(DDSPIPE_DYNTYPES_PARTICIPANT,
            "Loaded type " << dynamic_type->get_name() << " from IDL types path " << idl_path << ".");

            auto data = std::make_unique<core::types::DynamicTypeData>();
            data->dynamic_type = dynamic_type;
            data->type_identifier = type_identifier;

            type_object_reader.simulate_data_reception(std::move(data));
            loaded_types++;

            return true;
        });

    if (ret != fastdds::dds::RETCODE_OK)
    {
        EPROSIMA_LOG_WARNING(DDSPIPE_DYNTYPES_PARTICIPANT,
                "Some IDL files in IDL types path " << idl_path << " could not be loaded.");
        return utils::ReturnCode::RETCODE_ERROR;
    }

    if (loaded_types == 0)
    {
        EPROSIMA_LOG_WARNING(DDSPIPE_DYNTYPES_PARTICIPANT,
                "No types were loaded from IDL types path " << idl_path << ".");
        return utils::ReturnCode::RETCODE_NO_DATA;
    }

    return utils::ReturnCode::RETCODE_OK;
#else
    static_cast<void>(type_object_reader);

    EPROSIMA_LOG_ERROR(DDSPIPE_DYNTYPES_PARTICIPANT,
            "Cannot load IDL types path " << idl_path << ": loading types from IDL requires Fast DDS 3.4.0 or later.");
    return utils::ReturnCode::RETCODE_PRECONDITION_NOT_MET;
#endif // if DDSPIPE_IDL_TYPES_SUPPORTED
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
