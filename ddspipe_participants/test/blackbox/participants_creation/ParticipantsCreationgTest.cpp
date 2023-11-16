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

#include <ddspipe_participants/participant/auxiliar/BlankParticipant.hpp>
#include <ddspipe_participants/participant/auxiliar/EchoParticipant.hpp>
#include <ddspipe_participants/participant/rtps/DiscoveryServerParticipant.hpp>
#include <ddspipe_participants/participant/rtps/SimpleParticipant.hpp>
#include <ddspipe_participants/participant/rtps/CommonParticipant.hpp>
#include <ddspipe_participants/participant/rtps/InitialPeersParticipant.hpp>
#include <ddspipe_participants/participant/dds/XmlParticipant.hpp>
#include <ddspipe_participants/testing/entities/mock_entities.hpp>
#include <ddspipe_participants/testing/random_values.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe;

namespace test {

constexpr const unsigned int N_THREADS = 2;

} // test

/**
 * Test to check default participant configuration values.
 */
TEST(ParticipantsCreationgTest, default_configuration)
{
    // Common configuration
    {
        participants::ParticipantConfiguration conf;
        EXPECT_EQ(conf.app_id, "UNKNOWN_APP");
        EXPECT_EQ(conf.app_metadata, "");
        EXPECT_TRUE(conf.id.empty());
        EXPECT_FALSE(conf.is_repeater);
    }
}

/**
 * Test to create a participant of each kind and check it does not fail.
 *
 * CASES:
 * - Blank
 * - Echo
 * - Simple
 * - Discovery Server
 * - Initial Peers
 */
TEST(ParticipantsCreationgTest, creation_trivial)
{
    // Auxiliar objects
    std::shared_ptr<core::PayloadPool> payload_pool(new core::FastPayloadPool());
    std::shared_ptr<core::DiscoveryDatabase> discovery_database(new core::DiscoveryDatabase());
    core::types::ParticipantId part_id("testPart");

    // Blank
    {
        participants::BlankParticipant participant(part_id);
    }

    // Echo
    {
        std::shared_ptr<participants::EchoParticipantConfiguration> conf(
            new participants::EchoParticipantConfiguration());
        conf->id = part_id;

        participants::EchoParticipant participant(conf, discovery_database);
    }

    // Simple
    {
        std::shared_ptr<participants::SimpleParticipantConfiguration> conf(
            new participants::SimpleParticipantConfiguration());
        conf->id = part_id;

        participants::rtps::SimpleParticipant participant(conf, payload_pool, discovery_database);
        participant.init();
    }

    // Discovery Server
    {
        std::shared_ptr<participants::InitialPeersParticipantConfiguration> conf(
            new participants::InitialPeersParticipantConfiguration());
        conf->id = part_id;
        conf->listening_addresses.insert(participants::testing::random_address());

        participants::rtps::InitialPeersParticipant participant(conf, payload_pool, discovery_database);
        participant.init();
    }

    // Initial Peers
    {
        std::shared_ptr<participants::DiscoveryServerParticipantConfiguration> conf(
            new participants::DiscoveryServerParticipantConfiguration());
        conf->id = part_id;
        conf->listening_addresses.insert(participants::testing::random_address());

        participants::rtps::DiscoveryServerParticipant participant(conf, payload_pool, discovery_database);
        participant.init();
    }

    // Xml Participant
    {
        std::shared_ptr<participants::XmlParticipantConfiguration> conf(
            new participants::XmlParticipantConfiguration());
        conf->id = part_id;

        participants::dds::XmlParticipant participant(conf, payload_pool, discovery_database);
        participant.init();
    }
}

/**
 * Test to create a participant of each kind and add them to a DDS Pipe that uses a builtin topic.
 * This will force every participant to create an endpoint.
 * This checks the endpoint creation does not fail.
 */
TEST(ParticipantsCreationgTest, ddspipe_all_creation_builtin_topic)
{
    // Auxiliar objects
    std::shared_ptr<core::DiscoveryDatabase> discovery_database(new core::DiscoveryDatabase());
    std::shared_ptr<core::PayloadPool> payload_pool(new core::FastPayloadPool());
    std::shared_ptr<core::ParticipantsDatabase> part_db(new core::ParticipantsDatabase());
    std::shared_ptr<utils::SlotThreadPool> thread_pool(new utils::SlotThreadPool(test::N_THREADS));

    // Blank
    {
        core::types::ParticipantId part_id("Blank");
        part_db->add_participant(part_id, std::make_shared<participants::BlankParticipant>(part_id));
    }

    // Echo
    {
        std::shared_ptr<participants::EchoParticipantConfiguration> conf(
            new participants::EchoParticipantConfiguration());
        conf->id = core::types::ParticipantId("Echo");

        part_db->add_participant(conf->id, std::make_shared<participants::EchoParticipant>(
                    conf, discovery_database));
    }

    // Simple
    {
        std::shared_ptr<participants::SimpleParticipantConfiguration> conf(
            new participants::SimpleParticipantConfiguration());
        conf->id = core::types::ParticipantId("Simple");

        auto part = std::make_shared<participants::rtps::SimpleParticipant>(
            conf, payload_pool, discovery_database);
        part->init();
        part_db->add_participant(conf->id, part);
    }

    // Discovery Server
    {
        std::shared_ptr<participants::DiscoveryServerParticipantConfiguration> conf(
            new participants::DiscoveryServerParticipantConfiguration());
        conf->id = core::types::ParticipantId("DiscoveryServer");
        conf->listening_addresses.insert(participants::testing::random_address(1));

        auto part = std::make_shared<participants::rtps::DiscoveryServerParticipant>(
            conf, payload_pool, discovery_database);
        part->init();
        part_db->add_participant(conf->id, part);
    }

    // Initial Peers
    {
        std::shared_ptr<participants::InitialPeersParticipantConfiguration> conf(
            new participants::InitialPeersParticipantConfiguration());
        conf->id = core::types::ParticipantId("InitialPeers");
        conf->listening_addresses.insert(participants::testing::random_address(2));

        auto part = std::make_shared<participants::rtps::InitialPeersParticipant>(
            conf, payload_pool, discovery_database);
        part->init();
        part_db->add_participant(conf->id, part);
    }

    // Xml
    {
        std::shared_ptr<participants::XmlParticipantConfiguration> conf(
            new participants::XmlParticipantConfiguration());
        conf->id = core::types::ParticipantId("Xml");

        auto part = std::make_shared<participants::dds::XmlParticipant>(
            conf, payload_pool, discovery_database);
        part->init();
        part_db->add_participant(conf->id, part);
    }

    // Topic
    core::types::DdsTopic topic_1;
    topic_1.m_topic_name = "topic1";
    topic_1.type_name = "type1";
    eprosima::utils::Heritable<core::types::DistributedTopic> htopic_1 =
            eprosima::utils::Heritable<core::types::DdsTopic>::make_heritable(topic_1);

    // Special QoS Topic
    core::types::DdsTopic topic_2;
    topic_2.m_topic_name = "topic2";
    topic_2.type_name = "type2";
    topic_2.topic_qos.keyed = true;
    topic_2.topic_qos.ownership_qos = eprosima::ddspipe::core::types::OwnershipQosPolicyKind::EXCLUSIVE_OWNERSHIP_QOS;
    topic_2.topic_qos.use_partitions = true;
    eprosima::utils::Heritable<core::types::DistributedTopic> htopic_2 =
            eprosima::utils::Heritable<core::types::DdsTopic>::make_heritable(topic_2);

    // Create DDS Pipe
    core::DdsPipeConfiguration ddspipe_configuration;
    ddspipe_configuration.builtin_topics.insert(htopic_1);
    ddspipe_configuration.builtin_topics.insert(htopic_2);
    ddspipe_configuration.init_enabled = true;

    core::DdsPipe ddspipe(
        ddspipe_configuration,
        discovery_database,
        payload_pool,
        part_db,
        thread_pool
        );

    // Let everything destroy itself
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
