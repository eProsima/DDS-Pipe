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

// #include <fastrtps/xmlparser/XMLProfileManager.h>
#include <fastdds/dds/core/ReturnCode.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <ddspipe_participants/xml/XmlHandler.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

utils::ReturnCode XmlHandler::load_xml(
        const XmlHandlerConfiguration& configuration)
{
    // Load default xml profiles
    fastdds::dds::ReturnCode_t ret = fastdds::dds::DomainParticipantFactory::get_instance()->load_profiles();

    if (ret != fastdds::dds::RETCODE_OK)
    {
        return utils::ReturnCode::ERROR;
    }

    // Load string if exist
    if (configuration.raw.is_set())
    {
        fastdds::dds::ReturnCode_t ret = fastdds::dds::DomainParticipantFactory::get_instance()->load_XML_profiles_string(
            configuration.raw->c_str(),
            configuration.raw->size());

        if (ret != fastdds::dds::RETCODE_OK)
        {
            return utils::ReturnCode::ERROR;
        }
    }

    // Load files
    for (const auto& file : configuration.files)
    {
        fastdds::dds::ReturnCode_t ret = fastdds::dds::DomainParticipantFactory::get_instance()->load_XML_profiles_file(file);

        if (ret != fastdds::dds::RETCODE_OK)
        {
            return utils::ReturnCode::ERROR;
        }
    }

    return utils::ReturnCode::OK;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
