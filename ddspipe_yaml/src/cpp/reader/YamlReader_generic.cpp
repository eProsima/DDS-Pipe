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

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/reader/YamlReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// IMPLEMENTATION OF TRIVIAL TYPES
////////////////////////////////////////////////

template <>
void read(
        const Yaml& yml,
        std::string& object)
{
    object = yml.as<std::string>();
}

template <>
void read(
        const Yaml& yml,
        int& object)
{
    object = yml.as<int>();
}

template <>
void read(
        const Yaml& yml,
        unsigned int& object)
{
    int x = yml.as<int>();
    if (x < 0)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Expected to read a positive integer.");
    }
    object = x;
}

template <>
void read(
        const Yaml& yml,
        float& object)
{
    object = yml.as<float>();
}

template <>
void read(
        const Yaml& yml,
        bool& object)
{
    object = yml.as<bool>();
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
