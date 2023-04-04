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

#include <map>
#include <set>
#include <string>
#include <vector>

#include <cpp_utils/utils.hpp>

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/Yaml.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

template <typename T>
void set(
        Yaml& yml,
        const std::vector<T>& collection);

template <typename T>
void set(
        Yaml& yml,
        const std::set<T>& collection);

template <typename K, typename T>
void set(
        Yaml& yml,
        const std::map<K, T>& collection);

template <typename T>
void set_collection(
        Yaml& yml,
        const std::vector<T>& collection);

template <typename K, typename T>
void set_map(
        Yaml& yml,
        const std::map<K, T>& collection);

////////////////////////////////////////////////
// SPECIALIZATIONS FOR TRIVIAL TYPES
////////////////////////////////////////////////

template <>
DDSPIPE_YAML_DllAPI
void set(
        Yaml& yml,
        const std::string& value);

template <>
DDSPIPE_YAML_DllAPI
void set(
        Yaml& yml,
        const bool& value);

template <>
DDSPIPE_YAML_DllAPI
void set(
        Yaml& yml,
        const int& value);

////////////////////////////////////////////////
// SPECIALIZATIONS FOR OTHER TYPES
////////////////////////////////////////////////

template <typename T>
void set(
        Yaml& yml,
        const std::vector<T>& collection)
{
    set_collection(yml, collection);
}

template <typename T>
void set(
        Yaml& yml,
        const std::set<T>& collection)
{
    set_collection(yml, std::vector<T>(collection.begin(), collection.end()));
}

template <typename K, typename T>
void set(
        Yaml& yml,
        const std::map<K, T>& collection)
{
    set_map(yml, collection);
}

////////////////////////////////////
// SPECIALIZATIONS NEEDED
////////////////////////////////////

template <typename T>
void set(
        Yaml& yml,
        const TagType& tag,
        const T& value)
{
    auto yml_under_tag = add_tag(yml, tag);
    set(yml_under_tag, value);
}

template <typename T>
void set_collection(
        Yaml& yml,
        const std::vector<T>& collection)
{
    for (const auto& v : collection)
    {
        Yaml yml_value;
        set<T>(yml_value, v);
        yml.push_back(yml_value);
    }
}

template <typename K, typename T>
void set_map(
        Yaml& yml,
        const std::map<K, T>& collection)
{
    for (const auto& v : collection)
    {
        Yaml yml_in_new_tag = yml[utils::generic_to_string(v.first)];
        set<T>(yml_in_new_tag, v.second);
    }
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
