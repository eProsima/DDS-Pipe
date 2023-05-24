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

#include <cpp_utils/Log.hpp>
#include <cpp_utils/ReturnCode.hpp>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <ddspipe_participants/configuration/XmlParticipantConfiguration.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

bool XmlParticipantConfiguration::is_valid(
        utils::Formatter& error_msg) const noexcept
{
    if (!SimpleParticipantConfiguration::is_valid(error_msg))
    {
        return false;
    }

    // Check that XML profile exist (only if set)
    if (participant_profile.is_set())
    {
        fastdds::dds::DomainParticipantQos qos;
        auto res = fastdds::dds::DomainParticipantFactory::get_instance()->get_participant_qos_from_profile(
            participant_profile.get_value(),
            qos);

        if (res != utils::ReturnCode::RETCODE_OK)
        {
            error_msg << "Profile " << participant_profile.get_value() << " is not loaded in XML. ";
            return false;
        }
    }

    return true;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
