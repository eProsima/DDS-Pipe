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

#include <sstream>

#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_core/configuration/DdsPipeConfiguration.hpp>
#include <ddspipe_core/core/DdsPipe.hpp>
#include <ddspipe_core/dynamic/AllowedTopicList.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>
#include <ddspipe_core/efficiency/payload/FastPayloadPool.hpp>
#include <ddspipe_core/testing/random_values.hpp>

#include <ddspipe_participants/testing/entities/mock_entities.hpp>
#include <ddspipe_participants/utils/utils.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe;

namespace test {

constexpr const unsigned int N_THREADS = 2;
constexpr const unsigned int N_MESSAGES = 5;
constexpr const unsigned int N_PARTICIPANTS = 3;
constexpr const unsigned int N_TOPICS = 2;

participants::testing::MockRoutingData new_data(
        const core::types::ParticipantId& id,
        unsigned int index)
{
    participants::testing::MockRoutingData new_data;
    new_data.data = id + std::string("::") + std::to_string(index);
    return new_data;
}

} // test

/**
 * Test a DDS Pipe execution with mock participants
 */
TEST(DdsPipeCommunicationMockTest, mock_communication_trivial)
{
    // Topic to send data
    participants::testing::MockTopic topic_1;
    topic_1.m_topic_name = "topic1";
    eprosima::utils::Heritable<core::types::DistributedTopic> htopic_1 =
            eprosima::utils::Heritable<participants::testing::MockTopic>::make_heritable(topic_1);

    // Create Participants
    core::types::ParticipantId part_1_id("Participant_1");
    auto part_1 = std::make_shared<participants::testing::MockParticipant>(part_1_id);

    core::types::ParticipantId part_2_id("Participant_2");
    auto part_2 = std::make_shared<participants::testing::MockParticipant>(part_2_id);

    auto part_db = std::make_shared<core::ParticipantsDatabase>();
    part_db->add_participant(part_1_id, part_1);
    part_db->add_participant(part_2_id, part_2);

    // Create DDS Pipe
    core::DdsPipeConfiguration ddspipe_configuration;
    ddspipe_configuration.builtin_topics.insert(htopic_1);
    ddspipe_configuration.init_enabled = true;

    core::DdsPipe ddspipe(
        ddspipe_configuration,
        std::make_shared<core::DiscoveryDatabase>(),
        std::make_shared<core::FastPayloadPool>(),
        part_db,
        std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
        );

    // Look for the reader in participant 1 and writer in participant 2
    auto reader_1 = part_1->get_reader(topic_1);
    auto writer_2 = part_2->get_writer(topic_1);
    ASSERT_NE(reader_1, nullptr);
    ASSERT_NE(writer_2, nullptr);

    // Simulate N messages
    for (unsigned int i = 0; i < test::N_MESSAGES; i++)
    {
        reader_1->simulate_data_reception(test::new_data(part_1_id, i));
    }

    // Wait for all messages
    for (unsigned int i = 0; i < test::N_MESSAGES; i++)
    {
        auto received_data = writer_2->wait_data();
        ASSERT_EQ(received_data, test::new_data(part_1_id, i));
    }
}

/**
 * Test a DDS Pipe execution with mock participants when sending messages before enabling the pipe.
 * Also test it disabling, sending data and enabling again.
 *
 * STEPS:
 * - Create entities (disable)
 * - Send N messages
 * - Check that no messages has arrive
 * - Enable
 * - Wait for N messages
 * - Send N messages
 * - Wait for N messages
 * - Disable
 * - Send N messages
 * - Check that no messages has arrive
 * - Enable
 * - Wait for N messages
 */
TEST(DdsPipeCommunicationMockTest, mock_communication_before_enabling)
{
    // Topic to send data
    participants::testing::MockTopic topic_1;
    topic_1.m_topic_name = "topic1";
    eprosima::utils::Heritable<core::types::DistributedTopic> htopic_1 =
            eprosima::utils::Heritable<participants::testing::MockTopic>::make_heritable(topic_1);

    // Create Participants
    core::types::ParticipantId part_1_id("Participant_1");
    auto part_1 = std::make_shared<participants::testing::MockParticipant>(part_1_id);

    core::types::ParticipantId part_2_id("Participant_2");
    auto part_2 = std::make_shared<participants::testing::MockParticipant>(part_2_id);

    auto part_db = std::make_shared<core::ParticipantsDatabase>();
    part_db->add_participant(part_1_id, part_1);
    part_db->add_participant(part_2_id, part_2);

    // Create DDS Pipe
    core::DdsPipeConfiguration ddspipe_configuration;
    ddspipe_configuration.builtin_topics.insert(htopic_1);

    core::DdsPipe ddspipe(
        ddspipe_configuration,
        std::make_shared<core::DiscoveryDatabase>(),
        std::make_shared<core::FastPayloadPool>(),
        part_db,
        std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
        );

    // Look for the reader in participant 1 and writer in participant 2
    auto reader_1 = part_1->get_reader(topic_1);
    auto writer_2 = part_2->get_writer(topic_1);
    ASSERT_NE(reader_1, nullptr);
    ASSERT_NE(writer_2, nullptr);

    // Simulate N messages
    for (unsigned int i = 0; i < test::N_MESSAGES; i++)
    {
        reader_1->simulate_data_reception(test::new_data(part_1_id, i));
    }

    // Check that writer has not received nothing
    utils::sleep_for(10);
    ASSERT_EQ(writer_2->n_to_send_data(), 0u);

    // Enable Pipe
    ddspipe.enable();

    // Wait for all messages
    for (unsigned int i = 0; i < test::N_MESSAGES; i++)
    {
        auto received_data = writer_2->wait_data();
        ASSERT_EQ(received_data, test::new_data(part_1_id, i));
    }

    // Simulate N messages
    for (unsigned int i = test::N_MESSAGES; i < test::N_MESSAGES*2; i++)
    {
        reader_1->simulate_data_reception(test::new_data(part_1_id, i));
    }

    // Wait for all messages
    for (unsigned int i = test::N_MESSAGES; i < test::N_MESSAGES*2; i++)
    {
        auto received_data = writer_2->wait_data();
        ASSERT_EQ(received_data, test::new_data(part_1_id, i));
    }

    // Disable Pipe
    ddspipe.disable();

    // Simulate N messages
    for (unsigned int i = test::N_MESSAGES * 2; i < test::N_MESSAGES*3; i++)
    {
        reader_1->simulate_data_reception(test::new_data(part_1_id, i));
    }

    // Check that writer has not received nothing
    utils::sleep_for(10);
    ASSERT_EQ(writer_2->n_to_send_data(), 0u);

    // Enable Pipe
    ddspipe.enable();

    // Wait for all messages
    for (unsigned int i = test::N_MESSAGES * 2; i < test::N_MESSAGES*3; i++)
    {
        auto received_data = writer_2->wait_data();
        ASSERT_EQ(received_data, test::new_data(part_1_id, i));
    }
}

/**
 * Test a DDS Pipe execution with mock participants using a discovered topic by a simulated endpoint
 */
TEST(DdsPipeCommunicationMockTest, mock_communication_topic_discovery)
{
    // Topic to send data
    core::types::DdsTopic topic_1;
    topic_1.m_topic_name = "topic1";
    topic_1.type_name = "type1";
    topic_1.m_internal_type_discriminator = participants::testing::INTERNAL_TOPIC_TYPE_MOCK_TEST;
    eprosima::utils::Heritable<core::types::DistributedTopic> htopic_1 =
            eprosima::utils::Heritable<core::types::DdsTopic>::make_heritable(topic_1);

    // Create Participants
    core::types::ParticipantId part_1_id("Participant_1");
    auto part_1 = std::make_shared<participants::testing::MockParticipant>(part_1_id);

    core::types::ParticipantId part_2_id("Participant_2");
    auto part_2 = std::make_shared<participants::testing::MockParticipant>(part_2_id);

    auto disc_db = std::make_shared<core::DiscoveryDatabase>();

    auto part_db = std::make_shared<core::ParticipantsDatabase>();
    part_db->add_participant(part_1_id, part_1);
    part_db->add_participant(part_2_id, part_2);

    // Create DDS Pipe
    core::DdsPipeConfiguration ddspipe_configuration;

    core::DdsPipe ddspipe(
        ddspipe_configuration,
        disc_db,
        std::make_shared<core::FastPayloadPool>(),
        part_db,
        std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
        );
    ddspipe.enable();

    // Check there are no endpoints created yet
    ASSERT_EQ(part_1->n_readers(), 0);
    ASSERT_EQ(part_1->n_writers(), 0);
    ASSERT_EQ(part_2->n_readers(), 0);
    ASSERT_EQ(part_2->n_writers(), 0);

    // Simulate endpoint to discover topic
    core::types::Endpoint endpoint = core::testing::random_endpoint();
    endpoint.kind = core::types::EndpointKind::reader;
    endpoint.topic = topic_1;
    disc_db->add_endpoint(endpoint);

    // Wait for entities to be created
    utils::sleep_for(100);

    // Look for the reader in participant 1 and writer in participant 2
    auto reader_1 = part_1->get_reader(topic_1);
    auto writer_2 = part_2->get_writer(topic_1);
    ASSERT_NE(reader_1, nullptr);
    ASSERT_NE(writer_2, nullptr);

    // Simulate N messages
    for (unsigned int i = 0; i < test::N_MESSAGES; i++)
    {
        reader_1->simulate_data_reception(test::new_data(part_1_id, i));
    }

    // Wait for all messages
    for (unsigned int i = 0; i < test::N_MESSAGES; i++)
    {
        auto received_data = writer_2->wait_data();
        ASSERT_EQ(received_data, test::new_data(part_1_id, i));
    }
}

/**
 * Test that rediscovering an endpoint with the same partition mapping does not
 * update the bridge reader partitions again.
 *
 * The first endpoint creates the bridge. A second endpoint adds a new mapping
 * and must update the bridge once. Re-announcing that second endpoint with the
 * same mapping must not trigger another update.
 */
TEST(DdsPipeCommunicationMockTest, mock_communication_partition_filter_propagation)
{
    core::types::DdsTopic topic_1;
    topic_1.m_topic_name = "topic1";
    topic_1.type_name = "type1";
    topic_1.m_internal_type_discriminator = participants::testing::INTERNAL_TOPIC_TYPE_MOCK_TEST;

    core::types::ParticipantId part_1_id("Participant_1");
    auto part_1 = std::make_shared<participants::testing::MockParticipant>(part_1_id);

    core::types::ParticipantId part_2_id("Participant_2");
    auto part_2 = std::make_shared<participants::testing::MockParticipant>(part_2_id);

    auto part_db = std::make_shared<core::ParticipantsDatabase>();
    part_db->add_participant(part_1_id, part_1);
    part_db->add_participant(part_2_id, part_2);

    auto discovery_database = std::make_shared<core::DiscoveryDatabase>();

    core::DdsPipeConfiguration ddspipe_configuration;
    ddspipe_configuration.init_enabled = true;

    core::DdsPipe ddspipe(
        ddspipe_configuration,
        discovery_database,
        std::make_shared<core::FastPayloadPool>(),
        part_db,
        std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
        );

    core::types::Endpoint first_endpoint;
    first_endpoint.kind = core::types::EndpointKind::reader;
    first_endpoint.guid = core::testing::random_guid(1);
    first_endpoint.topic = topic_1;
    first_endpoint.discoverer_participant_id = core::types::ParticipantId("RemoteParticipant");
    first_endpoint.specific_qos.partitions.push_back("partition");

    discovery_database->add_endpoint(first_endpoint);
    utils::sleep_for(100);

    auto reader_1 = part_1->get_reader(topic_1);
    ASSERT_NE(reader_1, nullptr);
    ASSERT_EQ(reader_1->n_partition_updates(), 0u);

    // Discovering more endpoints, whatever partitions they announce, must never touch the Reader's
    // partition QoS. The partitions a Reader subscribes to are a policy, and only the filter sets
    // them; the partitions announced by remote endpoints are observed facts and live in the
    // DiscoveryDatabase, where every consumer queries them.
    core::types::Endpoint second_endpoint = first_endpoint;
    second_endpoint.guid = core::testing::random_guid(2);
    second_endpoint.specific_qos.partitions.clear();
    second_endpoint.specific_qos.partitions.push_back("other_partition");

    discovery_database->add_endpoint(second_endpoint);
    utils::sleep_for(100);

    EXPECT_EQ(reader_1->n_partition_updates(), 0u);

    discovery_database->update_endpoint(second_endpoint);
    utils::sleep_for(100);

    EXPECT_EQ(reader_1->n_partition_updates(), 0u);

    // Changing the filter, and only that, re-applies the Reader's partition QoS.
    ddspipe.set_partition_filter({"partition"});
    utils::sleep_for(100);

    EXPECT_EQ(reader_1->n_partition_updates(), 1u);

    // Both endpoints announce partitions, so the topic is partition aware. This is the single
    // answer to that question, and it is a live query rather than a cached map.
    EXPECT_TRUE(discovery_database->topic_has_partitions(topic_1.m_topic_name));
    EXPECT_FALSE(discovery_database->topic_has_partitions("a_topic_that_does_not_exist"));
}

/**
 * Test that a newly discovered endpoint is merged into an already existing bridge.
 *
 * This is the replacement-writer path: the bridge exists because the topic is builtin, while the
 * endpoint is discovered afterwards. The replacement writer must be created with the endpoint's
 * partition map.
 */
TEST(DdsPipeCommunicationMockTest, mock_communication_existing_bridge_partition_discovery)
{
    core::types::DdsTopic topic;
    topic.m_topic_name = "topic_with_partition_discovery";
    topic.type_name = "partition_discovery_type";
    topic.m_internal_type_discriminator = participants::testing::INTERNAL_TOPIC_TYPE_MOCK_TEST;

    auto htopic = utils::Heritable<core::types::DistributedTopic>::make_heritable(topic);

    core::types::ParticipantId part_1_id("Participant_1");
    auto part_1 = std::make_shared<participants::testing::MockParticipant>(part_1_id);

    core::types::ParticipantId part_2_id("Participant_2");
    auto part_2 = std::make_shared<participants::testing::MockParticipant>(part_2_id);

    auto discovery_database = std::make_shared<core::DiscoveryDatabase>();
    auto participants_database = std::make_shared<core::ParticipantsDatabase>();
    participants_database->add_participant(part_1_id, part_1);
    participants_database->add_participant(part_2_id, part_2);

    core::DdsPipeConfiguration configuration;
    configuration.builtin_topics.insert(htopic);
    configuration.init_enabled = true;
    configuration.remove_unused_entities = true;

    core::DdsPipe ddspipe(
        configuration,
        discovery_database,
        std::make_shared<core::FastPayloadPool>(),
        participants_database,
        std::make_shared<utils::SlotThreadPool>(test::N_THREADS));

    core::types::Endpoint endpoint = core::testing::random_endpoint();
    endpoint.kind = core::types::EndpointKind::reader;
    endpoint.topic = topic;
    endpoint.discoverer_participant_id = part_1_id;

    std::ostringstream guid;
    guid << endpoint.guid;
    endpoint.specific_partitions[guid.str()] = "partition_from_discovery";

    discovery_database->add_endpoint(endpoint);
    utils::sleep_for(100);

    const auto partitions = part_1->get_writer_topic_partitions(topic);
    ASSERT_EQ(partitions.size(), 1u);
    ASSERT_EQ(partitions.at(guid.str()), "partition_from_discovery");
}

/**
 * Test a DDS Pipe execution with mock participants using a builtin topic and allow or forbid it
 *
 * STEPS:
 * - create pipe with builtin blocked
 * - send data
 * - check no data has arrived
 * - allow topic
 * - check data has been sent
 * - block topic
 * - send data
 * - check no data has arrived
 */
TEST(DdsPipeCommunicationMockTest, mock_communication_topic_allow)
{
    // Topic to send data
    core::types::DdsTopic topic_1;
    topic_1.m_topic_name = "topic1";
    topic_1.type_name = "type1";
    topic_1.m_internal_type_discriminator = participants::testing::INTERNAL_TOPIC_TYPE_MOCK_TEST;
    eprosima::utils::Heritable<core::types::DistributedTopic> htopic_1 =
            eprosima::utils::Heritable<core::types::DdsTopic>::make_heritable(topic_1);

    // Create Participants
    core::types::ParticipantId part_1_id("Participant_1");
    auto part_1 = std::make_shared<participants::testing::MockParticipant>(part_1_id);

    core::types::ParticipantId part_2_id("Participant_2");
    auto part_2 = std::make_shared<participants::testing::MockParticipant>(part_2_id);

    auto part_db = std::make_shared<core::ParticipantsDatabase>();
    part_db->add_participant(part_1_id, part_1);
    part_db->add_participant(part_2_id, part_2);

    // Blocks all topics
    utils::Heritable<core::types::IFilterTopic> filter_topic =
            utils::Heritable<participants::testing::MockFilterAllTopic>::make_heritable();

    // Create DDS Pipe
    core::DdsPipeConfiguration ddspipe_configuration;
    ddspipe_configuration.blocklist.insert(filter_topic);
    ddspipe_configuration.builtin_topics.insert(htopic_1);
    ddspipe_configuration.init_enabled = true;

    core::DdsPipe ddspipe(
        ddspipe_configuration,
        std::make_shared<core::DiscoveryDatabase>(),
        std::make_shared<core::FastPayloadPool>(),
        part_db,
        std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
        );

    // Look for the reader in participant 1 and writer in participant 2
    auto reader_1 = part_1->get_reader(topic_1);
    auto writer_2 = part_2->get_writer(topic_1);
    ASSERT_NE(reader_1, nullptr);
    ASSERT_NE(writer_2, nullptr);

    // Simulate N messages
    for (unsigned int i = 0; i < test::N_MESSAGES; i++)
    {
        reader_1->simulate_data_reception(test::new_data(part_1_id, i));
    }

    // Check that writer has not received nothing as the topic is blocked
    utils::sleep_for(10);
    ASSERT_EQ(writer_2->n_to_send_data(), 0u);

    // Allow topic (empty allowed list allows everything)
    core::DdsPipeConfiguration new_ddspipe_configuration;
    new_ddspipe_configuration.allowlist.clear();
    new_ddspipe_configuration.blocklist.clear();

    ddspipe.reload_configuration(new_ddspipe_configuration);

    // Wait for all messages
    for (unsigned int i = 0; i < test::N_MESSAGES; i++)
    {
        auto received_data = writer_2->wait_data();
        ASSERT_EQ(received_data, test::new_data(part_1_id, i));
    }

    // Block topic
    new_ddspipe_configuration.blocklist.insert(filter_topic);

    ddspipe.reload_configuration(new_ddspipe_configuration);

    // Simulate N messages
    for (unsigned int i = 0; i < test::N_MESSAGES; i++)
    {
        reader_1->simulate_data_reception(test::new_data(part_1_id, i));
    }

    // Check that writer has not received nothing as the topic is blocked
    utils::sleep_for(10);
    ASSERT_EQ(writer_2->n_to_send_data(), 0u);
}

/**
 * Test a DDS Pipe execution with 3 mock participants and 2 topics in all directions
 */
TEST(DdsPipeCommunicationMockTest, mock_communication_multiple_participant_topics)
{
    // Create all topics required
    std::vector<std::pair<participants::testing::MockTopic, eprosima::utils::Heritable<core::types::DistributedTopic>>>
    topics;
    std::set<utils::Heritable<core::types::DistributedTopic>> builtin;
    for (unsigned int i = 0; i < test::N_TOPICS; i++)
    {
        participants::testing::MockTopic topic;
        topic.m_topic_name = "topic" + std::to_string(i);
        eprosima::utils::Heritable<core::types::DistributedTopic> htopic =
                eprosima::utils::Heritable<participants::testing::MockTopic>::make_heritable(topic);
        topics.push_back({topic, htopic});
        builtin.insert(htopic);
    }

    // Create all participants
    auto part_db = std::make_shared<core::ParticipantsDatabase>();
    std::vector<std::shared_ptr<participants::testing::MockParticipant>> participants;
    for (unsigned int i = 0; i < test::N_PARTICIPANTS; i++)
    {
        core::types::ParticipantId part_id("Participant_" + std::to_string(i));
        auto part = std::make_shared<participants::testing::MockParticipant>(part_id);
        participants.push_back(part);
        part_db->add_participant(part_id, part);
    }

    // Create DDS Pipe
    core::DdsPipeConfiguration ddspipe_configuration;
    ddspipe_configuration.builtin_topics = builtin;
    ddspipe_configuration.init_enabled = true;

    core::DdsPipe ddspipe(
        ddspipe_configuration,
        std::make_shared<core::DiscoveryDatabase>(),
        std::make_shared<core::FastPayloadPool>(),
        part_db,
        std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
        );

    // For every reader in every participant, send N data
    for (unsigned int i = 0; i < test::N_PARTICIPANTS; i++)
    {
        for (unsigned int j = 0; j < test::N_TOPICS; j++)
        {
            auto reader = participants[i]->get_reader(topics[j].first);
            ASSERT_NE(reader, nullptr);

            // Simulate N messages
            for (unsigned int i = 0; i < test::N_MESSAGES; i++)
            {
                participants::testing::MockRoutingData data;
                data.data = "some_data";
                reader->simulate_data_reception(participants::testing::MockRoutingData(data));
            }
        }
    }

    // Check that every writer has the amount of data expected
    for (unsigned int i = 0; i < test::N_PARTICIPANTS; i++)
    {
        for (unsigned int j = 0; j < test::N_TOPICS; j++)
        {
            auto writer = participants[i]->get_writer(topics[j].first);
            ASSERT_NE(writer, nullptr);

            // Wait for (#Participants x #messages) messages
            for (unsigned int i = 0; i < test::N_MESSAGES; i++)
            {
                writer->wait_data();
            }
        }
    }
}


/**
 * Test that changing the partition of an already discovered endpoint is visible to the live
 * writer QoS lookup used when a sample is received.
 *
 * This intentionally exercises the single relevant endpoint path. It is the path used when one
 * publisher is discovered and its partition is changed while it is still alive.
 */
TEST(DdsPipeCommunicationMockTest, mock_communication_endpoint_partition_update)
{
    core::types::DdsTopic topic;
    topic.m_topic_name = "topic_with_partition_update";
    topic.type_name = "partition_update_type";
    topic.m_internal_type_discriminator = participants::testing::INTERNAL_TOPIC_TYPE_MOCK_TEST;

    core::types::ParticipantId part_1_id("Participant_1");
    auto part_1 = std::make_shared<participants::testing::MockParticipant>(part_1_id);

    core::types::ParticipantId part_2_id("Participant_2");
    auto part_2 = std::make_shared<participants::testing::MockParticipant>(part_2_id);

    auto discovery_database = std::make_shared<core::DiscoveryDatabase>();
    auto participants_database = std::make_shared<core::ParticipantsDatabase>();
    participants_database->add_participant(part_1_id, part_1);
    participants_database->add_participant(part_2_id, part_2);

    core::DdsPipeConfiguration configuration;
    configuration.init_enabled = true;
    configuration.remove_unused_entities = false;
    configuration.discovery_trigger = core::DiscoveryTrigger::WRITER;

    core::DdsPipe ddspipe(
        configuration,
        discovery_database,
        std::make_shared<core::FastPayloadPool>(),
        participants_database,
        std::make_shared<utils::SlotThreadPool>(test::N_THREADS));

    core::types::Endpoint endpoint = core::testing::random_endpoint(1);
    endpoint.kind = core::types::EndpointKind::writer;
    endpoint.active = true;
    endpoint.topic = topic;
    endpoint.discoverer_participant_id = part_1_id;

    endpoint.specific_qos.partitions.push_back("partition_a");

    discovery_database->add_endpoint(endpoint);
    utils::sleep_for(100);

    core::types::SpecificEndpointQoS writer_qos;
    ASSERT_TRUE(participants::detail::try_specific_qos_of_writer_(
                *discovery_database, endpoint.guid, writer_qos));
    ASSERT_EQ(writer_qos.partitions.names(), std::vector<std::string>{"partition_a"});

    endpoint.specific_qos.partitions.clear();
    endpoint.specific_qos.partitions.push_back("partition_b");
    discovery_database->update_endpoint(endpoint);
    utils::sleep_for(100);

    writer_qos = {};
    ASSERT_TRUE(participants::detail::try_specific_qos_of_writer_(*discovery_database, endpoint.guid, writer_qos));
    EXPECT_EQ(writer_qos.partitions.names(), std::vector<std::string>{"partition_b"});
}

/**
 * Test that replacing an endpoint removes its old partition and makes the replacement endpoint's
 * partition available to the live writer QoS lookup.
 *
 * This models a publisher stopping and another publisher on the same topic appearing with a
 * different partition.
 */
TEST(DdsPipeCommunicationMockTest, mock_communication_replaced_endpoint_partition_update)
{
    core::types::DdsTopic topic;
    topic.m_topic_name = "topic_with_replaced_partition";
    topic.type_name = "replaced_partition_type";
    topic.m_internal_type_discriminator = participants::testing::INTERNAL_TOPIC_TYPE_MOCK_TEST;

    core::types::ParticipantId part_1_id("Participant_1");
    auto part_1 = std::make_shared<participants::testing::MockParticipant>(part_1_id);

    core::types::ParticipantId part_2_id("Participant_2");
    auto part_2 = std::make_shared<participants::testing::MockParticipant>(part_2_id);

    auto discovery_database = std::make_shared<core::DiscoveryDatabase>();
    auto participants_database = std::make_shared<core::ParticipantsDatabase>();
    participants_database->add_participant(part_1_id, part_1);
    participants_database->add_participant(part_2_id, part_2);

    core::DdsPipeConfiguration configuration;
    configuration.init_enabled = true;
    configuration.remove_unused_entities = false;
    configuration.discovery_trigger = core::DiscoveryTrigger::WRITER;

    core::DdsPipe ddspipe(
        configuration,
        discovery_database,
        std::make_shared<core::FastPayloadPool>(),
        participants_database,
        std::make_shared<utils::SlotThreadPool>(test::N_THREADS));

    core::types::Endpoint first_endpoint = core::testing::random_endpoint(1);
    first_endpoint.kind = core::types::EndpointKind::writer;
    first_endpoint.active = true;
    first_endpoint.topic = topic;
    first_endpoint.discoverer_participant_id = part_1_id;

    first_endpoint.specific_qos.partitions.push_back("partition_a");

    discovery_database->add_endpoint(first_endpoint);
    utils::sleep_for(100);

    core::types::SpecificEndpointQoS writer_qos;
    ASSERT_TRUE(participants::detail::try_specific_qos_of_writer_(*discovery_database,
            first_endpoint.guid, writer_qos));
    ASSERT_EQ(writer_qos.partitions.names(), std::vector<std::string>{"partition_a"});

    core::types::Endpoint removed_endpoint = first_endpoint;
    removed_endpoint.active = false;
    discovery_database->update_endpoint(removed_endpoint);
    utils::sleep_for(100);

    writer_qos = {};
    ASSERT_FALSE(participants::detail::try_specific_qos_of_writer_(*discovery_database,
            first_endpoint.guid, writer_qos));

    core::types::Endpoint replacement_endpoint = core::testing::random_endpoint(2);
    replacement_endpoint.kind = core::types::EndpointKind::writer;
    replacement_endpoint.active = true;
    replacement_endpoint.topic = topic;
    replacement_endpoint.discoverer_participant_id = part_1_id;

    replacement_endpoint.specific_qos.partitions.push_back("partition_b");

    discovery_database->add_endpoint(replacement_endpoint);
    utils::sleep_for(100);

    writer_qos = {};
    ASSERT_TRUE(participants::detail::try_specific_qos_of_writer_(*discovery_database,
            replacement_endpoint.guid, writer_qos));
    EXPECT_EQ(writer_qos.partitions.names(), std::vector<std::string>{"partition_b"});
}

/**
 * Test that applying a partition filter to a disabled pipe does not enable its Readers, and that
 * data arriving while disabled is still forwarded once the pipe is enabled again.
 *
 * Applying the filter by enabling the Reader directly used to leave it active behind a disabled
 * Track. The following Track::enable() then found the Reader already enabled and skipped its
 * enable_nts_(), so the data queued in between was never notified and never forwarded.
 */
TEST(DdsPipeCommunicationMockTest, mock_communication_partition_filter_while_disabled)
{
    core::types::DdsTopic topic;
    topic.m_topic_name = "topic_filter_while_disabled";
    topic.type_name = "type_filter_while_disabled";
    topic.m_internal_type_discriminator = participants::testing::INTERNAL_TOPIC_TYPE_MOCK_TEST;

    core::types::ParticipantId part_1_id("Participant_1");
    auto part_1 = std::make_shared<participants::testing::MockParticipant>(part_1_id);

    core::types::ParticipantId part_2_id("Participant_2");
    auto part_2 = std::make_shared<participants::testing::MockParticipant>(part_2_id);

    auto part_db = std::make_shared<core::ParticipantsDatabase>();
    part_db->add_participant(part_1_id, part_1);
    part_db->add_participant(part_2_id, part_2);

    auto disc_db = std::make_shared<core::DiscoveryDatabase>();

    core::DdsPipeConfiguration ddspipe_configuration;
    ddspipe_configuration.init_enabled = true;

    core::DdsPipe ddspipe(
        ddspipe_configuration,
        disc_db,
        std::make_shared<core::FastPayloadPool>(),
        part_db,
        std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS)
        );

    core::types::Endpoint endpoint = core::testing::random_endpoint();
    endpoint.kind = core::types::EndpointKind::reader;
    endpoint.topic = topic;
    disc_db->add_endpoint(endpoint);

    // Wait for entities to be created
    utils::sleep_for(100);

    auto reader_1 = part_1->get_reader(topic);
    auto writer_2 = part_2->get_writer(topic);
    ASSERT_NE(reader_1, nullptr);
    ASSERT_NE(writer_2, nullptr);

    // Disable the pipe, and with it every Track and every Reader
    ddspipe.disable();
    utils::sleep_for(100);

    // Applying the filter must reach the Reader, but must not enable it
    ddspipe.set_partition_filter({"partition"});
    utils::sleep_for(100);

    ASSERT_EQ(reader_1->n_partition_updates(), 1u);

    // Data arriving while the pipe is disabled must be queued, not forwarded
    reader_1->simulate_data_reception(test::new_data(part_1_id, 0));
    utils::sleep_for(100);

    ASSERT_EQ(writer_2->n_to_send_data(), 0u);

    // Enabling the pipe again must deliver the data queued while it was disabled.
    //
    // NOTE: polled with a bound instead of MockWriter::wait_data(), which waits forever. A
    // regression here means the data is never forwarded, and that must fail rather than hang.
    ddspipe.enable();

    for (unsigned int i = 0; i < 50 && writer_2->n_to_send_data() == 0u; i++)
    {
        utils::sleep_for(20);
    }

    ASSERT_EQ(writer_2->n_to_send_data(), 1u);
    EXPECT_EQ(writer_2->wait_data(), test::new_data(part_1_id, 0));
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
