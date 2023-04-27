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

#include <cpp_utils/types/Fuzzy.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// DEFINITION OF FUNCTIONS FOR TEMPLATED TYPES
////////////////////////////////////////////////

template <typename T>
void write_fuzzy(
        Yaml& yml,
        const utils::Fuzzy<T>& fuzzy);

template <typename T>
void write_collection(
        Yaml& yml,
        const std::vector<T>& collection);

template <typename K, typename T>
void write_map(
        Yaml& yml,
        const std::map<K, T>& collection);

////////////////////////////////////////////////
// DEFINITION OF FUNCTIONS FOR TRIVIAL TYPES
////////////////////////////////////////////////
// These are needed for Dll export

template <>
DDSPIPE_YAML_DllAPI
void write(
        Yaml& yml,
        const std::string& value);

template <>
DDSPIPE_YAML_DllAPI
void write(
        Yaml& yml,
        const bool& value);

template <>
DDSPIPE_YAML_DllAPI
void write(
        Yaml& yml,
        const int& value);

////////////////////////////////////////////////
// SPECIALIZATION DECLARATIONS FOR TEMPLATED TYPES
////////////////////////////////////////////////

template <typename T>
void write(
        Yaml& yml,
        const utils::Fuzzy<T>& collection);

template <typename T>
void write(
        Yaml& yml,
        const std::vector<T>& collection);

template <typename T>
void write(
        Yaml& yml,
        const std::set<T>& collection);

template <typename K, typename T>
void write(
        Yaml& yml,
        const std::map<K, T>& collection);

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
