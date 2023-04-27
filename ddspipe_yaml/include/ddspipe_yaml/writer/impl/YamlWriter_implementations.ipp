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

#include <cpp_utils/exception/InconsistencyException.hpp>
#include <cpp_utils/utils.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// DEFINITION OF SPECIFIC TYPES
////////////////////////////////////////////////

template <typename T>
void write_enumeration(
        Yaml& yml,
        const T& value,
        const std::map<TagType, T>& enum_values)
{
    // TODO check if value exists
    for (const auto& it : enum_values)
    {
        if (value == it.second)
        {
            write(yml, it.first);
        }
        return;
    }

    // If it finished without finding value, error
    throw utils::InconsistencyException(
            STR_ENTRY << "Value <" << value << "> of enumeration type <" << TYPE_NAME(T) << " has no parse value.");
}

////////////////////////////////////////////////
// SPECIALIZATIONS FOR TEMPLATED TYPES
////////////////////////////////////////////////

template <typename T>
void write(
        Yaml& yml,
        const TagType& tag,
        const utils::Fuzzy<T>& object)
{
    if (object.is_set())
    {
        write(yml, tag, object.get_reference());
    }
}

template <typename T>
void write(
        Yaml& yml,
        const utils::Fuzzy<T>& object)
{
    write_fuzzy<T>(yml, object);
}

template <typename T>
void write(
        Yaml& yml,
        const std::vector<T>& object)
{
    write_vector(yml, object);
}

template <typename T>
void write(
        Yaml& yml,
        const std::set<T>& object)
{
    write_vector(yml, std::vector<T>(object.begin(), object.end()));
}

template <typename K, typename T>
void write(
        Yaml& yml,
        const std::map<K, T>& object)
{
    write_map(yml, object);
}

////////////////////////////////////////////////
// SPECIALIZATIONS OF SPECIFIC FUNCTIONS
////////////////////////////////////////////////

template <typename T>
void write(
        Yaml& yml,
        const TagType& tag,
        const T& object)
{
    auto yml_under_tag = add_tag(yml, tag);
    write(yml_under_tag, object);
}

template <typename T>
void operator <<(
        Yaml& yml,
        const T& object)
{
    write(yml, object);
}

////////////////////////////////////////////////
// SPECIALIZATIONS OF SPECIFIC TYPES FUNCTIONS
////////////////////////////////////////////////

template <typename T>
void write_fuzzy(
        Yaml& yml,
        const utils::Fuzzy<T>& object)
{
    write(yml, object.get_reference());
}

template <typename T>
void write_vector(
        Yaml& yml,
        const std::vector<T>& object)
{
    for (const auto& v : object)
    {
        Yaml yml_value;
        write(yml_value, v);
        yml.push_back(yml_value);
    }
}

template <typename K, typename T>
void write_map(
        Yaml& yml,
        const std::map<K, T>& object)
{
    for (const auto& v : object)
    {
        Yaml yml_in_new_tag = yml[utils::generic_to_string(v.first)];
        write(yml_in_new_tag, v.second);
    }
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
