// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <set>

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/Yaml.hpp>
#include <ddspipe_yaml/YamlReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

/**
 * @brief Yaml Validator
 *
 * This class is used to validate Yaml objects.
 */
class YamlValidator
{
public:

    /**
     * @brief Validate a Yaml object against a specific version.
     *
     * For each type, the function should call \c validate_tags with the maximum set of tags \c yml may contain.
     *
     * @tparam T Type of the object to validate.
     * @param yml Yaml object to validate.
     * @param version Version to validate against.
     */
    DDSPIPE_YAML_DllAPI
    template <typename T>
    static bool validate(
            const Yaml& yml,
            const YamlReaderVersion& version = YamlReaderVersion::LATEST);

    /**
     * @brief Ensure that all the tags in \c yml are in \c tags.
     *
     * @param yml Yaml object to validate.
     * @param tags Set of tags to validate against.
     */
    DDSPIPE_YAML_DllAPI
    static bool validate_tags(
            const Yaml& yml,
            const std::set<TagType>& valid_tags);

protected:

    /**
     * @brief Get the position of the yaml node in the yaml file
     *
     * If the position is not available, it will return "unknown position"
     *
     * @param yml base yaml
     * @return "line X, column Y"
     */
    static std::string get_position_(
            const Yaml& yml);
};

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
