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

/**
 * @file YamlValidator.cpp
 *
 */

#include <cpp_utils/Log.hpp>
#include <ddspipe_yaml/YamlValidator.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

bool YamlValidator::validate_tags(
        const Yaml& yml,
        const std::set<TagType>& valid_tags)
{
    if (!yml.IsMap() && !yml.IsNull())
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "The yml: <" << yml << "> is not a yaml object map.");
    }

    // Check if there are any extra tags that are not in the list
    std::map<TagType, int> tags_count;
    bool is_valid = true;

    for (const auto& tag_it : yml)
    {
        const auto& tag = tag_it.first.as<TagType>();

        // Check if the tag is valid
        if (!valid_tags.count(tag))
        {
            logWarning(DDSPIPE_YAML, "Tag <" << tag << "> is not a valid tag.");
            is_valid = false;
        }

        // Check if the tag is repeated
        tags_count[tag]++;

        if (tags_count[tag] > 1)
        {
            logWarning(DDSPIPE_YAML, "Tag <" << tag << "> is repeated.");
            is_valid = false;
        }
    }

    return is_valid;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
