// Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

/**
 * Class that represents a RPC service in a DDS network.
 * It is composed of a pair of DDS topics; one for request and other for reply.
 * These two DDS topics are formed by mangling a \c service_name with different prefix and suffixes.
 */
class RpcTopic
{
public:

    //! Constructor by service name, request topic and reply topic
    DDSPIPE_CORE_DllAPI
    RpcTopic(
            const std::string& service_name,
            const DdsTopic& topic_request,
            const DdsTopic& topic_reply) noexcept;

    //! Constructor by request/reply topic only (infers service name and the other request/reply topic)
    DDSPIPE_CORE_DllAPI
    RpcTopic(
            const DdsTopic& topic) noexcept;

    DDSPIPE_CORE_DllAPI
    RpcTopic(
            const RpcTopic& other) noexcept;

    //! Service name getter
    DDSPIPE_CORE_DllAPI
    const std::string& service_name() const;

    //! Request topic getter
    DDSPIPE_CORE_DllAPI
    const DdsTopic& request_topic() const;

    //! Reply topic getter
    DDSPIPE_CORE_DllAPI
    const DdsTopic& reply_topic() const;

    //! Whether a topic is a request topic
    DDSPIPE_CORE_DllAPI
    static bool is_request_topic(
            const DdsTopic& topic);

    //! Whether a topic is a reply topic
    DDSPIPE_CORE_DllAPI
    static bool is_reply_topic(
            const DdsTopic& topic);

    //! Whether a topic is a service topic (request or reply topic)
    DDSPIPE_CORE_DllAPI
    static bool is_service_topic(
            const DdsTopic& topic);

    //! Whether a topic is a ROS2 request topic
    DDSPIPE_CORE_DllAPI
    static bool is_ros2_request_topic(
            const DdsTopic& topic);

    //! Whether a topic is a ROS2 reply topic
    DDSPIPE_CORE_DllAPI
    static bool is_ros2_reply_topic(
            const DdsTopic& topic);

    //! Whether a topic is a FastDDS request topic
    DDSPIPE_CORE_DllAPI
    static bool is_fastdds_request_topic(
            const DdsTopic& topic);

    //! Whether a topic is a FastDDS reply topic
    DDSPIPE_CORE_DllAPI
    static bool is_fastdds_reply_topic(
            const DdsTopic& topic);

    /**
     * Minor operator
     *
     * It compares both service names lexicographically
     */
    bool operator <(
            const RpcTopic& other) const;

protected:

    static bool has_prefix(
            const std::string& str,
            const std::string& prefix)
    {
        return str.find(prefix) == 0;
    }

    static bool has_suffix(
            const std::string& str,
            const std::string& suffix)
    {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    //! Name of the service
    std::string service_name_;

    //! Topic used for transmitting requests
    DdsTopic request_topic_;

    //! Topic used for transmitting replies
    DdsTopic reply_topic_;

    //! Prefix used for request topic
    std::string request_prefix_;
    //! Suffix used for request topic
    std::string request_suffix_;
    //! Prefix used for reply topic
    std::string reply_prefix_;
    //! Suffix used for reply topic
    std::string reply_suffix_;
    //! Suffix used for request type
    std::string request_type_suffix_;
    //! Suffix used for reply type
    std::string reply_type_suffix_;

    static const std::string ROS_TOPIC_REQUEST_PREFIX_STR;
    static const std::string ROS_TOPIC_REPLY_PREFIX_STR;
    static const std::string ROS_TOPIC_REQUEST_SUFFIX_STR;
    static const std::string ROS_TOPIC_REPLY_SUFFIX_STR;
    static const std::string ROS_TYPE_REQUEST_SUFFIX_STR;
    static const std::string ROS_TYPE_REPLY_SUFFIX_STR;

    static const std::string FASTDDS_TOPIC_REQUEST_PREFIX_STR;
    static const std::string FASTDDS_TOPIC_REPLY_PREFIX_STR;
    static const std::string FASTDDS_TOPIC_REQUEST_SUFFIX_STR;
    static const std::string FASTDDS_TOPIC_REPLY_SUFFIX_STR;
    static const std::string FASTDDS_TYPE_REQUEST_SUFFIX_STR;
    static const std::string FASTDDS_TYPE_REPLY_SUFFIX_STR;
};

/**
 * Serialization method
 *
 * It prints the service name inside "{}"
 * Example: RpcTopic{ServiceName}
 */
std::ostream& operator <<(
        std::ostream& os,
        const RpcTopic& a);

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
