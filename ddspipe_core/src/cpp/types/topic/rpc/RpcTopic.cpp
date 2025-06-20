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
// limitations under the License.

/**
 * @file RpcTopic.cpp
 *
 */

#include <regex>

#include <cpp_utils/utils.hpp>

#include <ddspipe_core/types/topic/rpc/RpcTopic.hpp>
#include <ddspipe_core/types/data/RpcPayloadData.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

const std::string RpcTopic::ROS_TOPIC_REQUEST_PREFIX_STR = "rq/";
const std::string RpcTopic::ROS_TOPIC_REPLY_PREFIX_STR = "rr/";
const std::string RpcTopic::ROS_TOPIC_REQUEST_SUFFIX_STR = "Request";
const std::string RpcTopic::ROS_TOPIC_REPLY_SUFFIX_STR = "Reply";
const std::string RpcTopic::ROS_TYPE_REQUEST_SUFFIX_STR = "Request_";
const std::string RpcTopic::ROS_TYPE_REPLY_SUFFIX_STR = "Response_";

const std::string RpcTopic::FASTDDS_TOPIC_REQUEST_PREFIX_STR = "";
const std::string RpcTopic::FASTDDS_TOPIC_REPLY_PREFIX_STR = "";
const std::string RpcTopic::FASTDDS_TOPIC_REQUEST_SUFFIX_STR = "_Request";
const std::string RpcTopic::FASTDDS_TOPIC_REPLY_SUFFIX_STR = "_Reply";
const std::string RpcTopic::FASTDDS_TYPE_REQUEST_SUFFIX_STR = "_Request";
const std::string RpcTopic::FASTDDS_TYPE_REPLY_SUFFIX_STR = "_Reply";

RpcTopic::RpcTopic(
        const std::string& service_name,
        const DdsTopic& request_topic,
        const DdsTopic& reply_topic) noexcept
    : service_name_(service_name)
    , request_topic_(request_topic)
    , reply_topic_(reply_topic)
{
}

RpcTopic::RpcTopic(
        const DdsTopic& topic) noexcept
{
    if (is_ros2_request_topic(topic))
    {
        request_topic_ = topic;
        reply_topic_ = topic;
        reply_topic_.m_topic_name =
                std::regex_replace(reply_topic_.m_topic_name, std::regex(
                            ROS_TOPIC_REQUEST_PREFIX_STR), ROS_TOPIC_REPLY_PREFIX_STR);
        reply_topic_.m_topic_name =
                std::regex_replace(reply_topic_.m_topic_name, std::regex(
                            ROS_TOPIC_REQUEST_SUFFIX_STR), ROS_TOPIC_REPLY_SUFFIX_STR);
        reply_topic_.type_name =
                std::regex_replace(reply_topic_.type_name, std::regex(
                            ROS_TYPE_REQUEST_SUFFIX_STR), ROS_TYPE_REPLY_SUFFIX_STR);

        service_name_ =
                std::regex_replace(topic.m_topic_name, std::regex(
                            ROS_TOPIC_REQUEST_PREFIX_STR + "|" + ROS_TOPIC_REQUEST_SUFFIX_STR), "");
    }
    else if (is_ros2_reply_topic(topic))
    {
        reply_topic_ = topic;
        request_topic_ = topic;
        request_topic_.m_topic_name =
                std::regex_replace(request_topic_.m_topic_name, std::regex(
                            ROS_TOPIC_REPLY_PREFIX_STR), ROS_TOPIC_REQUEST_PREFIX_STR);
        request_topic_.m_topic_name =
                std::regex_replace(request_topic_.m_topic_name, std::regex(
                            ROS_TOPIC_REPLY_SUFFIX_STR), ROS_TOPIC_REQUEST_SUFFIX_STR);
        request_topic_.type_name =
                std::regex_replace(request_topic_.type_name, std::regex(
                            ROS_TYPE_REPLY_SUFFIX_STR), ROS_TYPE_REQUEST_SUFFIX_STR);

        service_name_ =
                std::regex_replace(topic.m_topic_name, std::regex(
                            ROS_TOPIC_REPLY_PREFIX_STR + "|" + ROS_TOPIC_REPLY_SUFFIX_STR), "");
    }
    else if (is_fastdds_request_topic(topic))
    {
        request_topic_ = topic;
        reply_topic_ = topic;
        reply_topic_.m_topic_name =
                std::regex_replace(reply_topic_.m_topic_name, std::regex(
                            FASTDDS_TOPIC_REQUEST_SUFFIX_STR), FASTDDS_TOPIC_REPLY_SUFFIX_STR);
        reply_topic_.type_name =
                std::regex_replace(reply_topic_.type_name, std::regex(
                            FASTDDS_TYPE_REQUEST_SUFFIX_STR), FASTDDS_TYPE_REPLY_SUFFIX_STR);

        service_name_ = std::regex_replace(topic.m_topic_name, std::regex(FASTDDS_TOPIC_REQUEST_SUFFIX_STR), "");
    }
    else if (is_fastdds_reply_topic(topic))
    {
        reply_topic_ = topic;
        request_topic_ = topic;
        request_topic_.m_topic_name =
                std::regex_replace(request_topic_.m_topic_name, std::regex(
                            FASTDDS_TOPIC_REPLY_SUFFIX_STR), FASTDDS_TOPIC_REQUEST_SUFFIX_STR);
        request_topic_.type_name =
                std::regex_replace(request_topic_.type_name, std::regex(
                            FASTDDS_TYPE_REPLY_SUFFIX_STR), FASTDDS_TYPE_REQUEST_SUFFIX_STR);

        service_name_ = std::regex_replace(topic.m_topic_name, std::regex(FASTDDS_TOPIC_REPLY_SUFFIX_STR), "");
    }
    else
    {
        utils::tsnh(
            utils::Formatter() << "Attempting to create RpcTopic from invalid topic.");
    }

    reply_topic_.m_internal_type_discriminator = INTERNAL_TOPIC_TYPE_RPC;
    request_topic_.m_internal_type_discriminator = INTERNAL_TOPIC_TYPE_RPC;

    // Set both topic qos as the one found
    request_topic_.topic_qos = topic.topic_qos;
    reply_topic_.topic_qos = topic.topic_qos;

    // WORKAROUND: Remove type information from RPC topics. Currently the creation of an RPC topic is triggered when
    // an entity corresponding to the request or reply topics is discovered. This way, the topic and type names of the
    // other topic conforming the pair is deduced. However it is not possible to deduce the type information, so we
    // leave this field empty until the creation mechanism is adapted to cover this scenario.
    request_topic_.type_identifiers = fastdds::dds::xtypes::TypeIdentifierPair();
    reply_topic_.type_identifiers = fastdds::dds::xtypes::TypeIdentifierPair();
}

RpcTopic::RpcTopic(
        const RpcTopic& other) noexcept
    : service_name_(other.service_name_)
    , request_topic_(other.request_topic_)
    , reply_topic_(other.reply_topic_)
{

}

const std::string& RpcTopic::service_name() const
{
    return service_name_;
}

const DdsTopic& RpcTopic::request_topic() const
{
    return request_topic_;
}

const DdsTopic& RpcTopic::reply_topic() const
{
    return reply_topic_;
}

bool RpcTopic::is_ros2_request_topic(
        const DdsTopic& topic)
{
    const std::string& m_topic_name = topic.m_topic_name;
    const std::string& type_name = topic.type_name;

    bool topic_prefix_ok = m_topic_name.find(ROS_TOPIC_REQUEST_PREFIX_STR) == 0;
    bool topic_suffix_ok = (m_topic_name.size() >= ROS_TOPIC_REQUEST_SUFFIX_STR.size() &&
            m_topic_name.compare(m_topic_name.size() - ROS_TOPIC_REQUEST_SUFFIX_STR.size(),
            ROS_TOPIC_REQUEST_SUFFIX_STR.size(),
            ROS_TOPIC_REQUEST_SUFFIX_STR) == 0);

    bool type_suffix_ok = (type_name.size() >= ROS_TYPE_REQUEST_SUFFIX_STR.size() &&
            type_name.compare(type_name.size() - ROS_TYPE_REQUEST_SUFFIX_STR.size(),
            ROS_TYPE_REQUEST_SUFFIX_STR.size(),
            ROS_TYPE_REQUEST_SUFFIX_STR) == 0);

    return topic_prefix_ok && topic_suffix_ok && type_suffix_ok;
}

bool RpcTopic::is_ros2_reply_topic(
        const DdsTopic& topic)
{
    const std::string& m_topic_name = topic.m_topic_name;
    const std::string& type_name = topic.type_name;

    bool topic_prefix_ok = m_topic_name.find(ROS_TOPIC_REPLY_PREFIX_STR) == 0;
    bool topic_suffix_ok = (m_topic_name.size() >= ROS_TOPIC_REPLY_SUFFIX_STR.size() &&
            m_topic_name.compare(m_topic_name.size() - ROS_TOPIC_REPLY_SUFFIX_STR.size(),
            ROS_TOPIC_REPLY_SUFFIX_STR.size(),
            ROS_TOPIC_REPLY_SUFFIX_STR) == 0);

    bool type_suffix_ok = (type_name.size() >= ROS_TYPE_REPLY_SUFFIX_STR.size() &&
            type_name.compare(type_name.size() - ROS_TYPE_REPLY_SUFFIX_STR.size(),
            ROS_TYPE_REPLY_SUFFIX_STR.size(),
            ROS_TYPE_REPLY_SUFFIX_STR) == 0);

    return topic_prefix_ok && topic_suffix_ok && type_suffix_ok;
}

bool RpcTopic::is_fastdds_request_topic(
        const DdsTopic& topic)
{
    const std::string& m_topic_name = topic.m_topic_name;
    const std::string& type_name = topic.type_name;

    bool topic_prefix_ok = m_topic_name.find(FASTDDS_TOPIC_REQUEST_PREFIX_STR) == 0;
    bool topic_suffix_ok = (m_topic_name.size() >= FASTDDS_TOPIC_REQUEST_SUFFIX_STR.size() &&
            m_topic_name.compare(m_topic_name.size() - FASTDDS_TOPIC_REQUEST_SUFFIX_STR.size(),
            FASTDDS_TOPIC_REQUEST_SUFFIX_STR.size(),
            FASTDDS_TOPIC_REQUEST_SUFFIX_STR) == 0);

    bool type_suffix_ok = (type_name.size() >= FASTDDS_TYPE_REQUEST_SUFFIX_STR.size() &&
            type_name.compare(type_name.size() - FASTDDS_TYPE_REQUEST_SUFFIX_STR.size(),
            FASTDDS_TYPE_REQUEST_SUFFIX_STR.size(),
            FASTDDS_TYPE_REQUEST_SUFFIX_STR) == 0);

    return topic_prefix_ok && topic_suffix_ok && type_suffix_ok;
}

bool RpcTopic::is_fastdds_reply_topic(
        const DdsTopic& topic)
{
    const std::string& m_topic_name = topic.m_topic_name;
    const std::string& type_name = topic.type_name;

    bool topic_prefix_ok = m_topic_name.find(FASTDDS_TOPIC_REPLY_PREFIX_STR) == 0;
    bool topic_suffix_ok = (m_topic_name.size() >= FASTDDS_TOPIC_REPLY_SUFFIX_STR.size() &&
            m_topic_name.compare(m_topic_name.size() - FASTDDS_TOPIC_REPLY_SUFFIX_STR.size(),
            FASTDDS_TOPIC_REPLY_SUFFIX_STR.size(),
            FASTDDS_TOPIC_REPLY_SUFFIX_STR) == 0);

    bool type_suffix_ok = (type_name.size() >= FASTDDS_TYPE_REPLY_SUFFIX_STR.size() &&
            type_name.compare(type_name.size() - FASTDDS_TYPE_REPLY_SUFFIX_STR.size(),
            FASTDDS_TYPE_REPLY_SUFFIX_STR.size(),
            FASTDDS_TYPE_REPLY_SUFFIX_STR) == 0);

    return topic_prefix_ok && topic_suffix_ok && type_suffix_ok;
}

bool RpcTopic::is_request_topic(
        const DdsTopic& topic)
{
    return is_ros2_request_topic(topic) || is_fastdds_request_topic(topic);
}

bool RpcTopic::is_reply_topic(
        const DdsTopic& topic)
{
    return is_ros2_reply_topic(topic) || is_fastdds_reply_topic(topic);
}

bool RpcTopic::is_service_topic(
        const DdsTopic& topic)
{
    return is_request_topic(topic) || is_reply_topic(topic);
}

bool RpcTopic::operator <(
        const RpcTopic& other) const
{
    int name_comparison = service_name_.compare(other.service_name());
    if (name_comparison < 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::ostream& operator <<(
        std::ostream& os,
        const RpcTopic& a)
{
    os << "RpcTopic{" << a.service_name() << "}";
    return os;
}

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
