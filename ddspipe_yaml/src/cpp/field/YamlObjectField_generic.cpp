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

#include <cpp_utils/Log.hpp>
#include <cpp_utils/utils.hpp>

#include <ddspipe_yaml/core/Yaml.hpp>
#include <ddspipe_yaml/field/YamlObjectField.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

void read_fields(
        const Yaml& yml,
        const std::vector<utils::Heritable<IYamlObjectField>>& fields,
        bool fail_with_extra_tags /* = false */,
        bool show_warning_with_extra_tags /* = true */)
{
    // Check that yml to find is actually a map or it is empty
    if (!yml.IsMap() && !yml.IsNull())
    {
        throw eprosima::utils::ConfigurationException(STR_ENTRY
                      << "Trying to get tags: in a not yaml object map.");
    }

    // First, check that every required flag is present
    for (const auto& field : fields)
    {
        field->read_field(yml);
    }

    // If it should not show warnings, neither fail, do not check extra args
    if (!fail_with_extra_tags && !show_warning_with_extra_tags)
    {
        return;
    }

    // Now that every required field is present, check for extra tags
    for (const auto& yaml_field : yml)
    {
        auto tag = yaml_field.first.as<std::string>();
        bool should_be_present = false;

        // For each tag, check if it is inside allowed tags
        for (const auto& field : fields)
        {
            if (field->tag() == tag)
            {
                should_be_present = true;
                break;
            }
        }

        // If present tag is not inside allowed ones, fail
        if (!should_be_present)
        {
            if (fail_with_extra_tags)
            {
                throw eprosima::utils::ConfigurationException(STR_ENTRY
                                << "Unexpected tag <" << tag << "> present.");
            }
            else if(show_warning_with_extra_tags)
            {
                logWarning(DDSPIPE_YAML, "Unexpected tag <" << tag << "> present.");
            }
        }
    }
}

void write_fields(
        Yaml& yml,
        const std::vector<utils::Heritable<IYamlObjectField>>& fields)
{
    for (const auto& field : fields)
    {
        field->write_field(yml);
    }
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
