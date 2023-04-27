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

// !! WARNING !!
// NO RESPONSIBILITY SHALL BE TAKEN FOR ANY ISSUES CAUSED BY THE MISUSE OF THIS FILE
// This file is meant for developers only, and must not be used from any non expert.
// Only classes and methods in this file should be used, and not the ones in .ipp,
// and only inside read and write YAML methods

#pragma once

#include <vector>

#include <cpp_utils/memory/Heritable.hpp>
#include <cpp_utils/macros/custom_enumeration.hpp>

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/core/Yaml.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

ENUMERATION_BUILDER
(
    OptionalKind,
    required,
    optional,
    advisable,
    extra
);

// Forward declaration
class IYamlObjectField;

template <typename T>
utils::Heritable<IYamlObjectField> create_object_field(
        const std::string& tag,
        const OptionalKind& kind,
        const T& vessel);

template <typename T>
std::vector<utils::Heritable<IYamlObjectField>> object_fields(const T& object);

template <typename T>
void read_from_fields(
        const Yaml& yml,
        T& object);

template <typename T>
void write_from_fields(
        Yaml& yml,
        const T& object);

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */

// Include implementation template file
#include <ddspipe_yaml/field/impl/YamlObjectField.ipp>
