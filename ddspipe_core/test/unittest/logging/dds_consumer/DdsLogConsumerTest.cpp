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

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include <cpp_utils/Log.hpp>

#include <ddspipe_core/configuration/DdsPipeLogConfiguration.hpp>
#include <ddspipe_core/logging/DdsLogConsumer.hpp>
#include <ddspipe_core/types/dds/DomainId.hpp>

#if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)
    #include <ddspipe_core/types/logging/v1/LogEntry.h>
    #include <ddspipe_core/types/logging/v1/LogEntryPubSubTypes.h>
    #include <ddspipe_core/types/logging/v1/LogEntryTypeObject.h>
#else
    #include <ddspipe_core/types/logging/v2/LogEntry.h>
    #include <ddspipe_core/types/logging/v2/LogEntryPubSubTypes.h>
    #include <ddspipe_core/types/logging/v2/LogEntryTypeObject.h>
#endif // if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)

using namespace eprosima;
using namespace eprosima::fastdds::dds;

class DdsLogConsumerTest : public testing::Test
{
public:

    void SetUp() override
    {
        // Create the participant
        DomainParticipantQos pqos;
        pqos.name("DdsLogParticipantTest");

        participant_ = DomainParticipantFactory::get_instance()->create_participant(DOMAIN, pqos);

        ASSERT_NE(participant_, nullptr);

        // Register the type
        // TODO(tempate): write another test in which the type is discovered.
        TypeSupport type(new LogEntryPubSubType());
        type.register_type(participant_);

        // Create the subscriber
        Subscriber* subscriber = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);

        ASSERT_NE(subscriber, nullptr);

        // Create the topic
        TopicQos tqos = TOPIC_QOS_DEFAULT;
        tqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
        tqos.durability().kind = VOLATILE_DURABILITY_QOS;

        Topic* topic = participant_->create_topic(TOPIC_NAME, type.get_type_name(), tqos);

        ASSERT_NE(topic, nullptr);

        // Create the reader
        DataReaderQos rqos = DATAREADER_QOS_DEFAULT;

        rqos.data_sharing().automatic();
        rqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
        rqos.durability().kind = VOLATILE_DURABILITY_QOS;

        reader_ = subscriber->create_datareader(topic, rqos);

        ASSERT_NE(reader_, nullptr);
    }

    void TearDown() override
    {
        if (nullptr != participant_)
        {
            participant_->delete_contained_entities();
            DomainParticipantFactory::get_instance()->delete_participant(participant_);
        }
    }

protected:

    const ddspipe::core::types::DomainIdType DOMAIN{84};
    const std::string TOPIC_NAME{"DdsLogsTopic"};

    DomainParticipant* participant_ = nullptr;
    DataReader* reader_ = nullptr;
};

/**
 * Test that the DdsLogConsumer publishes logs when publish::enable is set to true.
 *
 * CASES:
 * - check that the DdsLogConsumer publishes logInfos.
 * - check that the DdsLogConsumer publishes logWarnings.
 * - check that the DdsLogConsumer publishes logErrors.
 */
TEST_F(DdsLogConsumerTest, publish_logs)
{
    // Configure the Log
    ddspipe::core::DdsPipeLogConfiguration log_configuration;
    log_configuration.publish.enable = true;
    log_configuration.publish.domain = DOMAIN;
    log_configuration.publish.topic_name = TOPIC_NAME;

    // Filter out every log except ours
    log_configuration.verbosity = utils::VerbosityKind::Info;
    log_configuration.filter[utils::VerbosityKind::Info].set_value("DDSPIPE_TEST");
    log_configuration.filter[utils::VerbosityKind::Warning].set_value("DDSPIPE_TEST");
    log_configuration.filter[utils::VerbosityKind::Error].set_value("DDSPIPE_TEST");

    utils::Log::ClearConsumers();
    utils::Log::SetVerbosity(log_configuration.verbosity);

    // Register the DdsLogConsumer
    if (log_configuration.publish.enable)
    {
        utils::Log::RegisterConsumer(std::make_unique<ddspipe::core::DdsLogConsumer>(&log_configuration));
    }

    // Wait for the publisher and the subscriber to match
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    LogEntry entry;
    SampleInfo info;

    // INFO
    {
        logInfo(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        utils::Log::Flush();

        // Wait for the subscriber to receive the message
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify that the content of the LogEntry published by the Log is correct
        ASSERT_EQ(reader_->take_next_sample(&entry, &info), ReturnCode_t::RETCODE_OK);
        ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

        ASSERT_EQ(entry.event(), UNDEFINED);
        ASSERT_EQ(entry.kind(), Kind::Info);
        ASSERT_EQ(entry.category(), "DDSPIPE_TEST");
        ASSERT_EQ(entry.message(), "LOG_CONSUMER_TEST | You only live once.");
    }

    // WARNING
    {
        logWarning(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        utils::Log::Flush();

        // Wait for the subscriber to receive the message
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify that the content of the LogEntry published by the Log is correct
        ASSERT_EQ(reader_->take_next_sample(&entry, &info), ReturnCode_t::RETCODE_OK);
        ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

        ASSERT_EQ(entry.event(), UNDEFINED);
        ASSERT_EQ(entry.kind(), Kind::Warning);
        ASSERT_EQ(entry.category(), "DDSPIPE_TEST");
        ASSERT_EQ(entry.message(), "LOG_CONSUMER_TEST | You only live once.");
    }

    // ERROR
    {
        logError(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        utils::Log::Flush();

        // Wait for the subscriber to receive the message
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify that the content of the LogEntry published by the Log is correct
        ASSERT_EQ(reader_->take_next_sample(&entry, &info), ReturnCode_t::RETCODE_OK);
        ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

        ASSERT_EQ(entry.event(), UNDEFINED);
        ASSERT_EQ(entry.kind(), Kind::Error);
        ASSERT_EQ(entry.category(), "DDSPIPE_TEST");
        ASSERT_EQ(entry.message(), "LOG_CONSUMER_TEST | You only live once.");
    }

    utils::Log::ClearConsumers();
}

/**
 * Test that the DdsLogConsumer doesn't publish when publish::enable is set to false.
 *
 * CASES:
 * - check that the DdsLogConsumer doesn't publish logInfos.
 * - check that the DdsLogConsumer doesn't publish logWarnings.
 * - check that the DdsLogConsumer doesn't publish logErrors.
 */
TEST_F(DdsLogConsumerTest, dont_publish_logs)
{
    // Configure the Log
    ddspipe::core::DdsPipeLogConfiguration log_configuration;
    log_configuration.publish.enable = false;
    log_configuration.publish.domain = DOMAIN;
    log_configuration.publish.topic_name = TOPIC_NAME;

    // Filter out every log except ours
    log_configuration.verbosity = utils::VerbosityKind::Info;
    log_configuration.filter[utils::VerbosityKind::Info].set_value("DDSPIPE_TEST");
    log_configuration.filter[utils::VerbosityKind::Warning].set_value("DDSPIPE_TEST");
    log_configuration.filter[utils::VerbosityKind::Error].set_value("DDSPIPE_TEST");

    utils::Log::ClearConsumers();
    utils::Log::SetVerbosity(log_configuration.verbosity);

    // Register the DdsLogConsumer
    if (log_configuration.publish.enable)
    {
        utils::Log::RegisterConsumer(std::make_unique<ddspipe::core::DdsLogConsumer>(&log_configuration));
    }

    // Wait for the publisher and the subscriber to match
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    LogEntry entry;
    SampleInfo info;

    // INFO
    {
        logInfo(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        utils::Log::Flush();

        // Wait for the subscriber to receive the message
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify that the content of the LogEntry published by the Log is correct
        ASSERT_EQ(reader_->take_next_sample(&entry, &info), ReturnCode_t::RETCODE_NO_DATA);
    }

    // WARNING
    {
        logWarning(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        utils::Log::Flush();

        // Wait for the subscriber to receive the message
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify that the content of the LogEntry published by the Log is correct
        ASSERT_EQ(reader_->take_next_sample(&entry, &info), ReturnCode_t::RETCODE_NO_DATA);
    }

    // ERROR
    {
        logError(DDSPIPE_TEST, "LOG_CONSUMER_TEST | You only live once.");
        utils::Log::Flush();

        // Wait for the subscriber to receive the message
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify that the content of the LogEntry published by the Log is correct
        ASSERT_EQ(reader_->take_next_sample(&entry, &info), ReturnCode_t::RETCODE_NO_DATA);
    }

    utils::Log::ClearConsumers();
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
