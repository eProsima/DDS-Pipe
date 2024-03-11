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
#include <ddspipe_core/monitoring/producers/StatusMonitorProducer.hpp>
#include <ddspipe_core/types/dds/DomainId.hpp>

#if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)
    #include <ddspipe_core/types/monitoring/status/v1/MonitoringStatus.h>
    #include <ddspipe_core/types/monitoring/status/v1/MonitoringStatusPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/status/v1/MonitoringStatusTypeObject.h>
#else
    #include <ddspipe_core/types/monitoring/status/v2/MonitoringStatus.h>
    #include <ddspipe_core/types/monitoring/status/v2/MonitoringStatusPubSubTypes.h>
    #include <ddspipe_core/types/monitoring/status/v2/MonitoringStatusTypeObject.h>
#endif // if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)

using namespace eprosima;
using namespace eprosima::fastdds::dds;


const int PERIOD = 100;


class DdsMonitorStatusTest : public testing::Test
{
public:

    void SetUp() override
    {
        // Create the participant
        DomainParticipantQos pqos;
        pqos.name("MonitorStatusTestParticipant");

        participant_ = DomainParticipantFactory::get_instance()->create_participant(DOMAIN, pqos);

        ASSERT_NE(participant_, nullptr);

        // Register the type
        // TODO(tempate): write another test in which the type is discovered.
        TypeSupport type(new MonitoringStatusPubSubType());
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
        configuration.producers["status"].enabled = true;
        configuration.producers["status"].period = PERIOD;
        configuration.consumers["status"].domain = DOMAIN;
        configuration.consumers["status"].topic_name = TOPIC_NAME;

        utils::Formatter error_msg;
        ASSERT_TRUE(configuration.is_valid(error_msg));

        static ddspipe::core::Monitor monitor(configuration);

        if (configuration.producers["status"].enabled)
        {
            monitor.monitorize_status();
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
    const std::string TOPIC_NAME{"DdsMonitoringStatusTopic"};

    DomainParticipant* participant_ = nullptr;
    DataReader* reader_ = nullptr;
};

/**
 * Test that the Monitor monitors the type mismatch correctly.
 *
 * CASES:
 * - check that the Monitor publishes the type_mismatch correctly.
 */
TEST_F(DdsMonitorStatusTest, type_mismatch)
{
    // Mock a type mismatch
    monitor_error("TYPE_MISMATCH");

    MonitoringStatus status;
    SampleInfo info;

    // Wait for the monitor to publish the next message
    ASSERT_TRUE(reader_->wait_for_unread_message(fastrtps::Duration_t(PERIOD*3)));

    ASSERT_EQ(reader_->take_next_sample(&status, &info), ReturnCode_t::RETCODE_OK);
    ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

    // Verify that the content of the MonitoringStatus published by the Monitor is correct
    ASSERT_TRUE(status.error_status().type_mismatch());
    ASSERT_FALSE(status.error_status().qos_mismatch());
    ASSERT_TRUE(status.has_errors());
}

/**
 * Test that the Monitor monitors the qos mismatch correctly.
 *
 * CASES:
 * - check that the Monitor publishes the qos_mismatch correctly.
 */
TEST_F(DdsMonitorStatusTest, qos_mismatch)
{
    // Mock a qos mismatch
    monitor_error("QOS_MISMATCH");

    MonitoringStatus status;
    SampleInfo info;

    // Wait for the monitor to publish the next message
    ASSERT_TRUE(reader_->wait_for_unread_message(fastrtps::Duration_t(PERIOD*3)));

    ASSERT_EQ(reader_->take_next_sample(&status, &info), ReturnCode_t::RETCODE_OK);
    ASSERT_EQ(info.instance_state, ALIVE_INSTANCE_STATE);

    // Verify that the content of the MonitoringStatus published by the Monitor is correct
    ASSERT_FALSE(status.error_status().type_mismatch());
    ASSERT_TRUE(status.error_status().qos_mismatch());
    ASSERT_TRUE(status.has_errors());
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
