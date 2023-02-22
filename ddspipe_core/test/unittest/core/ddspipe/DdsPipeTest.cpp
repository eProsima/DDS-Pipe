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

#include <ddspipe_core/core/DdsPipe.hpp>
#include <ddspipe_core/efficiency/payload/FastPayloadPool.hpp>

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
        test::DdsPipe ddspipe(
            std::make_shared<AllowedTopicList>(),
            std::make_shared<DiscoveryDatabase>(),
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
            );

        ASSERT_FALSE(ddspipe.is_enabled());
    }

    // enable
    {
        test::DdsPipe ddspipe(
            std::make_shared<AllowedTopicList>(),
            std::make_shared<DiscoveryDatabase>(),
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS),
            {},
            true
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

        test::DdsPipe ddspipe(
            std::make_shared<AllowedTopicList>(),
            std::make_shared<DiscoveryDatabase>(),
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS),
        {
            htopic_1
        }
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

        test::DdsPipe ddspipe(
            std::make_shared<AllowedTopicList>(),
            std::make_shared<DiscoveryDatabase>(),
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS),
        {
            htopic_1
        },
            true
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
        auto discovery_database = std::make_shared<DiscoveryDatabase>();
        test::DdsPipe ddspipe(
            std::make_shared<AllowedTopicList>(),
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

        auto discovery_database = std::make_shared<DiscoveryDatabase>();
        test::DdsPipe ddspipe(
            std::make_shared<AllowedTopicList>(),
            std::make_shared<DiscoveryDatabase>(),
            std::make_shared<FastPayloadPool>(),
            std::make_shared<ParticipantsDatabase>(),
            std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS),
            {htopic_1}
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
 * TODO
 */
TEST(DdsPipeTest, allowed_blocked_topics)
{
    // TODO
}

/**
 * TODO
 */
TEST(DdsPipeTest, reload)
{
    // TODO
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
