// Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <string>

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/type_representation/ITypeObjectRegistry.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObject.hpp>

#include <ddspipe_core/types/topic/rpc/RpcTopic.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe::core::types;

namespace test {

/**
 * Create a structure type called \c type_name and register it in the TypeObjectRegistry, both by its TypeObject and
 * under its name, returning the TypeIdentifiers registered under the name.
 */
fastdds::dds::xtypes::TypeIdentifierPair register_type_by_name(
        const std::string& type_name)
{
    auto factory = fastdds::dds::DynamicTypeBuilderFactory::get_instance();

    fastdds::dds::TypeDescriptor::_ref_type type_descriptor {fastdds::dds::traits<fastdds::dds::TypeDescriptor>::
                                                             make_shared()};
    type_descriptor->kind(fastdds::dds::TK_STRUCTURE);
    type_descriptor->name(type_name);
    fastdds::dds::DynamicTypeBuilder::_ref_type builder {factory->create_type(type_descriptor)};

    fastdds::dds::MemberDescriptor::_ref_type member_descriptor {fastdds::dds::traits<fastdds::dds::MemberDescriptor>
                                                                 ::make_shared()};
    member_descriptor->name("value");
    member_descriptor->type(factory->get_primitive_type(fastdds::dds::TK_INT32));
    builder->add_member(member_descriptor);

    auto& registry = fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry();

    fastdds::dds::xtypes::TypeIdentifierPair type_identifiers;
    EXPECT_EQ(fastdds::dds::RETCODE_OK,
            registry.register_typeobject_w_dynamic_type(builder->build(), type_identifiers));

    fastdds::dds::xtypes::TypeObject type_object;
    EXPECT_EQ(fastdds::dds::RETCODE_OK, registry.get_type_object(type_identifiers.type_identifier2(), type_object));

    fastdds::dds::xtypes::TypeIdentifierPair named_type_identifiers;
    EXPECT_EQ(fastdds::dds::RETCODE_OK,
            registry.register_type_object(type_name, type_object.complete(), named_type_identifiers));

    return named_type_identifiers;
}

/**
 * A ROS 2 service topic called \c topic_name, of type \c type_name and with \c type_identifiers.
 */
DdsTopic service_topic(
        const std::string& topic_name,
        const std::string& type_name,
        const fastdds::dds::xtypes::TypeIdentifierPair& type_identifiers)
{
    DdsTopic topic;
    topic.m_topic_name = topic_name;
    topic.type_name = type_name;
    topic.type_identifiers = type_identifiers;

    return topic;
}

} // namespace test

/**
 * The discovered request topic keeps its type information, and the deduced reply topic gets the type information
 * registered under its own type name - not a copy of the request's.
 */
TEST(RpcTopicTest, request_discovered_reply_registered)
{
    auto request_ids = test::register_type_by_name("test_a::srv::dds_::Service_Request_");
    auto reply_ids = test::register_type_by_name("test_a::srv::dds_::Service_Response_");

    RpcTopic rpc_topic(test::service_topic("rq/test_a/serviceRequest", "test_a::srv::dds_::Service_Request_",
            request_ids));

    ASSERT_TRUE(rpc_topic.request_topic().type_identifiers == request_ids);
    ASSERT_EQ(rpc_topic.reply_topic().type_name, "test_a::srv::dds_::Service_Response_");
    ASSERT_TRUE(rpc_topic.reply_topic().type_identifiers == reply_ids);
    ASSERT_FALSE(rpc_topic.reply_topic().type_identifiers == request_ids);
}

/**
 * The deduced reply topic has no type information when its type is not registered under its name.
 */
TEST(RpcTopicTest, request_discovered_reply_not_registered)
{
    auto request_ids = test::register_type_by_name("test_b::srv::dds_::Service_Request_");

    RpcTopic rpc_topic(test::service_topic("rq/test_b/serviceRequest", "test_b::srv::dds_::Service_Request_",
            request_ids));

    ASSERT_TRUE(rpc_topic.request_topic().type_identifiers == request_ids);
    ASSERT_TRUE(rpc_topic.reply_topic().type_identifiers == fastdds::dds::xtypes::TypeIdentifierPair());
}

/**
 * The same, the other way around: the discovered reply topic keeps its type information, and the deduced request
 * topic gets the type information registered under its own type name.
 */
TEST(RpcTopicTest, reply_discovered_request_registered)
{
    auto request_ids = test::register_type_by_name("test_c::srv::dds_::Service_Request_");
    auto reply_ids = test::register_type_by_name("test_c::srv::dds_::Service_Response_");

    RpcTopic rpc_topic(test::service_topic("rr/test_c/serviceReply", "test_c::srv::dds_::Service_Response_",
            reply_ids));

    ASSERT_TRUE(rpc_topic.reply_topic().type_identifiers == reply_ids);
    ASSERT_EQ(rpc_topic.request_topic().type_name, "test_c::srv::dds_::Service_Request_");
    ASSERT_TRUE(rpc_topic.request_topic().type_identifiers == request_ids);
}

/**
 * The deduced request topic has no type information when its type is not registered under its name.
 */
TEST(RpcTopicTest, reply_discovered_request_not_registered)
{
    auto reply_ids = test::register_type_by_name("test_d::srv::dds_::Service_Response_");

    RpcTopic rpc_topic(test::service_topic("rr/test_d/serviceReply", "test_d::srv::dds_::Service_Response_",
            reply_ids));

    ASSERT_TRUE(rpc_topic.reply_topic().type_identifiers == reply_ids);
    ASSERT_TRUE(rpc_topic.request_topic().type_identifiers == fastdds::dds::xtypes::TypeIdentifierPair());
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
