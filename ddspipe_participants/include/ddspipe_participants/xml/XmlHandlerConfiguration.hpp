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

#include <set>
#include <string>

#include <cpp_utils/types/Fuzzy.hpp>

#include <ddspipe_core/configuration/IConfiguration.hpp>

#include <ddspipe_participants/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * This data struct represents the configuration for an XML Handler
 */
struct XmlHandlerConfiguration : public core::IConfiguration
{
public:

    /////////////////////////
    // CONSTRUCTORS
    /////////////////////////
    DDSPIPE_PARTICIPANTS_DllAPI
    XmlHandlerConfiguration() = default;

    /////////////////////////
    // METHODS
    /////////////////////////

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool is_valid(
            utils::Formatter& error_msg) const noexcept override;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    utils::Fuzzy<std::string> raw {};
    std::set<std::string> files {};
};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
