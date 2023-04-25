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
#include <ddspipe_yaml/Yaml.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

/**
 * @brief Whether key \c tag is present in \c yml
 *
 * The key \c tag must be in base level of \c yml , it will no be looked for recursively.
 *
 * It could only look for keys in yaml map and empty yaml. It will fail in array and scalar cases.
 * In this second case, return will always be false.
 *
 * @param yml base yaml
 * @param tag key to look for in \c yml
 * @return true id \c tag is a key in \c yml
 * @return false otherwise
 *
 * @throw \c ConfigurationException if \c yml is not a map or empty yaml
 */
bool is_tag_present(
        const Yaml& yml,
        const TagType& tag);

/**
 * @brief Get the yaml inside key \c tag in \c yml
 *
 * \c tag must be present in \c yml . It uses \c is_tag_present to check the key is present.
 * Use \c is_tag_present before calling this method in order to avoid the exception.
 *
 * @param yml base yaml
 * @param tag key refering the value returned
 * @return yaml value inside key \c tag
 *
 * @throw \c ConfigurationException if \c tag is not present in \c yml
 */
Yaml get_value_in_tag(
        const Yaml& yml,
        const TagType& tag);

/**
 * @brief Fill an element given by parameter with the values inside \c yml
 *
 * This method simplifies the process of retrieving an object whose parent has its own \c fill method,
 * as the code is reused from one another calling parent \c fill in child.
 * It is also very helpful to handle default creation values. Without this, every different default value
 * must have its own if-else clause, forcing to create the respective constructor. With this method,
 * the default values are initialized with the default constructor, and then are overwritten by the yaml.
 * [this problem arises because C++ does not allow different order of parameters in method call]
 */
template <typename T>
void fill(
        T& object,
        const Yaml& yml);

//! Fill an element given by parameter with the values inside \c tag in \c yml
template <typename T>
void fill(
        T& object,
        const Yaml& yml,
        const TagType& tag);

ENUMERATION_BUILDER
(
    TagKind,
    required,
    optional
);

struct IYamlFieldGetter
{
    IYamlFieldGetter(
            TagKind kind_arg,
            std::string tag_arg)
        : kind(kind_arg)
        , tag(tag_arg)
    {
    }

    virtual ~IYamlFieldGetter() = default;
    virtual void get(const Yaml& yml) = 0;

    TagKind kind;
    std::string tag;
};

template <typename T>
struct YamlFieldGetter : IYamlFieldGetter
{
    YamlFieldGetter(
            std::string tag_arg,
            TagKind kind_arg,
            T& vessel_arg)
        : IYamlFieldGetter(kind_arg, tag_arg)
        , vessel(vessel_arg)
    {
    }

    virtual void get(
            const Yaml& yml) override
    {
        fill<T>(vessel, yml, tag);
    }

    T& vessel;
};

template <typename T>
struct YamlFieldGetterFuzzy : IYamlFieldGetter
{
    YamlFieldGetterFuzzy(
            std::string tag_arg,
            TagKind kind_arg,
            utils::Fuzzy<T>& vessel_arg)
        : IYamlFieldGetter(kind_arg, tag_arg)
        , vessel(vessel_arg)
    {
    }

    virtual void get(
            const Yaml& yml) override
    {
        fill<T>(vessel.get_reference(), yml, tag);
        vessel.set_level();
    }

    utils::Fuzzy<T>& vessel;
};

template <typename T>
std::vector<IYamlFieldGetter*> yaml_reader_definition(
        T& object);

DDSPIPE_YAML_DllAPI
inline bool get_tags(
        const Yaml& yml,
        const std::vector<IYamlFieldGetter*>& tags_to_get,
        bool fail_with_extra_tags = true,
        bool fail_with_exception = true)
{
    if (!yml.IsMap() && !yml.IsNull())
    {
        throw eprosima::utils::ConfigurationException(STR_ENTRY
                      << "Trying to get tags: in a not yaml object map.");
    }

    // First, check that every required flag is present
    for (const auto& field : tags_to_get)
    {
        bool present = is_tag_present(yml, field->tag);
        if (present)
        {
            field->get(yml);
        }
        else if (field->kind == TagKind::required)
        {
            if (fail_with_exception)
            {
                throw eprosima::utils::ConfigurationException(STR_ENTRY
                                << "Required tag <" << field->tag << "> not present.");
            }
            else
            {
                return false;
            }
        }
    }

    // Now that every required field is present, check for extra tags
    for (const auto& yaml_field : yml)
    {
        auto tag = yaml_field.first.as<std::string>();
        bool should_be_present = false;

        // For each tag, check if it is inside allowed tags
        for (const auto& field : tags_to_get)
        {
            if (field->tag == tag)
            {
                should_be_present = true;
                break;
            }
        }

        // If present tag is not inside allowed ones, fail
        if (!should_be_present)
        {
            if (fail_with_exception)
            {
                throw eprosima::utils::ConfigurationException(STR_ENTRY
                              << "Unexpected tag <" << tag << "> present.");
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}

template <typename T>
void fill(
        T& object,
        const Yaml& yml,
        const TagType& tag)
{
    // ATTENTION: This try catch can be avoided, it is only used to add verbose information
    try
    {
        fill<T>(object, get_value_in_tag(yml, tag));
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() <<
                      "Error filling object of type <" << TYPE_NAME(T) <<
                      "> in tag <" << tag << "> :\n " << e.what());
    }
}

template <typename T>
void fill(
        T& object,
        const Yaml& yml)
{
    get_tags(
        yml,
        yaml_reader_definition<T>(object)
    );
}

template <typename T>
T get_scalar(
        const Yaml& yml)
{
    if (!yml.IsScalar())
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() <<
                      "Trying to read a primitive value of type <" << TYPE_NAME(T) << "> from a non scalar yaml.");
    }

    try
    {
        return yml.as<T>();
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() <<
                      "Incorrect format for primitive value, expected <" << TYPE_NAME(T) << ">:\n " << e.what());
    }
}

template <>
void fill<std::string>(
        std::string& object,
        const Yaml& yml)
{
    object = get_scalar<std::string>(yml);
}

template <>
std::vector<IYamlFieldGetter*> yaml_reader_definition(
        core::types::DdsTopic& object)
{
    return
    {
        new YamlFieldGetter("name", TagKind::required,  object.m_topic_name),
        new YamlFieldGetter("type", TagKind::required,  object.type_name),
        new YamlFieldGetter("qos",  TagKind::optional,  object.topic_qos)
    };
}

template <>
void fill<core::types::TopicQoS>(
        core::types::TopicQoS& object,
        const Yaml& yml)
{
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */

// Include implementation template file
// #include <ddspipe_yaml/impl/NewYamlReader.ipp>
