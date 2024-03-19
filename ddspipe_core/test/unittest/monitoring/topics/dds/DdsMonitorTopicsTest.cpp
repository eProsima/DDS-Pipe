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
#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>
#include <ddspipe_core/types/dds/DomainId.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>

#if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v1/MonitoringTopicsPubSubTypes.h>
#else
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopics.h>
    #include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopicsPubSubTypes.h>
#endif // if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)

#include "../../constants.hpp"

using namespace eprosima;
using namespace eprosima::fastdds::dds;


class DdsMonitorTopicsTest : public testing::Test
{
public:

    void SetUp() override
    {
        // Initialize the Monitor
        ddspipe::core::MonitorConfiguration configuration;
        configuration.producers[ddspipe::core::TOPICS_MONITOR_PRODUCER_ID].enabled = true;
        configuration.producers[ddspipe::core::TOPICS_MONITOR_PRODUCER_ID].period = test::monitor::PERIOD_MS;
        configuration.consumers[ddspipe::core::TOPICS_MONITOR_PRODUCER_ID].domain = test::monitor::DOMAIN;
        configuration.consumers[ddspipe::core::TOPICS_MONITOR_PRODUCER_ID].topic_name = test::monitor::TOPIC_NAME;

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

        // Create the participant
        DomainParticipantQos pqos;
        pqos.name(test::monitor::PARTICIPANT_ID);

        participant_ = DomainParticipantFactory::get_instance()->create_participant(test::monitor::DOMAIN, pqos);

        ASSERT_NE(participant_, nullptr);

        // Register the type
        // TODO(tempate): write another test in which the type is discovered.
        TypeSupport type(new MonitoringTopicsPubSubType());
        type.register_type(participant_);

        // Create the subscriber
        Subscriber* subscriber = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);

        ASSERT_NE(subscriber, nullptr);

        // Create the topic
        Topic* topic = participant_->create_topic(test::monitor::TOPIC_NAME, type.get_type_name(), TOPIC_QOS_DEFAULT);

        ASSERT_NE(topic, nullptr);

        // Create the reader
        reader_ = subscriber->create_datareader(topic, DATAREADER_QOS_DEFAULT);

        ASSERT_NE(reader_, nullptr);
    }

    void TearDown() override
    {
        monitor_.reset(nullptr);

        if (nullptr != participant_)
        {
            participant_->delete_contained_entities();
            DomainParticipantFactory::get_instance()->delete_participant(participant_);
        }
    }

protected:

    std::unique_ptr<ddspipe::core::Monitor> monitor_{nullptr};

    ddspipe::core::types::DdsTopic topic_;
    ddspipe::core::types::ParticipantId participant_id_;

    DomainParticipant* participant_ = nullptr;
    DataReader* reader_ = nullptr;
};

/**
 * Test that the Monitor monitors the messages received correctly.
 *
 * CASES:
 * - check that the Monitor publishes the msgs_received correctly.
 */
TEST_F(DdsMonitorTopicsTest, msgs_received)
{
    // Mock a message received
    monitor_msg_rx(topic_, participant_id_);

    MonitoringTopics topics;
    SampleInfo info;

    // Wait for the monitor to publish the next message
    ASSERT_TRUE(reader_->wait_for_unread_message(test::monitor::MAX_WAITING_TIME));

    ASSERT_EQ(reader_->take_next_sample(&topics, &info), ReturnCode_t::RETCODE_OK);
    ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

    ASSERT_EQ(topics.topics().size(), 1);
    ASSERT_EQ(topics.topics()[0].name(), topic_.m_topic_name);
    ASSERT_EQ(topics.topics()[0].type_name(), topic_.type_name);
    ASSERT_FALSE(topics.topics()[0].type_discovered());
    ASSERT_FALSE(topics.topics()[0].type_mismatch());
    ASSERT_FALSE(topics.topics()[0].qos_mismatch());
    ASSERT_EQ(topics.topics()[0].data()[0].msgs_received(), 1);
    ASSERT_EQ(topics.topics()[0].data().size(), 1);
    ASSERT_EQ(topics.topics()[0].data()[0].msgs_lost(), 0);
    ASSERT_EQ(topics.topics()[0].data()[0].msg_rx_rate(), 1.0 / (test::monitor::PERIOD_SECS));
}

/**
 * Test that the Monitor monitors the messages lost correctly.
 *
 * CASES:
 * - check that the Monitor publishes the msgs_lost correctly.
 */
TEST_F(DdsMonitorTopicsTest, msgs_lost)
{
    // Mock a message received
    monitor_msg_lost(topic_, participant_id_);

    MonitoringTopics topics;
    SampleInfo info;

    // Wait for the monitor to publish the next message
    ASSERT_TRUE(reader_->wait_for_unread_message(test::monitor::MAX_WAITING_TIME));

    ASSERT_EQ(reader_->take_next_sample(&topics, &info), ReturnCode_t::RETCODE_OK);
    ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

    // Verify that the content of the MonitoringTopics published by the Monitor is correct
    ASSERT_EQ(topics.topics().size(), 1);
    ASSERT_EQ(topics.topics()[0].name(), topic_.m_topic_name);
    ASSERT_EQ(topics.topics()[0].type_name(), topic_.type_name);
    ASSERT_FALSE(topics.topics()[0].type_discovered());
    ASSERT_FALSE(topics.topics()[0].type_mismatch());
    ASSERT_FALSE(topics.topics()[0].qos_mismatch());
    ASSERT_EQ(topics.topics()[0].data().size(), 1);
    ASSERT_EQ(topics.topics()[0].data()[0].msgs_received(), 0);
    ASSERT_EQ(topics.topics()[0].data()[0].msgs_lost(), 1);
    ASSERT_EQ(topics.topics()[0].data()[0].msg_rx_rate(), 0);
}

/**
 * Test that the Monitor monitors the type discovered correctly.
 *
 * CASES:
 * - check that the Monitor publishes the type_discovered correctly.
 */
TEST_F(DdsMonitorTopicsTest, type_discovered)
{
    // Mock a message lost to register the topic's name in the Monitor
    monitor_msg_lost(topic_, participant_id_);

    // Mock a type discovered
    monitor_type_discovered(topic_.type_name);

    MonitoringTopics topics;
    SampleInfo info;

    // Wait for the monitor to publish the next message
    ASSERT_TRUE(reader_->wait_for_unread_message(test::monitor::MAX_WAITING_TIME));

    ASSERT_EQ(reader_->take_next_sample(&topics, &info), ReturnCode_t::RETCODE_OK);
    ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

    // Verify that the content of the MonitoringTopics published by the Monitor is correct
    ASSERT_EQ(topics.topics().size(), 1);
    ASSERT_EQ(topics.topics()[0].name(), topic_.m_topic_name);
    ASSERT_EQ(topics.topics()[0].type_name(), topic_.type_name);
    ASSERT_TRUE(topics.topics()[0].type_discovered());
    ASSERT_FALSE(topics.topics()[0].type_mismatch());
    ASSERT_FALSE(topics.topics()[0].qos_mismatch());
    ASSERT_EQ(topics.topics()[0].data().size(), 1);
    ASSERT_EQ(topics.topics()[0].data()[0].msgs_received(), 0);
    ASSERT_EQ(topics.topics()[0].data()[0].msgs_lost(), 1);
    ASSERT_EQ(topics.topics()[0].data()[0].msg_rx_rate(), 0);
}

/**
 * Test that the Monitor monitors the type mismatch correctly.
 *
 * CASES:
 * - check that the Monitor publishes the type_mismatch correctly.
 */
TEST_F(DdsMonitorTopicsTest, type_mismatch)
{
    // Mock a type mismatch
    monitor_type_mismatch(topic_);

    MonitoringTopics topics;
    SampleInfo info;

    // Wait for the monitor to publish the next message
    ASSERT_TRUE(reader_->wait_for_unread_message(test::monitor::MAX_WAITING_TIME));

    ASSERT_EQ(reader_->take_next_sample(&topics, &info), ReturnCode_t::RETCODE_OK);
    ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

    // Verify that the content of the MonitoringTopics published by the Monitor is correct
    ASSERT_EQ(topics.topics().size(), 1);
    ASSERT_EQ(topics.topics()[0].name(), topic_.m_topic_name);
    ASSERT_EQ(topics.topics()[0].type_name(), topic_.type_name);
    ASSERT_FALSE(topics.topics()[0].type_discovered());
    ASSERT_TRUE(topics.topics()[0].type_mismatch());
    ASSERT_FALSE(topics.topics()[0].qos_mismatch());
    ASSERT_EQ(topics.topics()[0].data().size(), 0);
}

/**
 * Test that the Monitor monitors the QoS mismatch correctly.
 *
 * CASES:
 * - check that the Monitor publishes the qos_mismatch correctly.
 */
TEST_F(DdsMonitorTopicsTest, qos_mismatch)
{
    // Mock a QoS mismatch
    monitor_qos_mismatch(topic_);

    MonitoringTopics topics;
    SampleInfo info;

    // Wait for the monitor to publish the next message
    ASSERT_TRUE(reader_->wait_for_unread_message(test::monitor::MAX_WAITING_TIME));

    ASSERT_EQ(reader_->take_next_sample(&topics, &info), ReturnCode_t::RETCODE_OK);
    ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

    // Verify that the content of the MonitoringTopics published by the Monitor is correct
    ASSERT_EQ(topics.topics().size(), 1);
    ASSERT_EQ(topics.topics()[0].name(), topic_.m_topic_name);
    ASSERT_EQ(topics.topics()[0].type_name(), topic_.type_name);
    ASSERT_FALSE(topics.topics()[0].type_discovered());
    ASSERT_FALSE(topics.topics()[0].type_mismatch());
    ASSERT_TRUE(topics.topics()[0].qos_mismatch());
    ASSERT_EQ(topics.topics()[0].data().size(), 0);
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
