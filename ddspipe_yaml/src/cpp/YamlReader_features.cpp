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
#include <ddspipe_participants/xml/XmlHandler.hpp>
#include <ddspipe_participants/xml/XmlHandlerConfiguration.hpp>

#include <ddspipe_yaml/Yaml.hpp>
#include <ddspipe_yaml/YamlReader.hpp>
#include <ddspipe_yaml/yaml_configuration_tags.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

/************************
* XML                   *
************************/

template <>
DDSPIPE_YAML_DllAPI
void YamlReader::fill(
        XmlHandlerConfiguration& object,
        const Yaml& yml,
        const YamlReaderVersion version)
{
    // Optional raw
    if (is_tag_present(yml, XML_RAw_TAG))
    {
        object.raw = get<std::string>(yml, XML_RAw_TAG, version);
    }

    // Optional files
    if (is_tag_present(yml, XML_FILES_TAG))
    {
        object.files = get_set<std::string>(yml, XML_FILES_TAG, version);
    }
}

template <>
DDSPIPE_YAML_DllAPI
XmlHandlerConfiguration YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    XmlHandlerConfiguration object;
    fill<XmlHandlerConfiguration>(object, yml, version);
    return object;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
