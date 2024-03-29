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
#include <ddspipe_core/monitoring/producers/StatusMonitorProducer.hpp>

#include "../../constants.hpp"

using namespace eprosima;
using namespace eprosima::fastdds::dds;


class LogMonitorStatusTest : public testing::Test
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
        ddspipe::core::MonitorConfiguration monitor_conf;
        monitor_conf.producers[ddspipe::core::STATUS_MONITOR_PRODUCER_ID].enabled = true;
        monitor_conf.producers[ddspipe::core::STATUS_MONITOR_PRODUCER_ID].period = test::monitor::PERIOD_MS;

        utils::Formatter error_msg;
        ASSERT_TRUE(monitor_conf.is_valid(error_msg));

        monitor_ = std::make_unique<ddspipe::core::Monitor>(monitor_conf);

        if (monitor_conf.producers[ddspipe::core::STATUS_MONITOR_PRODUCER_ID].enabled)
        {
            monitor_->monitor_status();
        }
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
};

/**
 * Test that the Monitor monitors the type mismatch correctly.
 *
 * CASES:
 * - check that the Monitor logs the type_mismatch correctly.
 */
TEST_F(LogMonitorStatusTest, type_mismatch)
{
    // Mock a type mismatch
    monitor_error("TYPE_MISMATCH");

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS*3));
    utils::Log::Flush();

    ASSERT_TRUE(contains_(testing::internal::GetCapturedStdout(), "Monitoring Status: [TYPE_MISMATCH]"));
}

/**
 * Test that the Monitor monitors the QoS mismatch correctly.
 *
 * CASES:
 * - check that the Monitor logs the qos_mismatch correctly.
 */
TEST_F(LogMonitorStatusTest, qos_mismatch)
{
    // Mock a qos mismatch
    monitor_error("QOS_MISMATCH");

    testing::internal::CaptureStdout();

    // Wait for the monitor to print the message
    std::this_thread::sleep_for(std::chrono::milliseconds(test::monitor::PERIOD_MS*3));
    utils::Log::Flush();

    ASSERT_TRUE(contains_(testing::internal::GetCapturedStdout(), "Monitoring Status: [QOS_MISMATCH]"));
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
