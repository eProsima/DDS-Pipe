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

#include <cpp_utils/exception/PreconditionNotMet.hpp>

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/YamlReader.hpp>
#include <ddspipe_yaml/YamlWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

Yaml add_tag(
        Yaml& yml,
        const TagType& tag,
        bool initialize /* = false */,
        bool overwrite /* = true */)
{
    // If node must be initialized and could not overwrite, and node already exist, throw failure.
    if (initialize && !overwrite && YamlReader::is_tag_present(yml, tag))
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

template <>
void set(
        Yaml& yml,
        const std::string& value)
{
    yml = value;
}

template <>
void set(
        Yaml& yml,
        const bool& value)
{
    yml = value;
}

template <>
void set(
        Yaml& yml,
        const int& value)
{
    yml = value;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
