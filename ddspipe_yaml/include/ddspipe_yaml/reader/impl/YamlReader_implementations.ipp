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

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// DEFINITION OF SPECIFIC TYPES
////////////////////////////////////////////////

template <typename T>
T read_enumeration(
        const Yaml& yml,
        const std::map<TagType, T>& enum_values)
{
    TagType value;
    read<TagType>(yml, value);

    // Find value
    auto it = enum_values.find(value);

    if (it == enum_values.end())
    {
        throw eprosima::utils::ConfigurationException(
                  STR_ENTRY << "Value <" << value << "> is not valid in enumeration <" << TYPE_NAME(T) << ".");
    }
    else
    {
        return it->second;
    }
}

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

template <typename T>
void read_collection(
        const Yaml& yml,
        std::vector<T>& collection)
{
    if (!yml.IsSequence())
    {
        throw eprosima::utils::ConfigurationException(
                  STR_ENTRY << "Incorrect format, yaml Sequence expected.");
    }

    try
    {
        for (Yaml yml_element : yml)
        {
            collection.push_back(read<T>(yml_element));
        }
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  STR_ENTRY << "Error reading yaml sequence of type <" << TYPE_NAME(T) << "> :\n " <<
                      e.what());
    }
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

template <typename T>
void read(
        const Yaml& yml,
        std::vector<T>& collection)
{
    read_collection<T>(yml, collection);
}

template <typename T>
void read(
        const Yaml& yml,
        std::set<T>& collection)
{
    std::vector<T> collection_vector;
    read_collection<T>(yml, collection_vector);
    collection = std::set<T>(collection_vector.begin(), collection_vector.end());
}

template <typename K, typename T>
void read(
        const Yaml& yml,
        std::map<K, T>& collection)
{
    read_map<K, T>(yml, collection);
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
        return read(get_value_in_tag(yml, tag), object);
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  STR_ENTRY <<
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
