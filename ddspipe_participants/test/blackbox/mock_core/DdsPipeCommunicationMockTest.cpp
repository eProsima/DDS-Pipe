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
#include <ddspipe_core/dynamic/AllowedTopicList.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>
#include <ddspipe_core/efficiency/payload/FastPayloadPool.hpp>
#include <ddspipe_core/testing/random_values.hpp>

#include <ddspipe_participants/testing/entities/mock_entities.hpp>

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

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
