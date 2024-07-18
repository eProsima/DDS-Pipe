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

    // Check if there are repeated tags or extra tags that are not in valid_tags
    std::map<TagType, int> tags_count;
    bool is_valid = true;

    for (const auto& tag_it : yml)
    {
        const auto& tag = tag_it.first;
        const auto& tag_name = tag.as<TagType>();

        // Check if the tag is valid
        if (!valid_tags.count(tag_name))
        {
            logWarning(DDSPIPE_YAML, "Tag <" << tag_name << "> is not a valid tag (" << get_position_(tag) << ").");
            is_valid = false;
        }

        // Check if the tag is repeated
        tags_count[tag_name]++;

        if (tags_count[tag_name] > 1)
        {
            logWarning(DDSPIPE_YAML, "Tag <" << tag_name << "> is repeated (" << get_position_(tag) << ").");
            is_valid = false;
        }
    }

    return is_valid;
}

std::string YamlValidator::get_position_(
        const Yaml& yml)
{
    int line;
    int column;

    try
    {
        const auto& mark = yml.Mark();

        if (mark.is_null())
        {
            return "unknown position";
        }

        line = mark.line + 1;
        column = mark.column + 1;
    }
    catch (const std::exception& e)
    {
        return "unknown position";
    }

    return "line " + std::to_string(line) + ", column " + std::to_string(column);
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
