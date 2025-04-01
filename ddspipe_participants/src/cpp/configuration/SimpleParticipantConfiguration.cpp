// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <cpp_utils/Log.hpp>

#include <ddspipe_participants/configuration/SimpleParticipantConfiguration.hpp>
#include <ddspipe_participants/types/address/Address.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

bool SimpleParticipantConfiguration::is_valid(
        utils::Formatter& error_msg) const noexcept
{
    if (!ParticipantConfiguration::is_valid(error_msg))
    {
        return false;
    }

    if (!domain.is_valid(error_msg))
    {
        error_msg << "Incorrect domain " << domain << ". ";
        return false;
    }

    if (!easy_mode_ip.empty())
    {
        // This option is incompatible with transport tag configurations,
        // so check that transport is set to the default value (builtin)
        if (transport != core::types::TransportDescriptors::builtin)
        {
            error_msg <<
                "Easy mode configuration is incompatible with transport tag configurations.";

            return false;
        }
        // Check if the IP is a valid IPv4 address
        if (!fastdds::rtps::IPLocator::isIPv4(easy_mode_ip))
        {
            error_msg << "Invalid Easy Mode IP value. It must be a valid IPv4 address.";
            return false;
        }
    }

    return true;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
