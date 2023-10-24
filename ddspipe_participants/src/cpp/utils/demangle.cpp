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

#include <algorithm>
#include <string>
#include <vector>

#include <cpp_utils/Log.hpp>
#include <cpp_utils/utils.hpp>

#include <utils/demangle.hpp>

namespace eprosima {
namespace ddspipe {
namespace utils {

const char* const ros_topic_prefix = "rt";
const char* const ros_service_requester_prefix = "rq";
const char* const ros_service_response_prefix = "rr";

const std::vector<std::string> ros_prefixes_ =
{ros_topic_prefix, ros_service_requester_prefix, ros_service_response_prefix};

std::string remove_prefix(
        const std::string& name,
        const std::string& prefix)
{
    if (name.rfind(prefix + "/", 0) == 0)
    {
        return name.substr(prefix.length());
    }
    return "";
}

std::string get_ros_prefix_if_exists(
        const std::string& topic_name)
{
    for (const auto& prefix : ros_prefixes_)
    {
        if (topic_name.rfind(prefix + "/", 0) == 0)
        {
            return prefix;
        }
    }
    return "";
}

std::string remove_ros_prefix_if_exists(
        const std::string& topic_name)
{
    for (const auto& prefix : ros_prefixes_)
    {
        if (topic_name.rfind(prefix + "/", 0) == 0)
        {
            return topic_name.substr(prefix.length());
        }
    }
    return topic_name;
}

const std::vector<std::string>& get_all_ros_prefixes()
{
    return ros_prefixes_;
}

std::string demangle_if_ros_topic(
        const std::string& topic_name)
{
    return remove_ros_prefix_if_exists(topic_name);
}

std::string demangle_if_ros_type(
        const std::string& dds_type_string)
{
    if (dds_type_string[dds_type_string.size() - 1] != '_')
    {
        // not a ROS type
        return dds_type_string;
    }

    std::string substring = "dds_::";
    size_t substring_position = dds_type_string.find(substring);
    if (substring_position == std::string::npos)
    {
        // not a ROS type
        return dds_type_string;
    }

    std::string type_namespace = dds_type_string.substr(0, substring_position);

    eprosima::utils::replace_all(type_namespace, "::", "/");

    size_t start = substring_position + substring.size();
    std::string type_name = dds_type_string.substr(start, dds_type_string.length() - 1 - start);

    return type_namespace + type_name;
}

std::string demangle_ros_topic_from_topic(
        const std::string& topic_name)
{
    return remove_prefix(topic_name, ros_topic_prefix);
}

std::string demangle_service_from_topic(
        const std::string& prefix,
        const std::string& topic_name,
        std::string suffix)
{
    std::string service_name = remove_prefix(topic_name, prefix);
    if ("" == service_name)
    {
        return "";
    }

    size_t suffix_position = service_name.rfind(suffix);
    if (suffix_position != std::string::npos)
    {
        if (service_name.length() - suffix_position - suffix.length() != 0)
        {
            logWarning(DDSPIPE_DEMANGLE,
                "service topic has service prefix and a suffix, but not at the end"
                ", report this: " << topic_name.c_str());
            return "";
        }
    }
    else
    {
        logWarning(DDSPIPE_DEMANGLE,
            "service topic has prefix but no suffix"
            ", report this: " << topic_name.c_str());
        return "";
    }
    return service_name.substr(0, suffix_position);
}

std::string demangle_service_from_topic(
        const std::string& topic_name)
{
    const std::string demangled_topic = demangle_service_reply_from_topic(topic_name);
    if ("" != demangled_topic)
    {
        return demangled_topic;
    }
    return demangle_service_request_from_topic(topic_name);
}

std::string demangle_service_request_from_topic(
        const std::string& topic_name)
{
    return demangle_service_from_topic(ros_service_requester_prefix, topic_name, "Request");
}

std::string demangle_service_reply_from_topic(
        const std::string& topic_name)
{
    return demangle_service_from_topic(ros_service_response_prefix, topic_name, "Reply");
}

std::string demangle_service_type_only(
        const std::string& dds_type_name)
{
    std::string ns_substring = "dds_::";
    size_t ns_substring_position = dds_type_name.find(ns_substring);
    if (std::string::npos == ns_substring_position)
    {
        // not a ROS service type
        return "";
    }
    auto suffixes = {
        std::string("_Response_"),
        std::string("_Request_"),
    };
    std::string found_suffix = "";
    size_t suffix_position = 0;
    for (auto suffix : suffixes)
    {
        suffix_position = dds_type_name.rfind(suffix);
        if (suffix_position != std::string::npos)
        {
            if (dds_type_name.length() - suffix_position - suffix.length() != 0)
            {
                logWarning(DDSPIPE_DEMANGLE,
                    "service type contains 'dds_::' and a suffix, but not at the end"
                    ", report this: " << dds_type_name.c_str());
                continue;
            }
            found_suffix = suffix;
            break;
        }
    }
    if (std::string::npos == suffix_position)
    {
        logWarning(DDSPIPE_DEMANGLE,
           "service type contains 'dds_::' but does not have a suffix"
            ", report this: " << dds_type_name.c_str());
        return "";
    }
    // everything checks out, reformat it from '[type_namespace::]dds_::<type><suffix>'
    // to '[type_namespace/]<type>'
    std::string type_namespace = dds_type_name.substr(0, ns_substring_position);
    eprosima::utils::replace_all(type_namespace, "::", "/");
    size_t start = ns_substring_position + ns_substring.length();
    std::string type_name = dds_type_name.substr(start, suffix_position - start);
    return type_namespace + type_name;
}

} /* namespace utils */
} /* namespace ddspipe */
} /* namespace eprosima */