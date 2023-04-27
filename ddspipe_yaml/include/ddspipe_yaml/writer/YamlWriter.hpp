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

#pragma once

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/core/Yaml.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

/**
 * @brief Set a new object in \c yml .
 *
 * This method is thought to be specialized for each different type coding the serialization method to a yaml.
 *
 * @param yml base yaml where to write the object
 * @param object object to write
 *
 * @tparam T type of the object to set in the yaml.
 *
 * @note some specializations are already implemented:
 * - native types (int, string, boolean)
 * - collections (vector, set, map)
 */
template <typename T>
void write(
        Yaml& yml,
        const T& object);

//! Set the \c object in a new yaml in \c yml under \c tag .
template <typename T>
void write(
        Yaml& yml,
        const TagType& tag,
        const T& object);


//! Fill Yaml structure with \c object
template <typename T>
void operator <<(
        Yaml& yml,
        const T& object);

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */

// Include implementation template file
#include <ddspipe_yaml/writer/impl/YamlWriter_declarations.ipp>
#include <ddspipe_yaml/writer/impl/YamlWriter_implementations.ipp>
