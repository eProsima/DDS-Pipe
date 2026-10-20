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
 * @file idl_types.hpp
 */

#pragma once

#include <string>
#include <vector>

#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>

#include <cpp_utils/ReturnCode.hpp>

#include <ddspipe_participants/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * @brief Load the structures declared in IDL files.
 *
 * The types are not registered anywhere, so it is up to the caller to decide what to do with them.
 * A path that fails does not stop the others from being loaded.
 *
 * @param idl_paths IDL files, or directories searched recursively for IDL files.
 * @param include_paths Directories searched for the files included by the IDL files.
 * @param types Vector where the loaded types are appended.
 * @return \c RETCODE_OK if every file was loaded, \c RETCODE_ERROR if some failed, \c RETCODE_NO_DATA if no
 * types were found, and \c RETCODE_PRECONDITION_NOT_MET if a path does not exist or cannot be read.
 */
DDSPIPE_PARTICIPANTS_DllAPI
utils::ReturnCode load_idl_types(
        const std::vector<std::string>& idl_paths,
        const std::vector<std::string>& include_paths,
        std::vector<fastdds::dds::DynamicType::_ref_type>& types);

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
