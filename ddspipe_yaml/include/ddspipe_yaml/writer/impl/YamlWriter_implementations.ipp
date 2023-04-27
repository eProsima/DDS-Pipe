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

#include <cpp_utils/utils.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// SPECIALIZATIONS FOR TEMPLATED TYPES
////////////////////////////////////////////////

template <typename T>
void write(
        Yaml& yml,
        const utils::Fuzzy<T>& fuzzy)
{
    write_fuzzy<T>(yml, fuzzy);
}

template <typename T>
void write(
        Yaml& yml,
        const std::vector<T>& collection)
{
    write_collection(yml, collection);
}

template <typename T>
void write(
        Yaml& yml,
        const std::set<T>& collection)
{
    write_collection(yml, std::vector<T>(collection.begin(), collection.end()));
}

template <typename K, typename T>
void write(
        Yaml& yml,
        const std::map<K, T>& collection)
{
    write_map(yml, collection);
}

////////////////////////////////////////////////
// SPECIALIZATIONS OF SPECIFIC FUNCTIONS
////////////////////////////////////////////////

template <typename T>
void write(
        Yaml& yml,
        const TagType& tag,
        const T& value)
{
    auto yml_under_tag = add_tag(yml, tag);
    write(yml_under_tag, value);
}

template <typename T>
void operator <<(
        Yaml& yml,
        const T& object)
{
    write<T>(yml, object);
}

////////////////////////////////////////////////
// SPECIALIZATIONS OF SPECIFIC TYPES FUNCTIONS
////////////////////////////////////////////////

template <typename T>
void write_fuzzy(
        Yaml& yml,
        const utils::Fuzzy<T>& fuzzy)
{
    write<T>(yml, fuzzy.get_reference());
}

template <typename T>
void write_collection(
        Yaml& yml,
        const std::vector<T>& collection)
{
    for (const auto& v : collection)
    {
        Yaml yml_value;
        write<T>(yml_value, v);
        yml.push_back(yml_value);
    }
}

template <typename K, typename T>
void write_map(
        Yaml& yml,
        const std::map<K, T>& collection)
{
    for (const auto& v : collection)
    {
        Yaml yml_in_new_tag = yml[utils::generic_to_string(v.first)];
        write<T>(yml_in_new_tag, v.second);
    }
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
