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

/**
 * @file LogConfiguration.cpp
 *
 */

#include <ddspipe_core/types/dds/LogConfiguration.hpp>
#include <cpp_utils/utils.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

LogConfiguration::LogConfiguration()
{
}

bool LogConfiguration::operator ==(
        const LogConfiguration& other) const noexcept
{
    return
        this->verbosity == other.verbosity;
        this->filter == other.filter;
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

std::ostream& operator<<(std::ostream& os, const LogFilter& filter)
{
    for (const auto& entry : filter)
    {
        os << "Kind: ";
        switch (entry.first)
        {
            case eprosima::fastdds::dds::Log::Kind::Info:
                os << "INFO";
                break;

            case eprosima::fastdds::dds::Log::Kind::Warning:
                os << "WARNING";
                break;

            case eprosima::fastdds::dds::Log::Kind::Error:
                os << "ERROR";
                break;

            default:
                os << "Unknown";
                break;
        }

        os << ", String: " << entry.second << std::endl;
    }

    return os;
}

std::ostream& operator <<(std::ostream& os, const LogConfiguration& configuration)
{
    os <<
        "LogConfiguration{" <<
        "verbosity(" << configuration.verbosity << ")" <<
        ";filter(";

    for (const auto& entry : configuration.filter)
    {
        os << "{" << entry.first << ", " << entry.second << "} ";
    }

    os << ")}";

    return os;
}

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */

