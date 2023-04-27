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

#pragma once

#include <cpp_utils/types/Fuzzy.hpp>
#include <cpp_utils/enum/EnumBuilder.hpp>
#include <cpp_utils/exception/ConfigurationException.hpp>

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/core/Yaml.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

/**
 * @brief Fill an element given by parameter with the values inside \c yml
 *
 * This method simplifies the process of retrieving an object whose parent has its own \c read method,
 * as the code is reused from one another calling parent \c read in child.
 * It is also very helpful to handle default creation values. Without this, every different default value
 * must have its own if-else clause, forcing to create the respective constructor. With this method,
 * the default values are initialized with the default constructor, and then are overwritten by the yaml.
 * [this problem arises because C++ does not allow different order of parameters in method call]
 */
template <typename T>
void read(
        const Yaml& yml,
        T& object);

//! Fill an element given by parameter with the values inside \c tag in \c yml
template <typename T>
void read(
        const Yaml& yml,
        const TagType& tag,
        T& object);

template <typename T>
T read(
        const Yaml& yml);

template <typename T>
T read(
        const Yaml& yml,
        const TagType& tag);

//! Fill \c object with a Yaml structure
template <typename T>
void operator <<(
        T& object,
        const Yaml& yml);

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */

// Include implementation template file
#include <ddspipe_yaml/reader/impl/YamlReader_declarations.ipp>
#include <ddspipe_yaml/reader/impl/YamlReader_implementations.ipp>
