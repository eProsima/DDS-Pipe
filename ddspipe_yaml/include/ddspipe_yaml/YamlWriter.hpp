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
#include <ddspipe_yaml/Yaml.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

/**
 * @brief Add a new yaml under tag \c tag .
 *
 * It creates a new yaml under the given one indexed by tag.
 * If the tag already existed, it reuses the yaml already there, unless \c initialize is set to true, in which
 * case it always must be reset.
 *
 * @param yml yaml base where new tag will be added.
 * @param tag name of the tad.
 * @param initialize whether the new yaml must be initialized (reset). This only affects if tag already existed.
 * @param overwrite whether the initialization must overwrite the existing flag.
 *
 * Using initialize=true and and overwrite=false leads to an error if tag already exist.
 *
 * @return Yaml new yaml under \c tag .
 *
 * @throw \c PreconditionNotMet if \c tag already exist, \c initialize set to true and \c overwrite set to false.
 */
DDSPIPE_YAML_DllAPI
Yaml add_tag(
        Yaml& yml,
        const TagType& tag,
        bool initialize = false,
        bool overwrite = true);

/**
 * @brief Set a new value in \c yml .
 *
 * This method is thought to be specialized for each different type coding the serialization method to a yaml.
 *
 * @param yml base yaml where to write the value
 * @param value value to write
 *
 * @tparam T type of the value to set in the yaml.
 *
 * @note some specializations are already implemented:
 * - native types (int, string, boolean)
 * - collections (vector, set, map)
 */
template <typename T>
void set(
        Yaml& yml,
        const T& value);

//! Set the \c value in a new yaml in \c yml under \c tag .
template <typename T>
void set(
        Yaml& yml,
        const TagType& tag,
        const T& value);

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */

// Include implementation template file
#include <ddspipe_yaml/impl/YamlWriter.ipp>
