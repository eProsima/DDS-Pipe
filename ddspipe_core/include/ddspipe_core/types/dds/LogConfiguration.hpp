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
// limitations under the License\.

#pragma once

#include <cpp_utils/types/Fuzzy.hpp>
#include <cpp_utils/logging/CustomStdLogConsumer.hpp>

#include <fastdds/dds/core/policy/QosPolicies.hpp>
#include <fastdds/rtps/common/Types.h>

#include <ddspipe_core/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

//! Durability kind enumeration
using VerbosityKind = eprosima::utils::Log::Kind;

//! Log Filter map
using LogFilter = std::map<eprosima::fastdds::dds::Log::Kind, std::string>;

/**
 * The collection of settings related to Logging.
 *
 * The Logging settings are:
 *  - Publish
 *  - Stdout
 *  - Verbosity
 *  - Filter
 *
 *
 */
struct
LogConfiguration
{
    /////////////////////////
    // CONSTRUCTORS
    /////////////////////////

    //! Default LogConfiguration with reader less restrictive parameters
    DDSPIPE_CORE_DllAPI
    LogConfiguration();

    /////////////////////////
    // OPERATORS
    /////////////////////////

    //! Equality operator
    DDSPIPE_CORE_DllAPI
    bool operator ==(
            const LogConfiguration& other) const noexcept;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    //! Verbosity kind
    VerbosityKind verbosity = VerbosityKind::Warning;
    //! Log Filter
    LogFilter filter = {
        {VerbosityKind::Error, ""},
        {VerbosityKind::Warning, ""},
        {VerbosityKind::Info, ""}};

    /////////////////////////
    // GLOBAL VARIABLES
    /////////////////////////

    DDSPIPE_CORE_DllAPI
    static LogConfiguration default_logging_configuration()
    {
        LogConfiguration config;
        config.verbosity = VerbosityKind::Warning;
        config.filter = {
            {VerbosityKind::Error, ""},
            {VerbosityKind::Warning, ""},
            {VerbosityKind::Info, ""}
        };
        return config;
    }

};

/**
 * @brief \c VerbosityKind to stream serialization
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const VerbosityKind& kind);

/**
 * @brief \c LogFilter to stream serialization
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const LogFilter& filter);

/**
 * @brief \c LogConfiguration to stream serialization
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const LogConfiguration& configuration);

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
