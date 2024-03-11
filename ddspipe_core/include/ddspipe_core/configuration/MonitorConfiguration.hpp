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

#include <map>
#include <string>

#include <cpp_utils/Formatter.hpp>

#include <ddspipe_core/configuration/DdsPublishingConfiguration.hpp>
#include <ddspipe_core/configuration/IConfiguration.hpp>
#include <ddspipe_core/configuration/MonitorProducerConfiguration.hpp>
#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/types/dds/DomainId.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * Configuration structure encapsulating the configuration of a \c Monitor instance.
 */
struct MonitorConfiguration : public IConfiguration
{
    /////////////////////////
    // METHODS
    /////////////////////////

    /**
     * @brief Check if the configuration is valid.
     *
     * @param error_msg The error message to be filled if the configuration is not valid.
     */
    DDSPIPE_CORE_DllAPI
    bool is_valid(
            utils::Formatter& error_msg) const noexcept override;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    // Domain to be used by the DdsMonitorConsumers that don't have a domain configured.
    types::DomainIdType domain{0};

    // Configurations of the DdsMonitorConsumers.
    std::map<std::string, DdsPublishingConfiguration> consumers;

    // Configurations of the MonitorProducers.
    std::map<std::string, MonitorProducerConfiguration> producers;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
