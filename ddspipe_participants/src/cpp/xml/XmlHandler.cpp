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

#include <cstdlib>
#include <map>
#include <mutex>
#include <string>

#include <tinyxml2.h>

#include <fastdds/dds/core/ReturnCode.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <cpp_utils/utils.hpp>

#include <ddspipe_participants/xml/XmlHandler.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

namespace {

// Fast DDS XML tags and environment variables (see fastdds xmlparser/XMLParserCommon.cpp)
constexpr const char* XML_ROOT_TAG = "dds";
constexpr const char* XML_PROFILES_TAG = "profiles";
constexpr const char* XML_PROFILE_NAME_ATTR = "profile_name";
constexpr const char* XML_DATA_WRITER_TAG = "data_writer";
constexpr const char* XML_PUBLISHER_TAG = "publisher";
constexpr const char* XML_DATA_READER_TAG = "data_reader";
constexpr const char* XML_SUBSCRIBER_TAG = "subscriber";
constexpr const char* XML_QOS_TAG = "qos";
constexpr const char* XML_TOPIC_TAG = "topic";
constexpr const char* XML_DURABILITY_TAG = "durability";
constexpr const char* XML_RELIABILITY_TAG = "reliability";
constexpr const char* XML_OWNERSHIP_TAG = "ownership";
constexpr const char* XML_HISTORY_TAG = "historyQos";
constexpr const char* DEFAULT_PROFILES_FILE_ENV = "FASTDDS_DEFAULT_PROFILES_FILE";
constexpr const char* SKIP_DEFAULT_PROFILES_FILE_ENV = "SKIP_DEFAULT_XML_FILE";
constexpr const char* DEFAULT_PROFILES_FILE = "DEFAULT_FASTDDS_PROFILES.xml";

std::mutex profiles_mutex_;
std::map<std::string, XmlProfileDefinedQoS> datawriter_profiles_;
std::map<std::string, XmlProfileDefinedQoS> datareader_profiles_;

std::string get_env_(
        const char* name)
{
    std::string value;
#ifdef _WIN32
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, name) == 0 && buf != nullptr)
    {
        value = buf;
        free(buf);
    }
#else
    if (const char* buf = std::getenv(name))
    {
        value = buf;
    }
#endif // ifdef _WIN32
    return value;
}

XmlProfileDefinedQoS parse_defined_qos_(
        const tinyxml2::XMLElement& profile)
{
    XmlProfileDefinedQoS defined;

    if (const tinyxml2::XMLElement* qos = profile.FirstChildElement(XML_QOS_TAG))
    {
        defined.durability = qos->FirstChildElement(XML_DURABILITY_TAG) != nullptr;
        defined.reliability = qos->FirstChildElement(XML_RELIABILITY_TAG) != nullptr;
        defined.ownership = qos->FirstChildElement(XML_OWNERSHIP_TAG) != nullptr;
    }

    if (const tinyxml2::XMLElement* topic = profile.FirstChildElement(XML_TOPIC_TAG))
    {
        defined.history = topic->FirstChildElement(XML_HISTORY_TAG) != nullptr;
    }

    return defined;
}

void register_profiles_(
        const tinyxml2::XMLElement& parent)
{
    for (const tinyxml2::XMLElement* element = parent.FirstChildElement();
            element != nullptr;
            element = element->NextSiblingElement())
    {
        const std::string tag = element->Name();

        if (tag == XML_PROFILES_TAG)
        {
            register_profiles_(*element);
            continue;
        }

        const char* profile_name = element->Attribute(XML_PROFILE_NAME_ATTR);

        if (profile_name == nullptr)
        {
            continue;
        }

        // Fast DDS keeps the first profile loaded with a given name, so emplace does not overwrite either
        if (tag == XML_DATA_WRITER_TAG || tag == XML_PUBLISHER_TAG)
        {
            datawriter_profiles_.emplace(profile_name, parse_defined_qos_(*element));
        }
        else if (tag == XML_DATA_READER_TAG || tag == XML_SUBSCRIBER_TAG)
        {
            datareader_profiles_.emplace(profile_name, parse_defined_qos_(*element));
        }
    }
}

void register_profiles_(
        const tinyxml2::XMLDocument& document)
{
    const tinyxml2::XMLElement* root = document.FirstChildElement(XML_ROOT_TAG);

    if (root == nullptr)
    {
        root = document.FirstChildElement(XML_PROFILES_TAG);
    }

    if (root != nullptr)
    {
        std::lock_guard<std::mutex> lock(profiles_mutex_);
        register_profiles_(*root);
    }
}

void register_profiles_from_file_(
        const std::string& file)
{
    tinyxml2::XMLDocument document;

    if (document.LoadFile(file.c_str()) == tinyxml2::XML_SUCCESS)
    {
        register_profiles_(document);
    }
}

void register_profiles_from_string_(
        const std::string& xml)
{
    tinyxml2::XMLDocument document;

    if (document.Parse(xml.c_str(), xml.size()) == tinyxml2::XML_SUCCESS)
    {
        register_profiles_(document);
    }
}

void register_default_profiles_()
{
    // Same files (and order) Fast DDS loads in DomainParticipantFactory::load_profiles
    const std::string env_file = get_env_(DEFAULT_PROFILES_FILE_ENV);

    if (!env_file.empty())
    {
        register_profiles_from_file_(env_file);
    }

    if (get_env_(SKIP_DEFAULT_PROFILES_FILE_ENV) != "1")
    {
        register_profiles_from_file_(DEFAULT_PROFILES_FILE);
    }
}

XmlProfileDefinedQoS get_profile_defined_qos_(
        const std::map<std::string, XmlProfileDefinedQoS>& profiles,
        const std::string& profile_name)
{
    std::lock_guard<std::mutex> lock(profiles_mutex_);

    const auto it = profiles.find(profile_name);

    if (it == profiles.end())
    {
        // Unknown profile (not loaded through XmlHandler):
        // consider every policy as defined in the XML
        return XmlProfileDefinedQoS{true, true, true, true};
    }

    return it->second;
}

} /* namespace */

utils::ReturnCode XmlHandler::load_xml(
        const XmlHandlerConfiguration& configuration)
{
    // Load default xml profiles
    fastdds::dds::ReturnCode_t ret = fastdds::dds::DomainParticipantFactory::get_instance()->load_profiles();

    if (ret != fastdds::dds::RETCODE_OK)
    {
        return utils::ReturnCode::RETCODE_ERROR;
    }

    register_default_profiles_();

    // Load string if exist
    if (configuration.raw.is_set())
    {
        ret = fastdds::dds::DomainParticipantFactory::get_instance()->load_XML_profiles_string(
            configuration.raw->c_str(),
            configuration.raw->size());

        if (ret != fastdds::dds::RETCODE_OK)
        {
            return utils::ReturnCode::RETCODE_ERROR;
        }

        register_profiles_from_string_(configuration.raw.get_value());
    }

    // Load files
    for (const auto& file : configuration.files)
    {
        ret = fastdds::dds::DomainParticipantFactory::get_instance()->load_XML_profiles_file(file);

        if (ret != fastdds::dds::RETCODE_OK)
        {
            return utils::ReturnCode::RETCODE_ERROR;
        }

        register_profiles_from_file_(file);
    }

    return utils::ReturnCode::RETCODE_OK;
}

XmlProfileDefinedQoS XmlHandler::get_datawriter_profile_defined_qos(
        const std::string& profile_name)
{
    return get_profile_defined_qos_(datawriter_profiles_, profile_name);
}

XmlProfileDefinedQoS XmlHandler::get_datareader_profile_defined_qos(
        const std::string& profile_name)
{
    return get_profile_defined_qos_(datareader_profiles_, profile_name);
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
