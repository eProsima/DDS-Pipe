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

#include <cpp_utils/ReturnCode.hpp>

#include <ddspipe_participants/reader/auxiliar/InternalReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

//! Shared implementation of the participants' \c load_idl_types , inserting the types in \c type_object_reader
utils::ReturnCode load_idl_types_into(
        const std::string& idl_path,
        InternalReader& type_object_reader);

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
