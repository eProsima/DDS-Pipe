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

#include <map>
#include <string>
#include <vector>

#include <fastdds/dds/xtypes/type_representation/detail/dds_xtypes_typeobject.hpp>

#include <cpp_utils/ReturnCode.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

//! Load and register the types of IDL files, storing their complete type identifiers by type name
utils::ReturnCode register_idl_types(
        const std::vector<std::string>& idl_paths,
        const std::vector<std::string>& include_paths,
        std::map<std::string, fastdds::dds::xtypes::TypeIdentifier>& idl_types);

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
