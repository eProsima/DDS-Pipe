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

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>

#include <ddspipe_participants/utils/demangle.hpp>

using namespace eprosima::ddspipe::utils;

/**
 * @todo
 */
TEST(DemangleTest, remove_prefix)
{
    EXPECT_EQ("", remove_prefix("hello", "world"));
    EXPECT_EQ("", remove_prefix("hello/rt", "rt"));
    EXPECT_EQ("", remove_prefix("rt/hello", "rt/"));
    EXPECT_EQ("/hello", remove_prefix("rt/hello", "rt"));
}

/**
 * @todo
 */
TEST(DemangleTest, get_ros_prefix_if_exists)
{
    EXPECT_EQ("", get_ros_prefix_if_exists("hello"));

    // ros_topic_prefix
    EXPECT_EQ("", get_ros_prefix_if_exists("hello/rt"));
    EXPECT_EQ("rt", get_ros_prefix_if_exists("rt/hello"));
    EXPECT_EQ("", get_ros_prefix_if_exists("/rt/hello"));

    // ros_service_requester_prefix
    EXPECT_EQ("", get_ros_prefix_if_exists("hello/rq"));
    EXPECT_EQ("rq", get_ros_prefix_if_exists("rq/hello"));
    EXPECT_EQ("", get_ros_prefix_if_exists("/rq/hello"));

    // ros_service_requester_prefix
    EXPECT_EQ("", get_ros_prefix_if_exists("hello/rr"));
    EXPECT_EQ("rr", get_ros_prefix_if_exists("rr/hello"));
    EXPECT_EQ("", get_ros_prefix_if_exists("/rr/hello"));
}

/**
 * @todo
 */
TEST(DemangleTest, remove_ros_prefix_if_exists)
{
    EXPECT_EQ("hello", remove_ros_prefix_if_exists("hello"));

    // ros_topic_prefix
    EXPECT_EQ("hello/rt", remove_ros_prefix_if_exists("hello/rt"));
    EXPECT_EQ("/hello", remove_ros_prefix_if_exists("rt/hello"));
    EXPECT_EQ("/rt/hello", remove_ros_prefix_if_exists("/rt/hello"));

    // ros_service_requester_prefix
    EXPECT_EQ("hello/rq", remove_ros_prefix_if_exists("hello/rq"));
    EXPECT_EQ("/hello", remove_ros_prefix_if_exists("rq/hello"));
    EXPECT_EQ("/rq/hello", remove_ros_prefix_if_exists("/rq/hello"));

    // ros_service_requester_prefix
    EXPECT_EQ("hello/rr", remove_ros_prefix_if_exists("hello/rr"));
    EXPECT_EQ("/hello", remove_ros_prefix_if_exists("rr/hello"));
    EXPECT_EQ("/rr/hello", remove_ros_prefix_if_exists("/rr/hello"));
}

/**
 * @todo
 */
TEST(DemangleTest, get_all_ros_prefixes)
{
    const std::vector<std::string> ros_prefixes = {"rt", "rq", "rr"};
    EXPECT_EQ(ros_prefixes, get_all_ros_prefixes());
}

/**
 * @todo
 */
TEST(DemangleTest, demangle_if_ros_topic)
{
    EXPECT_EQ("hello", demangle_if_ros_topic("hello"));

    // ros_topic_prefix
    EXPECT_EQ("hello/rt", demangle_if_ros_topic("hello/rt"));
    EXPECT_EQ("/hello", demangle_if_ros_topic("rt/hello"));
    EXPECT_EQ("/rt/hello", demangle_if_ros_topic("/rt/hello"));

    // ros_service_requester_prefix
    EXPECT_EQ("hello/rq", demangle_if_ros_topic("hello/rq"));
    EXPECT_EQ("/hello", demangle_if_ros_topic("rq/hello"));
    EXPECT_EQ("/rq/hello", demangle_if_ros_topic("/rq/hello"));

    // ros_service_requester_prefix
    EXPECT_EQ("hello/rr", demangle_if_ros_topic("hello/rr"));
    EXPECT_EQ("/hello", demangle_if_ros_topic("rr/hello"));
    EXPECT_EQ("/rr/hello", demangle_if_ros_topic("/rr/hello"));
}

/**
 * @todo
 */
TEST(DemangleTest, demangle_if_ros_type)
{
    // not a ROS type
    EXPECT_EQ("hello", demangle_if_ros_type("hello"));
    EXPECT_EQ("rt::hello", demangle_if_ros_type("rt::hello"));
    EXPECT_EQ("rt::dds_::hello", demangle_if_ros_type("rt::dds_::hello"));

    // ROS type
    EXPECT_EQ("rt/hello", demangle_if_ros_type("rt::dds_::hello_"));
}

/**
 * @todo
 */
TEST(DemangleTest, demangle_ros_topic_from_topic)
{
    EXPECT_EQ("", demangle_ros_topic_from_topic("hello"));
    EXPECT_EQ("", demangle_ros_topic_from_topic("hello/rt"));
    EXPECT_EQ("/hello", demangle_ros_topic_from_topic("rt/hello"));

    EXPECT_EQ("", demangle_ros_topic_from_topic("hello/rq"));
    EXPECT_EQ("", demangle_ros_topic_from_topic("rq/hello"));

    EXPECT_EQ("", demangle_ros_topic_from_topic("hello/rr"));
    EXPECT_EQ("", demangle_ros_topic_from_topic("rr/hello"));
}

/**
 * @todo
 */
TEST(DemangleTest, demangle_service_from_topic)
{
    EXPECT_EQ("", demangle_service_from_topic("hello"));

    EXPECT_EQ("", demangle_service_from_topic("rq/hello"));
    EXPECT_EQ("", demangle_service_from_topic("rr/hello"));

    EXPECT_EQ("/hello/world", demangle_service_from_topic("rq/hello/worldRequest"));
    EXPECT_EQ("/hello/world", demangle_service_from_topic("rr/hello/worldReply"));

    EXPECT_EQ("", demangle_service_from_topic("Request/hello/worldrq"));
    EXPECT_EQ("", demangle_service_from_topic("Reply/hello/worldrr"));
}

/**
 * @todo
 */
TEST(DemangleTest, demangle_service_request_from_topic)
{
    EXPECT_EQ("", demangle_service_request_from_topic("hello"));

    EXPECT_EQ("", demangle_service_request_from_topic("rq/hello"));
    EXPECT_EQ("", demangle_service_request_from_topic("rr/hello"));

    EXPECT_EQ("/hello/world", demangle_service_request_from_topic("rq/hello/worldRequest"));
    EXPECT_EQ("", demangle_service_request_from_topic("rr/hello/worldReply"));

    EXPECT_EQ("", demangle_service_request_from_topic("Request/hello/worldrq"));
    EXPECT_EQ("", demangle_service_request_from_topic("Reply/hello/worldrr"));
}

/**
 * @todo
 */
TEST(DemangleTest, demangle_service_reply_from_topic)
{
    EXPECT_EQ("", demangle_service_reply_from_topic("hello"));

    EXPECT_EQ("", demangle_service_reply_from_topic("rq/hello"));
    EXPECT_EQ("", demangle_service_reply_from_topic("rr/hello"));

    EXPECT_EQ("", demangle_service_reply_from_topic("rq/hello/worldRequest"));
    EXPECT_EQ("/hello/world", demangle_service_reply_from_topic("rr/hello/worldReply"));

    EXPECT_EQ("", demangle_service_reply_from_topic("Request/hello/worldrq"));
    EXPECT_EQ("", demangle_service_reply_from_topic("Reply/hello/worldrr"));
}

/**
 * @todo
 */
TEST(DemangleTest, demangle_service_type_only)
{
    // not a ROS service type
    EXPECT_EQ("", demangle_service_type_only("hello"));
    EXPECT_EQ("", demangle_service_type_only("rq/hello"));
    EXPECT_EQ("", demangle_service_type_only("rr/hello"));
    EXPECT_EQ("", demangle_service_type_only("rt/hello"));

    EXPECT_EQ("", demangle_service_type_only("hello"));
    EXPECT_EQ("", demangle_service_type_only("rq::dds_::hello"));
    EXPECT_EQ("", demangle_service_type_only("rr::dds_::hello"));
    EXPECT_EQ("", demangle_service_type_only("rt::dds_::hello"));

    // ROS service type
    EXPECT_EQ("", demangle_service_type_only("hello"));
    EXPECT_EQ("rq/hello", demangle_service_type_only("rq::dds_::hello_Request_"));
    EXPECT_EQ("rr/hello", demangle_service_type_only("rr::dds_::hello_Response_"));
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
