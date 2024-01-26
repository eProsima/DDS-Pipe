// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

/**
 * @file LogConfiguration.cpp
 *
 */

#include <ddspipe_core/configuration/LogConfiguration.hpp>
#include <cpp_utils/utils.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {


LogConfiguration::LogConfiguration()
    : verbosity{VerbosityKind::Warning, utils::FuzzyLevelValues::fuzzy_level_default}
{
    LogFilter filter_template;
    filter_template[VerbosityKind::Info] = "";
    filter_template[VerbosityKind::Warning] = "";
    filter_template[VerbosityKind::Error] = "";
    filter.set_value(filter_template, utils::FuzzyLevelValues::fuzzy_level_default);
    // verbosity.set_value(VerbosityKind::Warning, utils::FuzzyLevelValues::fuzzy_level_default);
}

bool LogConfiguration::operator ==(
        const LogConfiguration& other) const noexcept
{
    return
        (this->verbosity == other.verbosity) &&
        (this->filter == other.filter);
}

bool LogConfiguration::is_valid(utils::Formatter& error_msg) const noexcept
{
    // Verifica si verbosity es uno de los valores permitidos
    if (verbosity.get_value() != VerbosityKind::Error &&
        verbosity.get_value() != VerbosityKind::Warning &&
        verbosity.get_value() != VerbosityKind::Info)
    {
        error_msg << "Invalid verbosity level. Valid values are Error, Warning, or Info.";
        return false;
    }

    return true;
}

std::ostream& operator <<(std::ostream& os, const VerbosityKind& kind)
{
    switch (kind)
    {
        case VerbosityKind::Info:
            os << "INFO";
            break;

        case VerbosityKind::Warning:
            os << "WARNING";
            break;

        case VerbosityKind::Error:
            os << "ERROR";
            break;

        default:
            utils::tsnh(utils::Formatter() << "Invalid Verbosity Kind.");
            break;
    }

    return os;
}

std::ostream& operator <<(
        std::ostream& os,
        const utils::Fuzzy<VerbosityKind>& kind)
{
    os << "Fuzzy{Level(" << kind.get_level_as_str() << ") " << kind.get_reference() << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const LogFilter& filter)
{
    for (const auto& entry : filter)
    {
        os << "Kind: " << entry.first <<
        ", Regex: " << entry.second << std::endl;
    }

    return os;
}

std::ostream& operator <<(
        std::ostream& os,
        const utils::Fuzzy<LogFilter>& filter)
{
    os << "Fuzzy{Level(" << filter.get_level_as_str() << ") " << filter.get_reference() << "}";
    return os;
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */

