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

#pragma once

#include <cpp_utils/logging/LogConfiguration.hpp>
#include <cpp_utils/time/time_utils.hpp>
#include <cpp_utils/types/Fuzzy.hpp>
#include <cpp_utils/utils.hpp>

#include <ddspipe_core/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * Configuration structure encapsulating the configuration of a \c DdsPipe instance.
 */
struct CommandlineArgs
{
    /////////////////////////
    // CONSTRUCTORS
    /////////////////////////

    DDSPIPE_CORE_DllAPI CommandlineArgs();

    /////////////////////////
    // METHODS
    /////////////////////////

    /**
     * @brief \c is_valid method.
     */
    DDSPIPE_CORE_DllAPI
    bool is_valid(
            utils::Formatter& error_msg) const noexcept;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    // Configuration File path
    std::string file_path;

    // Time interval for automatic reloading of the configuration file
    utils::Duration_ms reload_time;

    // Verbosity level for logging
    utils::Fuzzy<utils::VerbosityKind> log_verbosity;

    // Filter for log messages
    utils::LogFilter log_filter;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
