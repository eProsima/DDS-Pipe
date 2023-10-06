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

#include <cpp_utils/Formatter.hpp>

#include <ddspipe_core/configuration/IConfiguration.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>

#include <ddspipe_core/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * Configuration structure encapsulating the forwarding routes of a \c DdsPipe instance.
 */
struct RoutesConfiguration : public IConfiguration
{

    using RoutesMap = std::map<types::ParticipantId, std::set<types::ParticipantId>>;

    /////////////////////////
    // CONSTRUCTORS
    /////////////////////////

    DDSPIPE_CORE_DllAPI RoutesConfiguration() = default;

    /////////////////////////
    // METHODS
    /////////////////////////

    DDSPIPE_CORE_DllAPI virtual bool is_valid(
            utils::Formatter& error_msg) const noexcept override;

    DDSPIPE_CORE_DllAPI bool is_valid(
            utils::Formatter& error_msg,
            const std::map<types::ParticipantId, bool>& participants) const noexcept;

    /////////////////////////
    // OPERATORS
    /////////////////////////

    DDSPIPE_CORE_DllAPI RoutesMap operator () () const;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    RoutesMap routes {};
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
