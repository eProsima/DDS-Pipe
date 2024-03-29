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

#include <cpp_utils/Formatter.hpp>
#include <cpp_utils/logging/BaseLogConfiguration.hpp>

#include <ddspipe_core/configuration/DdsPublishingConfiguration.hpp>
#include <ddspipe_core/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * The collection of settings related to the DDS Pipe's Log consumers.
 *
 * The Log consumers configured are:
 *  - DdsLogConsumer
 *  - StdLogConsumer
 */
struct DdsPipeLogConfiguration : public utils::BaseLogConfiguration
{

    /////////////////////////
    // CONSTRUCTORS
    /////////////////////////

    DDSPIPE_CORE_DllAPI
    DdsPipeLogConfiguration() = default;

    /////////////////////////
    // METHODS
    /////////////////////////

    DDSPIPE_CORE_DllAPI
    virtual bool is_valid(
            utils::Formatter& error_msg) const noexcept override;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    DdsPublishingConfiguration publish;

    bool stdout_enable = true;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
