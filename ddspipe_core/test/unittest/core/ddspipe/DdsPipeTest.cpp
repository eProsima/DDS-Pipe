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

#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_core/configuration/DdsPipeConfiguration.hpp>
#include <ddspipe_core/core/DdsPipe.hpp>
#include <ddspipe_core/efficiency/payload/FastPayloadPool.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

using namespace eprosima::ddspipe::core;

namespace test {

constexpr const unsigned int N_THREADS = 2;

struct DdsPipe : public eprosima::ddspipe::core::DdsPipe
{
    using eprosima::ddspipe::core::DdsPipe::DdsPipe;

    bool is_enabled() const
    {
        std::lock_guard<std::mutex> _(mutex_);
        return enabled_;
    }

    bool is_topic_discovered(
            const eprosima::utils::Heritable<types::DistributedTopic>& topic) const
    {
        std::lock_guard<std::mutex> _(mutex_);
        return current_topics_.find(topic) != current_topics_.end();
    }

    bool is_topic_discovered(
            const types::RpcTopic service) const
    {
        std::lock_guard<std::mutex> _(mutex_);
        return current_services_.find(service) != current_services_.end();
    }

    bool is_topic_active(
            const eprosima::utils::Heritable<types::DistributedTopic>& topic) const
    {
        std::lock_guard<std::mutex> _(mutex_);
        auto it = current_topics_.find(topic);
        if (it == current_topics_.end())
        {
            return false;
        }
        return it->second;
    }

    bool is_topic_active(
            const types::RpcTopic service) const
    {
        std::lock_guard<std::mutex> _(mutex_);
        auto it = current_services_.find(service);
        if (it == current_services_.end())
        {
            return false;
        }
        return it->second;
    }

    bool is_bridge_created(
            const eprosima::utils::Heritable<types::DistributedTopic>& topic) const
    {
        std::lock_guard<std::mutex> _(mutex_);
        return bridges_.find(topic) != bridges_.end();
    }

    bool is_bridge_created(
            const types::RpcTopic service) const
    {
        std::lock_guard<std::mutex> _(mutex_);
        return rpc_bridges_.find(service) != rpc_bridges_.end();
    }

};

} // test

/**
 * Test creation of a DdsPipe
 *
 * CASES:
 * - default
 * - enable
 * - with builtin_topics default
 * - with builtin_topics enable
 */
TEST(DdsPipeTest, default_initialization)
{
    // default
    {
        DdsPipeConfiguration ddspipe_configuration;

        test::DdsPipe ddspipe(
            ddspipe_configuration,
            std::make_shared<DiscoveryDatabase>(),
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
            );

        ASSERT_FALSE(ddspipe.is_enabled());
    }

    // enable
    {
        DdsPipeConfiguration ddspipe_configuration;
        ddspipe_configuration.init_enabled = true;

        test::DdsPipe ddspipe(
            ddspipe_configuration,
            std::make_shared<DiscoveryDatabase>(),
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
            );

        ASSERT_TRUE(ddspipe.is_enabled());
    }

    // with builtin_topics default
    {
        types::DdsTopic topic_1;
        topic_1.m_topic_name = "topic1";
        topic_1.type_name = "type1";
        eprosima::utils::Heritable<types::DistributedTopic> htopic_1 =
                eprosima::utils::Heritable<types::DdsTopic>::make_heritable(topic_1);

        DdsPipeConfiguration ddspipe_configuration;
        ddspipe_configuration.builtin_topics.insert(htopic_1);

        test::DdsPipe ddspipe(
            ddspipe_configuration,
            std::make_shared<DiscoveryDatabase>(),
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
            );

        ASSERT_FALSE(ddspipe.is_enabled());

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_FALSE(ddspipe.is_topic_active(htopic_1));
        ASSERT_TRUE(ddspipe.is_bridge_created(htopic_1));
    }

    // with builtin_topics enable
    {
        types::DdsTopic topic_1;
        topic_1.m_topic_name = "topic1";
        topic_1.type_name = "type1";
        eprosima::utils::Heritable<types::DistributedTopic> htopic_1 =
                eprosima::utils::Heritable<types::DdsTopic>::make_heritable(topic_1);

        DdsPipeConfiguration ddspipe_configuration;
        ddspipe_configuration.builtin_topics.insert(htopic_1);
        ddspipe_configuration.init_enabled = true;

        test::DdsPipe ddspipe(
            ddspipe_configuration,
            std::make_shared<DiscoveryDatabase>(),
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
            );

        ASSERT_TRUE(ddspipe.is_enabled());

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_TRUE(ddspipe.is_topic_active(htopic_1));
        ASSERT_TRUE(ddspipe.is_bridge_created(htopic_1));
    }
}

/**
 * Test enabling DDS Pipe
 *
 * CASES:
 * - enable discovered topic
 * - enable with builtin topic
 */
TEST(DdsPipeTest, enable_disable)
{
    // enable discovered topic
    {
        DdsPipeConfiguration ddspipe_configuration;

        auto discovery_database = std::make_shared<DiscoveryDatabase>();

        test::DdsPipe ddspipe(
            ddspipe_configuration,
            discovery_database,
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
            );

        types::DdsTopic topic_1;
        topic_1.m_topic_name = "topic1";
        topic_1.type_name = "type1";
        eprosima::utils::Heritable<types::DistributedTopic> htopic_1 =
                eprosima::utils::Heritable<types::DdsTopic>::make_heritable(topic_1);

        ASSERT_FALSE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_FALSE(ddspipe.is_topic_active(htopic_1));
        ASSERT_FALSE(ddspipe.is_bridge_created(htopic_1));

        types::Endpoint endpoint_1;
        endpoint_1.kind = types::EndpointKind::reader;
        endpoint_1.topic = topic_1;

        discovery_database->add_endpoint(endpoint_1);

        // Wait a bit for callback to arrive
        eprosima::utils::sleep_for(10u);

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_FALSE(ddspipe.is_topic_active(htopic_1));
        ASSERT_FALSE(ddspipe.is_bridge_created(htopic_1));

        ddspipe.enable();

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_TRUE(ddspipe.is_topic_active(htopic_1));
        ASSERT_TRUE(ddspipe.is_bridge_created(htopic_1));

        ddspipe.disable();

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_FALSE(ddspipe.is_topic_active(htopic_1));
        ASSERT_TRUE(ddspipe.is_bridge_created(htopic_1));
    }

    // enable with builtin topic
    {
        types::DdsTopic topic_1;
        topic_1.m_topic_name = "topic1";
        topic_1.type_name = "type1";
        eprosima::utils::Heritable<types::DistributedTopic> htopic_1 =
                eprosima::utils::Heritable<types::DdsTopic>::make_heritable(topic_1);

        DdsPipeConfiguration ddspipe_configuration;
        ddspipe_configuration.builtin_topics.insert(htopic_1);

        test::DdsPipe ddspipe(
            ddspipe_configuration,
            std::make_shared<DiscoveryDatabase>(),
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
            );

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_FALSE(ddspipe.is_topic_active(htopic_1));
        ASSERT_TRUE(ddspipe.is_bridge_created(htopic_1));

        ddspipe.enable();

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_TRUE(ddspipe.is_topic_active(htopic_1));
        ASSERT_TRUE(ddspipe.is_bridge_created(htopic_1));

        ddspipe.disable();

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_FALSE(ddspipe.is_topic_active(htopic_1));
        ASSERT_TRUE(ddspipe.is_bridge_created(htopic_1));
    }
}

/**
 * Test the DDS Pipe's allowlist and blocklist
 *
 * CASES:
 * - check that a topic is allowed when the allowed topics are empty.
 * - check that a topic is allowed when its in the allowlist.
 * - check that a topic is blocked when its in the blocklist.
 * - check that a topic is allowed when it gets removed from the blocklist.
 */
TEST(DdsPipeTest, allowed_blocked_topics)
{
    {
        DdsPipeConfiguration ddspipe_configuration;
        ddspipe_configuration.init_enabled = true;

        auto discovery_database = std::make_shared<DiscoveryDatabase>();

        test::DdsPipe ddspipe(
            ddspipe_configuration,
            discovery_database,
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
            );

        types::DdsTopic topic_1;
        topic_1.m_topic_name = "topic1";
        topic_1.type_name = "type1";
        eprosima::utils::Heritable<types::DistributedTopic> htopic_1 =
                eprosima::utils::Heritable<types::DdsTopic>::make_heritable(topic_1);

        ASSERT_FALSE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_FALSE(ddspipe.is_topic_active(htopic_1));
        ASSERT_FALSE(ddspipe.is_bridge_created(htopic_1));

        // Create a wildcard topic
        types::WildcardDdsFilterTopic topic_2;
        topic_2.topic_name.set_value("t*");

        eprosima::utils::Heritable<types::IFilterTopic> htopic_2 =
                eprosima::utils::Heritable<types::WildcardDdsFilterTopic>::make_heritable(topic_2);

        // Add the Topic to the blocklist
        DdsPipeConfiguration new_ddspipe_configuration;
        new_ddspipe_configuration.blocklist.insert(htopic_2);
        ddspipe.reload_configuration(new_ddspipe_configuration);

        types::Endpoint endpoint_1;
        endpoint_1.kind = types::EndpointKind::reader;
        endpoint_1.topic = topic_1;

        discovery_database->add_endpoint(endpoint_1);

        // Wait a bit for callback to arrive
        eprosima::utils::sleep_for(10u);

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_FALSE(ddspipe.is_topic_active(htopic_1));
        ASSERT_FALSE(ddspipe.is_bridge_created(htopic_1));

        // Remove the Topic from the blocklist
        new_ddspipe_configuration.blocklist.erase(htopic_2);
        ddspipe.reload_configuration(new_ddspipe_configuration);

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_TRUE(ddspipe.is_topic_active(htopic_1));
        ASSERT_TRUE(ddspipe.is_bridge_created(htopic_1));

        // Create two Wildcard Topics
        types::WildcardDdsFilterTopic topic_3;
        topic_3.topic_name.set_value("topic*");

        eprosima::utils::Heritable<types::IFilterTopic> htopic_3 =
                eprosima::utils::Heritable<types::WildcardDdsFilterTopic>::make_heritable(topic_3);

        types::WildcardDdsFilterTopic topic_4;
        topic_4.topic_name.set_value("top*");

        eprosima::utils::Heritable<types::IFilterTopic> htopic_4 =
                eprosima::utils::Heritable<types::WildcardDdsFilterTopic>::make_heritable(topic_4);

        // Add the Topic to the allowlist
        new_ddspipe_configuration.allowlist.insert(htopic_3);
        ddspipe.reload_configuration(new_ddspipe_configuration);

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_TRUE(ddspipe.is_topic_active(htopic_1));
        ASSERT_TRUE(ddspipe.is_bridge_created(htopic_1));

        // Add the Topic to the blocklist
        new_ddspipe_configuration.blocklist.insert(htopic_4);
        ddspipe.reload_configuration(new_ddspipe_configuration);

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_FALSE(ddspipe.is_topic_active(htopic_1));
        ASSERT_TRUE(ddspipe.is_bridge_created(htopic_1));

        // Remove the Topic from the blocklist
        new_ddspipe_configuration.blocklist.erase(htopic_4);
        ddspipe.reload_configuration(new_ddspipe_configuration);

        ASSERT_TRUE(ddspipe.is_topic_discovered(htopic_1));
        ASSERT_TRUE(ddspipe.is_topic_active(htopic_1));
        ASSERT_TRUE(ddspipe.is_bridge_created(htopic_1));
    }
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
