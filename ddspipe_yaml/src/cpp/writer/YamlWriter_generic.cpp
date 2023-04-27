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
#include <ddspipe_yaml/reader/YamlReader.hpp>
#include <ddspipe_yaml/writer/YamlWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// IMPLEMENTATION OF TRIVIAL TYPES
////////////////////////////////////////////////

template <>
void write(
        Yaml& yml,
        const std::string& value)
{
    yml = value;
}

template <>
void write(
        Yaml& yml,
        const int& value)
{
    yml = value;
}

template <>
void write(
        Yaml& yml,
        const unsigned int& value)
{
    int x = value;
    if (x < 0)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Expected to write a positive integer.");
    }
    yml = value;
}

template <>
void write(
        Yaml& yml,
        const bool& value)
{
    yml = value;
}

template <>
void write(
        Yaml& yml,
        const float& value)
{
    yml = value;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
