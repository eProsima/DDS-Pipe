// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <chrono>
#include <thread>

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>

#include <ddspipe_core/configuration/MonitorConfiguration.hpp>
#include <ddspipe_core/monitoring/Monitor.hpp>
#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>

#include "../../constants.hpp"

using namespace eprosima;
using namespace eprosima::fastdds::dds;



class StdoutMonitorTopicsTest : public testing::Test
{
public:

    void SetUp() override
    {
        // Initialize the Monitor
        ddspipe::core::MonitorConfiguration configuration;
        configuration.producers["topics"].enabled = true;
        configuration.producers["topics"].period = test::monitor::PERIOD_MS;

        utils::Formatter error_msg;
        ASSERT_TRUE(configuration.is_valid(error_msg));

        monitor_ = std::make_unique<ddspipe::core::Monitor>(configuration);

        if (configuration.producers["topics"].enabled)
        {
            monitor_->monitor_topics();
        }

        // Initialize the Topic
        topic_.m_topic_name = test::monitor::MOCK_TOPIC_NAME;
        topic_.type_name = test::monitor::MOCK_TYPE_NAME;

        // Initialize the Participant ID
        participant_id_ = test::monitor::MOCK_PARTICIPANT_ID;
    }

    void TearDown() override
    {
        monitor_.reset(nullptr);
    }

protected:

    std::unique_ptr<ddspipe::core::Monitor> monitor_{nullptr};

    ddspipe::core::types::DdsTopic topic_;
    ddspipe::core::types::ParticipantId participant_id_;
};

/**
 * Test that the Monitor monitors the messages received correctly.
 *
 * CASES:
 * - check that the Monitor prints the msgs_received correctly.
 */
TEST_F(StdoutMonitorTopicsTest, msgs_received)
{
    // Mock a message received
    monitor_msg_rx(topic_, participant_id_);

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS + 1));

    ASSERT_EQ(testing::internal::GetCapturedStdout(),
            "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
            "false, Type Mismatch: false, QoS Mismatch: false, Data: [Participant ID: MonitoredParticipant, "
            "Messages Received: 1, Messages Lost: 0, Message Reception Rate: 10; ]; ]\n");
}

/**
 * Test that the Monitor monitors the messages lost correctly.
 *
 * CASES:
 * - check that the Monitor prints the msgs_lost correctly.
 */
TEST_F(StdoutMonitorTopicsTest, msgs_lost)
{
    // Mock a message lost
    monitor_msg_lost(topic_, participant_id_);

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS + 1));

    ASSERT_EQ(testing::internal::GetCapturedStdout(),
            "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
            "false, Type Mismatch: false, QoS Mismatch: false, Data: [Participant ID: MonitoredParticipant, "
            "Messages Received: 0, Messages Lost: 1, Message Reception Rate: 0; ]; ]\n");
}

/**
 * Test that the Monitor monitors the type discovered correctly.
 *
 * CASES:
 * - check that the Monitor prints the type_discovered correctly.
 */
TEST_F(StdoutMonitorTopicsTest, type_discovered)
{
    // Mock a message lost to register the topic's name in the Monitor
    monitor_msg_lost(topic_, participant_id_);

    // Mock a type discovered
    monitor_type_discovered(topic_.type_name);

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS + 1));

    ASSERT_EQ(testing::internal::GetCapturedStdout(),
            "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
            "true, Type Mismatch: false, QoS Mismatch: false, Data: [Participant ID: MonitoredParticipant, "
            "Messages Received: 0, Messages Lost: 1, Message Reception Rate: 0; ]; ]\n");
}

/**
 * Test that the Monitor monitors the type mismatch correctly.
 *
 * CASES:
 * - check that the Monitor prints the type_mismatch correctly.
 */
TEST_F(StdoutMonitorTopicsTest, type_mismatch)
{
    // Mock a type mismatch
    monitor_type_mismatch(topic_);

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS + 1));

    ASSERT_EQ(testing::internal::GetCapturedStdout(),
            "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
            "false, Type Mismatch: true, QoS Mismatch: false, Data: []; ]\n");
}

/**
 * Test that the Monitor monitors the QoS mismatch correctly.
 *
 * CASES:
 * - check that the Monitor prints the qos_mismatch correctly.
 */
TEST_F(StdoutMonitorTopicsTest, qos_mismatch)
{
    // Mock a QoS mismatch
    monitor_qos_mismatch(topic_);

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS + 1));

    ASSERT_EQ(testing::internal::GetCapturedStdout(),
            "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
            "false, Type Mismatch: false, QoS Mismatch: true, Data: []; ]\n");
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
