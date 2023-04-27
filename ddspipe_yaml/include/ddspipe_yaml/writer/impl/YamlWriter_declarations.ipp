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
// DEFINITION OF SPECIFIC TYPES
////////////////////////////////////////////////

template <typename T>
void write_enumeration(
        Yaml& yml,
        const T& object,
        const std::map<TagType, T>& enum_values);

////////////////////////////////////////////////
// DEFINITION OF FUNCTIONS FOR TEMPLATED TYPES
////////////////////////////////////////////////

template <typename T>
void write_fuzzy(
        Yaml& yml,
        const utils::Fuzzy<T>& object);

template <typename T>
void write_vector(
        Yaml& yml,
        const std::vector<T>& object);

template <typename K, typename T>
void write_map(
        Yaml& yml,
        const std::map<K, T>& object);

////////////////////////////////////////////////
// DEFINITION OF FUNCTIONS FOR TRIVIAL TYPES
////////////////////////////////////////////////
// These are needed for Dll export

template <>
void write(
        Yaml& yml,
        const std::string& object);

template <>
void write(
        Yaml& yml,
        const bool& object);

template <>
void write(
        Yaml& yml,
        const int& object);

////////////////////////////////////////////////
// SPECIALIZATION DECLARATIONS FOR TEMPLATED TYPES
////////////////////////////////////////////////

template <typename T>
void write(
        Yaml& yml,
        const TagType& tag,
        const utils::Fuzzy<T>& object);

template <typename T>
void write(
        Yaml& yml,
        const utils::Fuzzy<T>& object);

template <typename T>
void write(
        Yaml& yml,
        const std::vector<T>& object);

template <typename T>
void write(
        Yaml& yml,
        const std::set<T>& object);

template <typename K, typename T>
void write(
        Yaml& yml,
        const std::map<K, T>& object);

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
