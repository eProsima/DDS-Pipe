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

#include <cpp_utils/Log.hpp>
#include <cpp_utils/logging/BaseLogConfiguration.hpp>
#include <cpp_utils/logging/StdLogConsumer.hpp>

#include <ddspipe_core/configuration/MonitorConfiguration.hpp>
#include <ddspipe_core/monitoring/Monitor.hpp>
#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>

#include "../../constants.hpp"

using namespace eprosima;
using namespace eprosima::fastdds::dds;



class LogMonitorTopicsTest : public testing::Test
{
public:

    void SetUp() override
    {
        // Initialize the Log
        utils::Log::ClearConsumers();

        utils::BaseLogConfiguration log_conf;
        log_conf.verbosity = utils::VerbosityKind::Info;
        log_conf.filter[utils::VerbosityKind::Info].set_value("MONITOR_DATA");

        utils::Log::SetVerbosity(log_conf.verbosity);

        utils::Log::RegisterConsumer(
            std::make_unique<utils::StdLogConsumer>(&log_conf));

        // Initialize the Monitor
        ddspipe::core::MonitorConfiguration configuration;
        configuration.producers[ddspipe::core::TOPICS_MONITOR_PRODUCER_ID].enabled = true;
        configuration.producers[ddspipe::core::TOPICS_MONITOR_PRODUCER_ID].period = test::monitor::PERIOD_MS;

        utils::Formatter error_msg;
        ASSERT_TRUE(configuration.is_valid(error_msg));

        monitor_ = std::make_unique<ddspipe::core::Monitor>(configuration);

        if (configuration.producers[ddspipe::core::TOPICS_MONITOR_PRODUCER_ID].enabled)
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
        utils::Log::ClearConsumers();

        monitor_.reset(nullptr);
    }

protected:

    bool contains_(
            const std::string& str,
            const std::string& substr)
    {
        return str.find(substr) != std::string::npos;
    }

    std::unique_ptr<ddspipe::core::Monitor> monitor_{nullptr};

    ddspipe::core::types::DdsTopic topic_;
    ddspipe::core::types::ParticipantId participant_id_;
};

/**
 * Test that the Monitor monitors the messages received correctly.
 *
 * CASES:
 * - check that the Monitor logs the msgs_received correctly.
 */
TEST_F(LogMonitorTopicsTest, msgs_received)
{
    // Mock a message received
    monitor_msg_rx(topic_, participant_id_);

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS*3));
    utils::Log::Flush();

    ASSERT_TRUE(contains_(testing::internal::GetCapturedStdout(),
            "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
            "false, Type Mismatch: false, QoS Mismatch: false, Data: [Participant ID: MonitoredParticipant, "
            "Messages Received: 1, Messages Lost: 0, Message Reception Rate: 2; ]; ]"));
}

/**
 * Test that the Monitor monitors the messages lost correctly.
 *
 * CASES:
 * - check that the Monitor logs the msgs_lost correctly.
 */
TEST_F(LogMonitorTopicsTest, msgs_lost)
{
    // Mock a message lost
    monitor_msg_lost(topic_, participant_id_);

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS*3));
    utils::Log::Flush();

    ASSERT_TRUE(contains_(testing::internal::GetCapturedStdout(),
            "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
            "false, Type Mismatch: false, QoS Mismatch: false, Data: [Participant ID: MonitoredParticipant, "
            "Messages Received: 0, Messages Lost: 1, Message Reception Rate: 0; ]; ]"));
}

/**
 * Test that the Monitor monitors the type discovered correctly.
 *
 * CASES:
 * - check that the Monitor logs the type_discovered correctly.
 */
TEST_F(LogMonitorTopicsTest, type_discovered)
{
    // Mock a message lost to register the topic's name in the Monitor
    monitor_msg_lost(topic_, participant_id_);

    // Mock a type discovered
    monitor_type_discovered(topic_.type_name);

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS*3));
    utils::Log::Flush();

    ASSERT_TRUE(contains_(testing::internal::GetCapturedStdout(),
            "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
            "true, Type Mismatch: false, QoS Mismatch: false, Data: [Participant ID: MonitoredParticipant, "
            "Messages Received: 0, Messages Lost: 1, Message Reception Rate: 0; ]; ]"));
}

/**
 * Test that the Monitor monitors the type mismatch correctly.
 *
 * CASES:
 * - check that the Monitor logs the type_mismatch correctly.
 */
TEST_F(LogMonitorTopicsTest, type_mismatch)
{
    // Mock a type mismatch
    monitor_type_mismatch(topic_);

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS*3));
    utils::Log::Flush();

    ASSERT_TRUE(contains_(testing::internal::GetCapturedStdout(),
            "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
            "false, Type Mismatch: true, QoS Mismatch: false, Data: []; ]"));
}

/**
 * Test that the Monitor monitors the QoS mismatch correctly.
 *
 * CASES:
 * - check that the Monitor logs the qos_mismatch correctly.
 */
TEST_F(LogMonitorTopicsTest, qos_mismatch)
{
    // Mock a QoS mismatch
    monitor_qos_mismatch(topic_);

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS*3));
    utils::Log::Flush();

    ASSERT_TRUE(contains_(testing::internal::GetCapturedStdout(),
            "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
            "false, Type Mismatch: false, QoS Mismatch: true, Data: []; ]"));
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
