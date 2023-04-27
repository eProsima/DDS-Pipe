// Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <string>

#include <yaml-cpp/yaml.h>

#include <ddspipe_yaml/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace yaml {

/**
 * Configuration is in dictionary format
 *
 * YAML spec: https://yaml.org/spec/1.2.2/
 *
 * @note It is not legal to repeat keys in a YAML
 *
 * @note this object contains a reference to the actual yaml object. Thus passing it by value or by reference
 * makes no different.
 */
using Yaml = YAML::Node;

//! Type of tag in the yaml
using TagType = std::string;

/**
 * @brief Whether key \c tag is present in \c yml
 *
 * The key \c tag must be in base level of \c yml , it will no be looked for recursively.
 *
 * It could only look for keys in yaml map and empty yaml. It will fail in array and scalar cases.
 * In this second case, return will always be false.
 *
 * @param yml base yaml
 * @param tag key to look for in \c yml
 * @return true id \c tag is a key in \c yml
 * @return false otherwise
 *
 * @throw \c ConfigurationException if \c yml is not a map or empty yaml
 */
DDSPIPE_YAML_DllAPI
bool is_tag_present(
        const Yaml& yml,
        const TagType& tag);

/**
 * @brief Get the yaml inside key \c tag in \c yml
 *
 * \c tag must be present in \c yml . It uses \c is_tag_present to check the key is present.
 * Use \c is_tag_present before calling this method in order to avoid the exception.
 *
 * @param yml base yaml
 * @param tag key refering the value returned
 * @return yaml value inside key \c tag
 *
 * @throw \c ConfigurationException if \c tag is not present in \c yml
 */
DDSPIPE_YAML_DllAPI
Yaml get_value_in_tag(
        const Yaml& yml,
        const TagType& tag);

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

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
