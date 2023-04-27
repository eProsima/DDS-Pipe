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

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// DEFINITION OF FUNCTIONS FOR TEMPLATED TYPES
////////////////////////////////////////////////

template <typename T>
void read_fuzzy(
        const Yaml& yml,
        utils::Fuzzy<T>& fuzzy)
{
    read<T>(yml, fuzzy.get_reference());
    fuzzy.set_level();
}

////////////////////////////////////////////////
// SPECIALIZATION DECLARATIONS FOR TEMPLATED TYPES
////////////////////////////////////////////////

template <typename T>
void read(
        const Yaml& yml,
        utils::Fuzzy<T>& fuzzy)
{
    read_fuzzy(yml, fuzzy);
}

////////////////////////////////////////////////
// SPECIALIZATIONS OF SPECIFIC FUNCTIONS
////////////////////////////////////////////////

template <typename T>
void read(
        const Yaml& yml,
        const TagType& tag,
        T& object)
{
    // ATTENTION: This try catch can be avoided, it is only used to add verbose information
    try
    {
        return read<T>(get_value_in_tag(yml, tag), object);
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() <<
                      "Error reading object of type <" << TYPE_NAME(T) <<
                      "> in tag <" << tag << "> :\n " << e.what());
    }
}


template <typename T>
T read(
        const Yaml& yml)
{
    T object;
    read<T>(yml, object);
    return object;
}

template <typename T>
T read(
        const Yaml& yml,
        const TagType& tag)
{
    T object;
    read<T>(yml, tag, object);
    return object;
}

template <typename T>
void operator <<(
    T& object,
    const Yaml& yml)
{
    read<T>(yml, object);
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
