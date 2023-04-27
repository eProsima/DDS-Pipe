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

#include <cpp_utils/exception/ConfigurationException.hpp>
#include <cpp_utils/exception/InconsistencyException.hpp>

#include <ddspipe_yaml/reader/YamlReader.hpp>
#include <ddspipe_yaml/writer/YamlWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

////////////////////////////////////////////////
// DEFINITIONS
////////////////////////////////////////////////
// These definitions are hidden from user and thus they are in .ipp

class IYamlObjectField
{
public:
    virtual ~IYamlObjectField() = default;
    virtual void read_field(const Yaml& yml) = 0;
    virtual void write_field(Yaml& yml) const = 0;
    virtual std::string tag() const noexcept = 0;
};

template <typename T>
class YamlObjectField : public IYamlObjectField
{
public:

    YamlObjectField(
            const std::string& tag,
            const OptionalKind& kind,
            T& vessel);

    YamlObjectField(
            const std::string& tag,
            const OptionalKind& kind,
            const T& vessel);

    virtual void read_field(
            const Yaml& yml) override;

    virtual void write_field(
            Yaml& yml) const override;

    virtual std::string tag() const noexcept override;

protected:

    std::string tag_;
    OptionalKind kind_;
    T& vessel_;
};

DDSPIPE_YAML_DllAPI
void read_fields(
        const Yaml& yml,
        const std::vector<utils::Heritable<IYamlObjectField>>& fields,
        bool fail_with_extra_tags = false,
        bool show_warning_with_extra_tags = true);

DDSPIPE_YAML_DllAPI
void write_fields(
        Yaml& yml,
        const std::vector<utils::Heritable<IYamlObjectField>>& fields);

////////////////////////////////////////////////
// IMPLEMENTATIONS
////////////////////////////////////////////////

template <typename T>
YamlObjectField<T>::YamlObjectField(
        const std::string& tag,
        const OptionalKind& kind,
        const T& vessel)
    : tag_(tag)
    , kind_(kind)
    , vessel_(const_cast<T&>(vessel))
{
    // Do nothing

    // WARNING: This ctor does something that SHOULD NOT BE DONE
    // It cast a const reference to non const. This is done because we assume this will be used wisely
    // and non const variables will be tried to be written.
    // However it suppose a risk that may change in the future if we find a way to solve the following problem:
    // There is NO WAY to write function object_fields commonly for const and non const variables.
    // So instead of forcing developer to write it twice for each type, it is just written once and here cast is made.
}

template <typename T>
void YamlObjectField<T>::read_field(
        const Yaml& yml)
{
    bool present = is_tag_present(yml, this->tag_);
    if (present)
    {
        read<T>(yml, this->tag_, vessel_);
    }
    else if (this->kind_ == OptionalKind::required)
    {
        throw eprosima::utils::ConfigurationException(STR_ENTRY
                            << "Required tag <" << this->tag_
                            << "> not present.");
    }
    else if (this->kind_ == OptionalKind::advisable)
    {
        logWarning(DDSPIPE_YAML, "Tag <" << this->tag_ << "> is advisable but not present in YAML.");
    }
}

template <typename T>
void YamlObjectField<T>::write_field(
        Yaml& yml) const
{
    write<T>(yml, tag_, vessel_);
}

template <typename T>
std::string YamlObjectField<T>::tag() const noexcept
{
    return tag_;
}

template <typename T>
utils::Heritable<IYamlObjectField> create_object_field(
        const std::string& tag,
        const OptionalKind& kind,
        const T& vessel)
{
    return utils::Heritable<YamlObjectField<T>>::make_heritable(
        YamlObjectField<T>(tag, kind, vessel));
}

template <typename T>
void read_from_fields(
        const Yaml& yml,
        T& object)
{
    read_fields(
        yml,
        object_fields(object)
    );
}

template <typename T>
void write_from_fields(
        Yaml& yml,
        const T& object)
{
    write_fields(
        yml,
        object_fields(object)
    );
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
