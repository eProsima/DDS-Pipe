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

#include <cpp_utils/ReturnCode.hpp>

#include <ddspipe_participants/xml/XmlHandlerConfiguration.hpp>
#include <ddspipe_participants/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * This class is used for loading XML profiles and in order to configure XML Participants
 */
class XmlHandler
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    static utils::ReturnCode load_xml(
            const XmlHandlerConfiguration& configuration);

};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
