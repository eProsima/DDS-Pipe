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

#include <yaml-cpp/yaml.h>

#include <cpp_utils/exception/ConfigurationException.hpp>
#include <cpp_utils/exception/PreconditionNotMet.hpp>

#include <ddspipe_yaml/core/Yaml.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

bool is_tag_present(
        const Yaml& yml,
        const TagType& tag)
{
    if (!yml.IsMap() && !yml.IsNull())
    {
        throw eprosima::utils::PreconditionNotMet(
                  utils::Formatter() << "Trying to find a tag: <" << tag << "> in a not yaml object map.");
    }

    // Explicit conversion to avoid windows format warning
    // This method performace is the same as only retrieving bool
    return (yml[tag]) ? true : false;
}

Yaml get_value_in_tag(
        const Yaml& yml,
        const TagType& tag)
{
    if (is_tag_present(yml, tag))
    {
        return yml[tag];
    }
    else
    {
        throw eprosima::utils::PreconditionNotMet(
                  utils::Formatter() << "Required tag not found: <" << tag << ">.");
    }
}

Yaml add_tag(
        Yaml& yml,
        const TagType& tag,
        bool initialize /* = false */,
        bool overwrite /* = true */)
{
    // If node must be initialized and could not overwrite, and node already exist, throw failure.
    if (initialize && !overwrite && is_tag_present(yml, tag))
    {
        throw utils::PreconditionNotMet(
                  STR_ENTRY
                      << "Tag <" << tag
                      << "> already exist and could not overwrite.");
    }

    // If node must be initialized, independent on if it exist or not, create a new one and set.
    if (initialize && overwrite)
    {
        yml.remove(tag);
        return yml[tag];
    }

    // If node must not be initialized and exist or
    // node must be created
    // This call reuses yaml inside or create it if it does not exist
    return yml[tag];
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
