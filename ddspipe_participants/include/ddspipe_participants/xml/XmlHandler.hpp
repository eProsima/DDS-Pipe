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

#include <cpp_utils/ReturnCode.hpp>

#include <ddspipe_participants/xml/XmlHandlerConfiguration.hpp>
#include <ddspipe_participants/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * QoS policies explicitly defined (i.e. with their tag present) in a DataWriter / DataReader XML profile
 */
struct XmlProfileDefinedQoS
{
    bool durability {false};
    bool reliability {false};
    bool ownership {false};
    bool history {false};
};

/**
 * This class is used for loading XML profiles and in order to configure XML Participants
 */
class XmlHandler
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    static utils::ReturnCode load_xml(
            const XmlHandlerConfiguration& configuration);

    /**
     * @brief QoS policies explicitly defined in the DataWriter XML profile \c profile_name .
     *
     * Fast DDS fills every policy not present in a profile with its default value, so this is the only way to
     * know which policies were actually set in the XML.
     *
     * @note If the profile has not been loaded through \c load_xml , every policy is considered as defined.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    static XmlProfileDefinedQoS get_datawriter_profile_defined_qos(
            const std::string& profile_name);

    /**
     * @brief QoS policies explicitly defined in the DataReader XML profile \c profile_name .
     *
     * Fast DDS fills every policy not present in a profile with its default value, so this is the only way to
     * know which policies were actually set in the XML.
     *
     * @note If the profile has not been loaded through \c load_xml , every policy is considered as defined.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    static XmlProfileDefinedQoS get_datareader_profile_defined_qos(
            const std::string& profile_name);

};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
