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
#include <ddspipe_core/monitoring/producers/StatusMonitorProducer.hpp>

using namespace eprosima;
using namespace eprosima::fastdds::dds;


const int PERIOD = 100;
const ddspipe::core::types::DomainIdType DOMAIN = 84;
const std::string TOPIC_NAME = "DdsMonitoringTopicsTopic";


class StdoutMonitorStatusTest : public testing::Test
{
public:

    void SetUp() override
    {
        // Initialize the Monitor
        ddspipe::core::MonitorConfiguration configuration;
        configuration.producers["status"].enabled = true;
        configuration.producers["status"].period = PERIOD;
        configuration.consumers["status"].domain = DOMAIN;
        configuration.consumers["status"].topic_name = TOPIC_NAME;

        utils::Formatter error_msg;
        ASSERT_TRUE(configuration.is_valid(error_msg));

        monitor_ = std::make_unique<ddspipe::core::Monitor>(configuration);

        if (configuration.producers["status"].enabled)
        {
            monitor_->monitorize_status();
        }
    }

    void TearDown() override
    {
        monitor_.reset(nullptr);
    }

protected:

    std::unique_ptr<ddspipe::core::Monitor> monitor_{nullptr};
};

/**
 * Test that the Monitor monitors the type mismatch correctly.
 *
 * CASES:
 * - check that the Monitor prints the type_mismatch correctly.
 */
TEST_F(StdoutMonitorStatusTest, type_mismatch)
{
    // Mock a type mismatch
    monitor_error("TYPE_MISMATCH");

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD+1));

    ASSERT_EQ(testing::internal::GetCapturedStdout(),
            "Monitoring Status: [TYPE_MISMATCH]\n");
}

/**
 * Test that the Monitor monitors the QoS mismatch correctly.
 *
 * CASES:
 * - check that the Monitor prints the qos_mismatch correctly.
 */
TEST_F(StdoutMonitorStatusTest, qos_mismatch)
{
    // Mock a qos mismatch
    monitor_error("QOS_MISMATCH");

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD+1));

    ASSERT_EQ(testing::internal::GetCapturedStdout(),
            "Monitoring Status: [QOS_MISMATCH]\n");
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
