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

#include <ddspipe_core/configuration/MonitorConfiguration.hpp>
#include <ddspipe_core/monitoring/Monitor.hpp>
#include <ddspipe_core/monitoring/consumers/DdsMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/consumers/DdsMonitorParticipantRegistry.hpp>
#include <ddspipe_core/monitoring/consumers/StdoutMonitorConsumer.hpp>
#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>
#include <ddspipe_core/types/dds/DomainId.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>

#if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopicsPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopicsTypeObject.h>
#else
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopicsPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopicsTypeObject.h>
#endif // if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)

using namespace eprosima;
using namespace eprosima::fastdds::dds;


const int PERIOD = 200;


class MonitorTopicsTest : public testing::Test
{
public:

    void SetUp() override
    {
        // Create the participant
        DomainParticipantQos pqos;
        pqos.name("MonitorTopicsTestParticipant");

        participant_ = DomainParticipantFactory::get_instance()->create_participant(DOMAIN, pqos);

        ASSERT_NE(participant_, nullptr);

        // Register the type
        // TODO(tempate): write another test in which the type is discovered.
        TypeSupport type(new MonitoringTopicsPubSubType());
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

        // Initialize the Monitor
        ddspipe::core::MonitorConfiguration configuration;
        configuration.producers["topics"].enabled = true;
        configuration.producers["topics"].period = PERIOD;
        configuration.consumers["topics"].domain = DOMAIN;
        configuration.consumers["topics"].topic_name = TOPIC_NAME;

        utils::Formatter error_msg;
        ASSERT_TRUE(configuration.is_valid(error_msg));

        if (configuration.producers["topics"].enabled)
        {
            // Register the Topics Monitor Producer
            auto topics_producer = ddspipe::core::TopicsMonitorProducer::get_instance();
            topics_producer->init(configuration.producers["topics"]);

            // Register the consumers
            topics_producer->register_consumer(std::make_unique<ddspipe::core::StdoutMonitorConsumer<MonitoringTopics>>());
            topics_producer->register_consumer(std::make_unique<ddspipe::core::DdsMonitorConsumer<MonitoringTopics>>(
                    configuration.consumers["topics"], registry_, type));

            monitor_.register_producer(topics_producer);

            // Wait for the publisher and the subscriber to match
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
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
    const std::string TOPIC_NAME{"DdsMonitoringTopicsTopic"};

    DomainParticipant* participant_ = nullptr;
    DataReader* reader_ = nullptr;

    ddspipe::core::Monitor monitor_;
    ddspipe::core::DdsMonitorParticipantRegistry registry_;
};

/**
 * Test that the Monitor monitors the messages received correctly.
 *
 * CASES:
 * - check that the Monitor prints the msgs_received correctly.
 * - check that the Monitor publishes the msgs_received correctly.
 */
TEST_F(MonitorTopicsTest, msgs_received)
{
    MonitoringTopics topics;
    SampleInfo info;

    // Mock a topic and a participant_id to monitor
    ddspipe::core::types::DdsTopic topic;
    topic.m_topic_name = "MonitoredTopic";
    topic.type_name = "MonitoredTopicType";

    ddspipe::core::types::ParticipantId participant_id = "MonitoredParticipant";

    // Mock a message received
    monitor_msg_rx(topic, participant_id);

    // StdoutMonitorConsumer
    {
        testing::internal::CaptureStdout();

        // Wait for the monitor to publish the message
        std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));

        ASSERT_EQ(testing::internal::GetCapturedStdout(),
                "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
                "false, Type Mismatch: false, QoS Mismatch: false, Data: [Participant ID: MonitoredParticipant, "
                "Messages Received: 1, Frequency: 5, Messages Lost: 0; ]; ]\n");
    }

    // DdsMonitorConsumer
    {
        // Loop to wait for the monitored message to be received
        for (int i = 0; i < 3; i++)
        {
            // Verify that the Monitor is publishing MonitoringTopics in the topic
            ASSERT_EQ(reader_->take_next_sample(&topics, &info), ReturnCode_t::RETCODE_OK);
            ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

            if (topics.topics().size() == 1)
            {
                break;
            }
        }

        // Verify that the content of the MonitoringTopics published by the Monitor is correct
        ASSERT_EQ(topics.topics().size(), 1);
        ASSERT_FALSE(topics.topics()[0].type_discovered());
        ASSERT_FALSE(topics.topics()[0].type_mismatch());
        ASSERT_FALSE(topics.topics()[0].qos_mismatch());
        ASSERT_EQ(topics.topics()[0].data().size(), 1);
        ASSERT_EQ(topics.topics()[0].data()[0].msgs_received(), 1);
        ASSERT_EQ(topics.topics()[0].data()[0].msgs_lost(), 0);
        ASSERT_EQ(topics.topics()[0].data()[0].frequency(), 1.0 / ((double) PERIOD / 1000));
    }
}

/**
 * Test that the Monitor monitors the messages lost correctly.
 *
 * CASES:
 * - check that the Monitor prints the msgs_lost correctly.
 * - check that the Monitor publishes the msgs_lost correctly.
 */
TEST_F(MonitorTopicsTest, msgs_lost)
{
    MonitoringTopics topics;
    SampleInfo info;

    // Mock a topic and a participant_id to monitor
    ddspipe::core::types::DdsTopic topic;
    topic.m_topic_name = "MonitoredTopic";
    topic.type_name = "MonitoredTopicType";

    ddspipe::core::types::ParticipantId participant_id = "MonitoredParticipant";

    // Mock a message received
    monitor_msg_lost(topic, participant_id);

    // StdoutMonitorConsumer
    {
        testing::internal::CaptureStdout();

        // Wait for the monitor to publish the message
        std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));

        ASSERT_EQ(testing::internal::GetCapturedStdout(),
                "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
                "false, Type Mismatch: false, QoS Mismatch: false, Data: [Participant ID: MonitoredParticipant, "
                "Messages Received: 0, Frequency: 0, Messages Lost: 1; ]; ]\n");
    }

    // DdsMonitorConsumer
    {
        // Loop to wait for the monitored message to be received
        for (int i = 0; i < 3; i++)
        {
            // Verify that the Monitor is publishing MonitoringTopics in the topic
            ASSERT_EQ(reader_->take_next_sample(&topics, &info), ReturnCode_t::RETCODE_OK);
            ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

            if (topics.topics().size() == 1)
            {
                break;
            }
        }

        // Verify that the content of the MonitoringTopics published by the Monitor is correct
        ASSERT_EQ(topics.topics().size(), 1);
        ASSERT_FALSE(topics.topics()[0].type_discovered());
        ASSERT_FALSE(topics.topics()[0].type_mismatch());
        ASSERT_FALSE(topics.topics()[0].qos_mismatch());
        ASSERT_EQ(topics.topics()[0].data().size(), 1);
        ASSERT_EQ(topics.topics()[0].data()[0].msgs_received(), 0);
        ASSERT_EQ(topics.topics()[0].data()[0].msgs_lost(), 1);
        ASSERT_EQ(topics.topics()[0].data()[0].frequency(), 0);
    }
}

/**
 * Test that the Monitor monitors the type discovered correctly.
 *
 * CASES:
 * - check that the Monitor prints the type_discovered correctly.
 * - check that the Monitor publishes the type_discovered correctly.
 */
TEST_F(MonitorTopicsTest, type_discovered)
{
    MonitoringTopics topics;
    SampleInfo info;

    // Mock a topic and a participant_id to monitor
    ddspipe::core::types::DdsTopic topic;
    topic.m_topic_name = "MonitoredTopic";
    topic.type_name = "MonitoredTopicType";

    ddspipe::core::types::ParticipantId participant_id = "MonitoredParticipant";

    // Mock a message lost to register the topic's name in the Monitor
    monitor_msg_lost(topic, participant_id);

    // Mock a type discovered
    monitor_type_discovered(topic.type_name);

    // StdoutMonitorConsumer
    {
        testing::internal::CaptureStdout();

        // Wait for the monitor to publish the message
        std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));

        ASSERT_EQ(testing::internal::GetCapturedStdout(),
                "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
                "true, Type Mismatch: false, QoS Mismatch: false, Data: [Participant ID: MonitoredParticipant, "
                "Messages Received: 0, Frequency: 0, Messages Lost: 1; ]; ]\n");
    }

    // DdsMonitorConsumer
    {
        // Loop to wait for the monitored message to be received
        for (int i = 0; i < 3; i++)
        {
            // Verify that the Monitor is publishing MonitoringTopics in the topic
            ASSERT_EQ(reader_->take_next_sample(&topics, &info), ReturnCode_t::RETCODE_OK);
            ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

            if (topics.topics().size() == 1)
            {
                break;
            }
        }

        // Verify that the content of the MonitoringTopics published by the Monitor is correct
        ASSERT_EQ(topics.topics().size(), 1);
        ASSERT_TRUE(topics.topics()[0].type_discovered());
        ASSERT_FALSE(topics.topics()[0].type_mismatch());
        ASSERT_FALSE(topics.topics()[0].qos_mismatch());
        ASSERT_EQ(topics.topics()[0].data().size(), 1);
        ASSERT_EQ(topics.topics()[0].data()[0].msgs_received(), 0);
        ASSERT_EQ(topics.topics()[0].data()[0].msgs_lost(), 1);
        ASSERT_EQ(topics.topics()[0].data()[0].frequency(), 0);
    }
}

/**
 * Test that the Monitor monitors the type mismatch correctly.
 *
 * CASES:
 * - check that the Monitor prints the type_mismatch correctly.
 * - check that the Monitor publishes the type_mismatch correctly.
 */
TEST_F(MonitorTopicsTest, type_mismatch)
{
    MonitoringTopics topics;
    SampleInfo info;

    // Mock a topic and a participant_id to monitor
    ddspipe::core::types::DdsTopic topic;
    topic.m_topic_name = "MonitoredTopic";
    topic.type_name = "MonitoredTopicType";

    // Mock a type mismatch
    monitor_type_mismatch(topic);

    // StdoutMonitorConsumer
    {
        testing::internal::CaptureStdout();

        // Wait for the monitor to publish the message
        std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));

        ASSERT_EQ(testing::internal::GetCapturedStdout(),
                "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
                "false, Type Mismatch: true, QoS Mismatch: false, Data: []; ]\n");
    }

    // DdsMonitorConsumer
    {
        // Loop to wait for the monitored message to be received
        for (int i = 0; i < 3; i++)
        {
            // Verify that the Monitor is publishing MonitoringTopics in the topic
            ASSERT_EQ(reader_->take_next_sample(&topics, &info), ReturnCode_t::RETCODE_OK);
            ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

            if (topics.topics().size() == 1)
            {
                break;
            }
        }

        // Verify that the content of the MonitoringTopics published by the Monitor is correct
        ASSERT_EQ(topics.topics().size(), 1);
        ASSERT_FALSE(topics.topics()[0].type_discovered());
        ASSERT_TRUE(topics.topics()[0].type_mismatch());
        ASSERT_FALSE(topics.topics()[0].qos_mismatch());
        ASSERT_EQ(topics.topics()[0].data().size(), 0);
    }
}

/**
 * Test that the Monitor monitors the QoS mismatch correctly.
 *
 * CASES:
 * - check that the Monitor prints the qos_mismatch correctly.
 * - check that the Monitor publishes the qos_mismatch correctly.
 */
TEST_F(MonitorTopicsTest, qos_mismatch)
{
    MonitoringTopics topics;
    SampleInfo info;

    // Mock a topic and a participant_id to monitor
    ddspipe::core::types::DdsTopic topic;
    topic.m_topic_name = "MonitoredTopic";
    topic.type_name = "MonitoredTopicType";

    // Mock a QoS mismatch
    monitor_qos_mismatch(topic);

    // StdoutMonitorConsumer
    {
        testing::internal::CaptureStdout();

        // Wait for the monitor to publish the message
        std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));

        ASSERT_EQ(testing::internal::GetCapturedStdout(),
                "Monitoring Topics: [Topic Name: MonitoredTopic, Type Name: MonitoredTopicType, Type Discovered: "
                "false, Type Mismatch: false, QoS Mismatch: true, Data: []; ]\n");
    }

    // DdsMonitorConsumer
    {
        // Loop to wait for the monitored message to be received
        for (int i = 0; i < 3; i++)
        {
            // Verify that the Monitor is publishing MonitoringTopics in the topic
            ASSERT_EQ(reader_->take_next_sample(&topics, &info), ReturnCode_t::RETCODE_OK);
            ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

            if (topics.topics().size() == 1)
            {
                break;
            }
        }

        // Verify that the content of the MonitoringTopics published by the Monitor is correct
        ASSERT_EQ(topics.topics().size(), 1);
        ASSERT_FALSE(topics.topics()[0].type_discovered());
        ASSERT_FALSE(topics.topics()[0].type_mismatch());
        ASSERT_TRUE(topics.topics()[0].qos_mismatch());
        ASSERT_EQ(topics.topics()[0].data().size(), 0);
    }
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
