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
#include <cpp_utils/logging/StdLogConsumer.hpp>

#include <ddspipe_core/configuration/DdsPipeLogConfiguration.hpp>

using namespace eprosima;
using namespace eprosima::fastdds::dds;

/**
 * Test that the StdLogConsumer prints logs when stdout is set to true.
 *
 * CASES:
 * - check that the StdLogConsumer prints logInfos.
 * - check that the StdLogConsumer prints logWarnings.
 * - check that the StdLogConsumer prints logErrors.
 */
TEST(StdLogConsumerTest, print_logs)
{
    // Configure the Log
    ddspipe::core::DdsPipeLogConfiguration log_configuration;
    log_configuration.stdout_enable = true;

    // Filter out every log except ours
    log_configuration.verbosity = utils::VerbosityKind::Info;
    log_configuration.filter[utils::VerbosityKind::Info].set_value("DDSPIPE_TEST");
    log_configuration.filter[utils::VerbosityKind::Warning].set_value("DDSPIPE_TEST");
    log_configuration.filter[utils::VerbosityKind::Error].set_value("DDSPIPE_TEST");

    utils::Log::ClearConsumers();
    utils::Log::SetVerbosity(log_configuration.verbosity);

    // Register the StdLogConsumer
    if (log_configuration.stdout_enable)
    {
        utils::Log::RegisterConsumer(std::make_unique<utils::StdLogConsumer>(&log_configuration));
    }

    // INFO
    {
        testing::internal::CaptureStdout();

        logInfo(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        eprosima::utils::Log::Flush();

        ASSERT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    // WARNING
    {
        testing::internal::CaptureStderr();

        logWarning(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        eprosima::utils::Log::Flush();

        ASSERT_FALSE(testing::internal::GetCapturedStderr().empty());
    }

    // ERROR
    {
        testing::internal::CaptureStderr();

        logError(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        eprosima::utils::Log::Flush();

        ASSERT_FALSE(testing::internal::GetCapturedStderr().empty());
    }
}

/**
 * Test that the StdLogConsumer doesn't print logs when stdout is set to false.
 *
 * CASES:
 * - check that the StdLogConsumer doesn't print logInfos.
 * - check that the StdLogConsumer doesn't print logWarnings.
 * - check that the StdLogConsumer doesn't print logErrors.
 */
TEST(StdLogConsumerTest, dont_print_logs)
{
    // Configure the Log
    ddspipe::core::DdsPipeLogConfiguration log_configuration;
    log_configuration.stdout_enable = false;

    // Filter out every log except ours
    log_configuration.verbosity = utils::VerbosityKind::Info;
    log_configuration.filter[utils::VerbosityKind::Info].set_value("DDSPIPE_TEST");
    log_configuration.filter[utils::VerbosityKind::Warning].set_value("DDSPIPE_TEST");
    log_configuration.filter[utils::VerbosityKind::Error].set_value("DDSPIPE_TEST");

    utils::Log::ClearConsumers();
    utils::Log::SetVerbosity(log_configuration.verbosity);

    // Register the StdLogConsumer
    if (log_configuration.stdout_enable)
    {
        utils::Log::RegisterConsumer(std::make_unique<utils::StdLogConsumer>(&log_configuration));
    }

    // INFO
    {
        testing::internal::CaptureStdout();

        logInfo(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        eprosima::utils::Log::Flush();

        ASSERT_TRUE(testing::internal::GetCapturedStdout().empty());
    }

    // WARNING
    {
        testing::internal::CaptureStderr();

        logWarning(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        eprosima::utils::Log::Flush();

        ASSERT_TRUE(testing::internal::GetCapturedStderr().empty());
    }

    // ERROR
    {
        testing::internal::CaptureStderr();

        logError(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        eprosima::utils::Log::Flush();

        ASSERT_TRUE(testing::internal::GetCapturedStderr().empty());
    }
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
