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
// limitations under the License\.

#pragma once

#include <cpp_utils/types/Fuzzy.hpp>
#include <cpp_utils/logging/CustomStdLogConsumer.hpp>

#include <fastdds/dds/core/policy/QosPolicies.hpp>
#include <fastdds/rtps/common/Types.h>

#include <ddspipe_core/configuration/IConfiguration.hpp>
#include <ddspipe_core/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace core {

//! Verbosity enumeration
using VerbosityKind = fastdds::dds::Log::Kind;

//! Log Filter map to assign a filter to every kind of Log
using LogFilter = std::map<VerbosityKind, std::string>;

/**
 * The collection of settings related to Logging.
 *
 * The Logging settings are:
 *  - Verbosity
 *  - Filter
 *
 *
 */
struct LogConfiguration : public IConfiguration
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
    utils::Fuzzy<VerbosityKind> verbosity;
    //! Log Filter
    utils::Fuzzy<LogFilter> filter;

    /////////////////////////
    // METHODS
    /////////////////////////

    /**
     * @brief Override \c is_valid method.
     */
    DDSPIPE_CORE_DllAPI
    virtual bool is_valid(
            utils::Formatter& error_msg) const noexcept override;

};


/**
 * @brief \c VerbosityKind to stream serialization
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const VerbosityKind& kind);

/**
 * @brief \c VerbosityKind to stream Fuzzy level
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const utils::Fuzzy<VerbosityKind>& kind);

/**
 * @brief \c LogFilter to stream serialization
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const LogFilter& filter);

/**
 * @brief \c LogFilter to stream Fuzzy level
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const utils::Fuzzy<LogFilter>& filter);

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
