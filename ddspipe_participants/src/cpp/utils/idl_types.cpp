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

#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>

#include <cpp_utils/Log.hpp>

#include <ddspipe_participants/utils/idl_types.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

utils::ReturnCode load_idl_types(
        const std::vector<std::string>& idl_paths,
        const std::vector<std::string>& include_paths,
        std::vector<fastdds::dds::DynamicType::_ref_type>& types)
{
    const auto previous_size = types.size();
    bool path_not_found = false;
    bool files_failed = false;

    for (const auto& idl_path : idl_paths)
    {
        const auto ret = fastdds::dds::DynamicTypeBuilderFactory::get_instance()->for_each_type_w_uri(
            idl_path,
            include_paths,
            [&](fastdds::dds::DynamicTypeBuilder::_ref_type builder)
            {
                // Only structures can be the type of a topic
                if (builder->get_kind() != fastdds::dds::TK_STRUCTURE)
                {
                    return true;
                }

                const auto dynamic_type = builder->build();

                if (!dynamic_type)
                {
                    EPROSIMA_LOG_WARNING(DDSPIPE_IDL_TYPES,
                    "Failed to build type " << builder->get_name() << " from IDL path " << idl_path << ".");
                    return true;
                }

                EPROSIMA_LOG_INFO(DDSPIPE_IDL_TYPES,
                "Loaded type " << dynamic_type->get_name() << " from IDL path " << idl_path << ".");

                types.push_back(dynamic_type);

                return true;
            });

        if (ret == fastdds::dds::RETCODE_BAD_PARAMETER)
        {
            path_not_found = true;
        }
        else if (ret != fastdds::dds::RETCODE_OK)
        {
            EPROSIMA_LOG_WARNING(DDSPIPE_IDL_TYPES,
                    "Some IDL files in IDL path " << idl_path << " could not be loaded.");
            files_failed = true;
        }
    }

    if (path_not_found)
    {
        return utils::ReturnCode::RETCODE_PRECONDITION_NOT_MET;
    }

    if (files_failed)
    {
        return utils::ReturnCode::RETCODE_ERROR;
    }

    if (types.size() == previous_size)
    {
        EPROSIMA_LOG_WARNING(DDSPIPE_IDL_TYPES, "No types were loaded from the IDL paths.");
        return utils::ReturnCode::RETCODE_NO_DATA;
    }

    return utils::ReturnCode::RETCODE_OK;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
