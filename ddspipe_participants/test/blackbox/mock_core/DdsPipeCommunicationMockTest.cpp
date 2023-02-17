// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <ddspipe_participants/testing/entities/mock_entities.hpp>

using namespace eprosima::ddspipe;

namespace test {

constexpr const unsigned int N_THREADS = 2;
constexpr const unsigned int N_MESSAGES = 5;

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
    core::DdsPipe ddspipe(
        std::make_shared<core::AllowedTopicList>(),
        std::make_shared<core::DiscoveryDatabase>(),
        std::make_shared<core::FastPayloadPool>(),
        part_db,
        std::make_shared<eprosima::utils::SlotThreadPool>(test::N_THREADS),
        {htopic_1},
        true
    );

    // Look for the reader in participant 1 and writer in participant 2
    auto reader_1 = part_1->readers[topic_1.topic_unique_name()];
    auto writer_2 = part_2->writers[topic_1.topic_unique_name()];
    ASSERT_NE(reader_1, nullptr);
    ASSERT_NE(writer_2, nullptr);

    // Simulate N messages
    for (unsigned int i=0; i<test::N_MESSAGES; i++)
    {
        reader_1->simulate_data_reception(test::new_data(part_1_id, i));
    }

    // Wait for all messages
    for (unsigned int i=0; i<test::N_MESSAGES; i++)
    {
        auto received_data = writer_2->wait_data();
        ASSERT_EQ(received_data, test::new_data(part_1_id, i));
    }
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
