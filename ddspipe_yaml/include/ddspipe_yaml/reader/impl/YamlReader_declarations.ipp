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
T read_enumeration(
        const Yaml& yml,
        const std::map<TagType, T>& enum_values);

////////////////////////////////////////////////
// DEFINITION OF FUNCTIONS FOR TEMPLATED TYPES
////////////////////////////////////////////////

template <typename T>
void read_fuzzy(
        const Yaml& yml,
        utils::Fuzzy<T>& fuzzy);

template <typename T>
void read_collection(
        const Yaml& yml,
        std::vector<T>& collection);

template <typename K, typename T>
void read_map(
        const Yaml& yml,
        std::map<K, T>& collection);

////////////////////////////////////////////////
// SPECIALIZATION DECLARATIONS FOR TEMPLATED TYPES
////////////////////////////////////////////////

template <typename T>
void read(
        const Yaml& yml,
        utils::Fuzzy<T>& fuzzy);

template <typename T>
void read(
        const Yaml& yml,
        std::vector<T>& collection);

template <typename T>
void read(
        const Yaml& yml,
        std::set<T>& collection);

template <typename K, typename T>
void read(
        const Yaml& yml,
        std::map<K, T>& collection);

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
