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

#include <ddspipe_core/configuration/IConfiguration.hpp>

#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/types/dds/DomainId.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * Configuration structure encapsulating the configuration required to publish data.
 */
struct DdsPublishingConfiguration : public IConfiguration
{

    /////////////////////////
    // CONSTRUCTORS
    /////////////////////////

    DDSPIPE_CORE_DllAPI
    DdsPublishingConfiguration() = default;

    /////////////////////////
    // METHODS
    /////////////////////////

    DDSPIPE_CORE_DllAPI
    virtual bool is_valid(
            utils::Formatter& error_msg) const noexcept override;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    //! Whether the publishing is enabled or not.
    bool enable = false;

    //! The DDS domain id to publish in.
    types::DomainIdType domain{0};

    //! The DDS topic name to publish in.
    std::string topic_name{};

    //! Whether the LogEntry type should be published or not.
    bool publish_type = false;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
