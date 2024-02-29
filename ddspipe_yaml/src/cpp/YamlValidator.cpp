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
        const std::set<TagType>& tags)
{
    if (!yml.IsMap() && !yml.IsNull())
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "The yml: <" << yml << "> is not a yaml object map.");
    }

    // Check if there are any extra tags that are not in either list
    bool has_extra_tags = false;

    for (const auto& tag_it : yml)
    {
        const auto& tag = tag_it.first.as<TagType>();

        if (!tags.count(tag))
        {
            logWarning(DDSPIPE_YAML, "Tag <" << tag << "> is not a valid tag.");
            has_extra_tags = true;
        }
    }

    return !has_extra_tags;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
