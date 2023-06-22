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

#include <cpp_utils/utils.hpp>

#include <fastrtps/xmlparser/XMLProfileManager.h>

#include <ddspipe_participants/xml/XmlHandler.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

utils::ReturnCode XmlHandler::load_xml(
        const XmlHandlerConfiguration& configuration)
{
    // Load default xml profiles
    // Note: we assume this function cannot fail
    fastrtps::xmlparser::XMLProfileManager::loadDefaultXMLFile();

    // Load string if exist
    if (configuration.raw.is_set())
    {
        fastrtps::xmlparser::XMLP_ret res = fastrtps::xmlparser::XMLProfileManager::loadXMLString(
            configuration.raw->c_str(),
            configuration.raw->size());

        if (res != fastrtps::xmlparser::XMLP_ret::XML_OK)
        {
            return utils::ReturnCode::RETCODE_ERROR;
        }
    }

    // Load files
    for (const auto& file : configuration.files)
    {
        fastrtps::xmlparser::XMLP_ret res = fastrtps::xmlparser::XMLProfileManager::loadXMLFile(file);

        if (res != fastrtps::xmlparser::XMLP_ret::XML_OK)
        {
            return utils::ReturnCode::RETCODE_ERROR;
        }
    }

    return utils::ReturnCode::RETCODE_OK;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
