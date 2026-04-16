// Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <cstdint>

#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_core/types/dds/Guid.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

class IMatchableEndpoint
{
public:

    DDSPIPE_CORE_DllAPI
    virtual ~IMatchableEndpoint() = default;

    DDSPIPE_CORE_DllAPI
    virtual bool wait_until_matched(
            uint32_t number_of_endpoints,
            utils::Duration_ms timeout_ms) const noexcept = 0;

    DDSPIPE_CORE_DllAPI
    virtual bool wait_until_matched(
            const types::Guid& endpoint_guid,
            utils::Duration_ms timeout_ms) const noexcept = 0;

    DDSPIPE_CORE_DllAPI
    virtual bool wait_until_unmatched(
            uint32_t number_of_endpoints,
            utils::Duration_ms timeout_ms) const noexcept = 0;

    DDSPIPE_CORE_DllAPI
    virtual bool wait_until_unmatched(
            const types::Guid& endpoint_guid,
            utils::Duration_ms timeout_ms) const noexcept = 0;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
