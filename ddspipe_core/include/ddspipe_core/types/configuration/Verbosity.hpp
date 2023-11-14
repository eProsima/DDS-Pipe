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
// limitations under the License\.

#pragma once

#include <string>

#include <ddspipe_core/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

//! Data Type of the value that stores the Verbosity level
using VerbosityLevelType = short;

/**
 * @brief Guide values for the meaning of the Fuzzy level
 *
 * This values are not all the possible values of a Verbosity level.
 * A Verbosity level could be any number in range of \c VerbosityLevelType .
 * This values are only a guide to know what each level refers to.
 */
enum VerbosityLevelValues : VerbosityLevelType
{
    QUIET = 0,
    DISCOVERY = 20,
    ACTIVITY = 40,
    NOISY = 60
};

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
