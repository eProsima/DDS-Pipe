// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <ddspipe_core/configuration/DdsPipeConfiguration.hpp>
#include <ddspipe_core/core/DdsPipe.hpp>
#include <ddspipe_core/efficiency/payload/FastPayloadPool.hpp>

#include <ddspipe_participants/participant/auxiliar/BlankParticipant.hpp>
#include <ddspipe_participants/participant/auxiliar/EchoParticipant.hpp>
#include <ddspipe_participants/participant/rtps/DiscoveryServerParticipant.hpp>
#include <ddspipe_participants/participant/rtps/SimpleParticipant.hpp>
#include <ddspipe_participants/participant/rtps/CommonParticipant.hpp>
#include <ddspipe_participants/participant/rtps/InitialPeersParticipant.hpp>
#include <ddspipe_participants/participant/dds/XmlParticipant.hpp>
#include <ddspipe_participants/testing/entities/mock_entities.hpp>
#include <ddspipe_participants/testing/random_values.hpp>
#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>
#include <ddspipe_participants/reader/dds/SimpleReader.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>
#include <ddspipe_participants/writer/dds/SimpleWriter.hpp>
#include <ddspipe_participants/xml/XmlHandler.hpp>
#include <ddspipe_participants/xml/XmlHandlerConfiguration.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <ddspipe_participants/types/dds/TopicDataType.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe;

namespace test {

class TestableWriter : public participants::dds::SimpleWriter
{
public:

    using SimpleWriter::SimpleWriter;

    fastdds::dds::DataWriter* get_dds_writer()
    {
        return writer_;
    }

};

class TestableReader : public participants::dds::SimpleReader
{
public:

    using SimpleReader::SimpleReader;

    fastdds::dds::DataReader* get_dds_reader()
    {
        return reader_;
    }

};

constexpr const unsigned int N_THREADS = 2;

} // namespace test

/**
 * Test to check default participant configuration values.
 */
TEST(ParticipantsCreationgTest, default_configuration)
{
    // Common configuration
    {
        participants::ParticipantConfiguration conf;
        EXPECT_EQ(conf.app_id, "UNKNOWN_APP");
        EXPECT_EQ(conf.app_metadata, "");
        EXPECT_TRUE(conf.id.empty());
        EXPECT_FALSE(conf.is_repeater);
    }
}

/**
 * Test to create a participant of each kind and check it does not fail.
 *
 * CASES:
 * - Blank
 * - Echo
 * - Simple
 * - Discovery Server
 * - Initial Peers
 */
TEST(ParticipantsCreationgTest, creation_trivial)
{
    // Auxiliar objects
    std::shared_ptr<core::PayloadPool> payload_pool(new core::FastPayloadPool());
    std::shared_ptr<core::DiscoveryDatabase> discovery_database(new core::DiscoveryDatabase());
    core::types::ParticipantId part_id("testPart");

    // Blank
    {
        participants::BlankParticipant participant(part_id);
    }

    // Echo
    {
        std::shared_ptr<participants::EchoParticipantConfiguration> conf(
            new participants::EchoParticipantConfiguration());
        conf->id = part_id;

        participants::EchoParticipant participant(conf, discovery_database);
    }

    // Simple
    {
        std::shared_ptr<participants::SimpleParticipantConfiguration> conf(
            new participants::SimpleParticipantConfiguration());
        conf->id = part_id;

        participants::rtps::SimpleParticipant participant(conf, payload_pool, discovery_database);
        participant.init();
    }

    // Discovery Server
    {
        std::shared_ptr<participants::InitialPeersParticipantConfiguration> conf(
            new participants::InitialPeersParticipantConfiguration());
        conf->id = part_id;
        conf->listening_addresses.insert(participants::testing::random_address());

        participants::rtps::InitialPeersParticipant participant(conf, payload_pool, discovery_database);
        participant.init();
    }

    // Initial Peers
    {
        std::shared_ptr<participants::DiscoveryServerParticipantConfiguration> conf(
            new participants::DiscoveryServerParticipantConfiguration());
        conf->id = part_id;
        conf->listening_addresses.insert(participants::testing::random_address());

        participants::rtps::DiscoveryServerParticipant participant(conf, payload_pool, discovery_database);
        participant.init();
    }

    // Xml Participant
    {
        std::shared_ptr<participants::XmlParticipantConfiguration> conf(
            new participants::XmlParticipantConfiguration());
        conf->id = part_id;

        participants::dds::XmlParticipant participant(conf, payload_pool, discovery_database);
        participant.init();
    }
}

/**
 * Test to create a participant of each kind and add them to a DDS Pipe that uses a builtin topic.
 * This will force every participant to create an endpoint.
 * This checks the endpoint creation does not fail.
 */
TEST(ParticipantsCreationgTest, ddspipe_all_creation_builtin_topic)
{
    // Auxiliar objects
    std::shared_ptr<core::DiscoveryDatabase> discovery_database(new core::DiscoveryDatabase());
    std::shared_ptr<core::PayloadPool> payload_pool(new core::FastPayloadPool());
    std::shared_ptr<core::ParticipantsDatabase> part_db(new core::ParticipantsDatabase());
    std::shared_ptr<utils::SlotThreadPool> thread_pool(new utils::SlotThreadPool(test::N_THREADS));

    // Blank
    {
        core::types::ParticipantId part_id("Blank");
        part_db->add_participant(part_id, std::make_shared<participants::BlankParticipant>(part_id));
    }

    // Echo
    {
        std::shared_ptr<participants::EchoParticipantConfiguration> conf(
            new participants::EchoParticipantConfiguration());
        conf->id = core::types::ParticipantId("Echo");

        part_db->add_participant(conf->id, std::make_shared<participants::EchoParticipant>(
                    conf, discovery_database));
    }

    // Simple
    {
        std::shared_ptr<participants::SimpleParticipantConfiguration> conf(
            new participants::SimpleParticipantConfiguration());
        conf->id = core::types::ParticipantId("Simple");

        auto part = std::make_shared<participants::rtps::SimpleParticipant>(
            conf, payload_pool, discovery_database);
        part->init();
        part_db->add_participant(conf->id, part);
    }

    // Discovery Server
    {
        std::shared_ptr<participants::DiscoveryServerParticipantConfiguration> conf(
            new participants::DiscoveryServerParticipantConfiguration());
        conf->id = core::types::ParticipantId("DiscoveryServer");
        conf->listening_addresses.insert(participants::testing::random_address(1));

        auto part = std::make_shared<participants::rtps::DiscoveryServerParticipant>(
            conf, payload_pool, discovery_database);
        part->init();
        part_db->add_participant(conf->id, part);
    }

    // Initial Peers
    {
        std::shared_ptr<participants::InitialPeersParticipantConfiguration> conf(
            new participants::InitialPeersParticipantConfiguration());
        conf->id = core::types::ParticipantId("InitialPeers");
        conf->listening_addresses.insert(participants::testing::random_address(2));

        auto part = std::make_shared<participants::rtps::InitialPeersParticipant>(
            conf, payload_pool, discovery_database);
        part->init();
        part_db->add_participant(conf->id, part);
    }

    // Xml
    {
        std::shared_ptr<participants::XmlParticipantConfiguration> conf(
            new participants::XmlParticipantConfiguration());
        conf->id = core::types::ParticipantId("Xml");

        auto part = std::make_shared<participants::dds::XmlParticipant>(
            conf, payload_pool, discovery_database);
        part->init();
        part_db->add_participant(conf->id, part);
    }

    // Topic
    core::types::DdsTopic topic_1;
    topic_1.m_topic_name = "topic1";
    topic_1.type_name = "type1";
    eprosima::utils::Heritable<core::types::DistributedTopic> htopic_1 =
            eprosima::utils::Heritable<core::types::DdsTopic>::make_heritable(topic_1);

    // Special QoS Topic
    core::types::DdsTopic topic_2;
    topic_2.m_topic_name = "topic2";
    topic_2.type_name = "type2";
    topic_2.topic_qos.keyed = true;
    topic_2.topic_qos.ownership_qos = eprosima::ddspipe::core::types::OwnershipQosPolicyKind::EXCLUSIVE_OWNERSHIP_QOS;
    topic_2.topic_qos.use_partitions = true;
    eprosima::utils::Heritable<core::types::DistributedTopic> htopic_2 =
            eprosima::utils::Heritable<core::types::DdsTopic>::make_heritable(topic_2);

    // Create DDS Pipe
    core::DdsPipeConfiguration ddspipe_configuration;
    ddspipe_configuration.builtin_topics.insert(htopic_1);
    ddspipe_configuration.builtin_topics.insert(htopic_2);
    ddspipe_configuration.init_enabled = true;

    core::DdsPipe ddspipe(
        ddspipe_configuration,
        discovery_database,
        payload_pool,
        part_db,
        thread_pool
        );

    // Let everything destroy itself
}

/**
 * Test that writer creation falls back correctly depending on whether a matching XML profile exists.
 *
 * CASES:
 * - Topic name matches a loaded XML DataWriter profile  -> QoS comes from profile (DYNAMIC history memory policy)
 * - Topic name has no matching XML profile              -> QoS comes from default (PREALLOCATED_WITH_REALLOC)
 */
TEST(ParticipantsCreationgTest, writer_topic_profile_lookup)
{
    // Load an XML profile whose name matches the topic we will use in the first case
    participants::XmlHandlerConfiguration xml_conf;
    xml_conf.raw.set_value(
        R"(<?xml version="1.0" encoding="utf-8"?>
        <dds xmlns="http://www.eprosima.com">
            <profiles>
                <data_writer profile_name="writer_topic_with_profile">
                    <historyMemoryPolicy>DYNAMIC</historyMemoryPolicy>
                </data_writer>
            </profiles>
        </dds>)");
    ASSERT_EQ(participants::XmlHandler::load_xml(xml_conf), utils::ReturnCode::RETCODE_OK);

    std::shared_ptr<core::PayloadPool> payload_pool(new core::FastPayloadPool());

    // Create a raw DDS DomainParticipant to back the writer
    auto dds_participant =
            fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(
        0,
        fastdds::dds::PARTICIPANT_QOS_DEFAULT);
    ASSERT_NE(nullptr, dds_participant);

    core::types::ParticipantId part_id("WriterProfileTestPart");

    auto register_type_and_topic = [&](const std::string& topic_name, const std::string& type_name)
            -> fastdds::dds::Topic*
            {
                fastdds::dds::TypeSupport type_support(
                    new participants::dds::TopicDataType(
                        payload_pool,
                        type_name,
                        fastdds::dds::xtypes::TypeIdentifierPair(),
                        false));
                dds_participant->register_type(type_support);
                return dds_participant->create_topic(
                    topic_name,
                    type_name,
                    dds_participant->get_default_topic_qos());
            };

    // Case 1: profile exists -> DYNAMIC_RESERVE_MEMORY_MODE
    {
        core::types::DdsTopic topic;
        topic.m_topic_name = "writer_topic_with_profile";
        topic.type_name = "WriterProfileType";

        auto dds_topic = register_type_and_topic(topic.m_topic_name, topic.type_name);
        ASSERT_NE(nullptr, dds_topic);

        test::TestableWriter writer(part_id, topic, payload_pool, dds_participant, dds_topic);
        writer.init({});

        fastdds::dds::DataWriterQos qos;
        writer.get_dds_writer()->get_qos(qos);
        EXPECT_EQ(fastdds::rtps::DYNAMIC_RESERVE_MEMORY_MODE, qos.endpoint().history_memory_policy);
    }

    // Case 2: no profile -> PREALLOCATED_WITH_REALLOC_MEMORY_MODE (Fast DDS default)
    {
        core::types::DdsTopic topic;
        topic.m_topic_name = "writer_topic_without_profile";
        topic.type_name = "WriterNoProfileType";

        auto dds_topic = register_type_and_topic(topic.m_topic_name, topic.type_name);
        ASSERT_NE(nullptr, dds_topic);

        test::TestableWriter writer(part_id, topic, payload_pool, dds_participant, dds_topic);
        writer.init({});

        fastdds::dds::DataWriterQos qos;
        writer.get_dds_writer()->get_qos(qos);
        EXPECT_EQ(fastdds::rtps::PREALLOCATED_WITH_REALLOC_MEMORY_MODE,
                qos.endpoint().history_memory_policy);
    }

    fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(dds_participant);
}

/**
 * Test that reader creation falls back correctly depending on whether a matching XML profile exists.
 *
 * CASES:
 * - Topic name matches a loaded XML DataReader profile  -> QoS comes from profile (DYNAMIC history memory policy)
 * - Topic name has no matching XML profile              -> QoS comes from default (PREALLOCATED_WITH_REALLOC)
 */
TEST(ParticipantsCreationgTest, reader_topic_profile_lookup)
{
    participants::XmlHandlerConfiguration xml_conf;
    xml_conf.raw.set_value(
        R"(<?xml version="1.0" encoding="utf-8"?>
        <dds xmlns="http://www.eprosima.com">
            <profiles>
                <data_reader profile_name="reader_topic_with_profile">
                    <historyMemoryPolicy>DYNAMIC</historyMemoryPolicy>
                </data_reader>
            </profiles>
        </dds>)");
    ASSERT_EQ(participants::XmlHandler::load_xml(xml_conf), utils::ReturnCode::RETCODE_OK);

    std::shared_ptr<core::PayloadPool> payload_pool(new core::FastPayloadPool());

    // Create a raw DDS DomainParticipant to back the reader
    auto dds_participant =
            fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(
        0,
        fastdds::dds::PARTICIPANT_QOS_DEFAULT);
    ASSERT_NE(nullptr, dds_participant);

    core::types::ParticipantId part_id("ReaderProfileTestPart");

    auto register_type_and_topic = [&](const std::string& topic_name, const std::string& type_name)
            -> fastdds::dds::Topic*
            {
                fastdds::dds::TypeSupport type_support(
                    new participants::dds::TopicDataType(
                        payload_pool,
                        type_name,
                        fastdds::dds::xtypes::TypeIdentifierPair(),
                        false));
                dds_participant->register_type(type_support);
                return dds_participant->create_topic(
                    topic_name,
                    type_name,
                    dds_participant->get_default_topic_qos());
            };

    // Case 1: profile exists -> DYNAMIC_RESERVE_MEMORY_MODE
    {
        core::types::DdsTopic topic;
        topic.m_topic_name = "reader_topic_with_profile";
        topic.type_name = "ReaderProfileType";

        auto dds_topic = register_type_and_topic(topic.m_topic_name, topic.type_name);
        ASSERT_NE(nullptr, dds_topic);

        test::TestableReader reader(part_id, topic, payload_pool, dds_participant, dds_topic);
        reader.init({}, "");

        fastdds::dds::DataReaderQos qos;
        reader.get_dds_reader()->get_qos(qos);
        EXPECT_EQ(fastdds::rtps::DYNAMIC_RESERVE_MEMORY_MODE, qos.endpoint().history_memory_policy);
    }

    // Case 2: no profile -> PREALLOCATED_WITH_REALLOC_MEMORY_MODE (Fast DDS default)
    {
        core::types::DdsTopic topic;
        topic.m_topic_name = "reader_topic_without_profile";
        topic.type_name = "ReaderNoProfileType";

        auto dds_topic = register_type_and_topic(topic.m_topic_name, topic.type_name);
        ASSERT_NE(nullptr, dds_topic);

        test::TestableReader reader(part_id, topic, payload_pool, dds_participant, dds_topic);
        reader.init({}, "");

        fastdds::dds::DataReaderQos qos;
        reader.get_dds_reader()->get_qos(qos);
        EXPECT_EQ(fastdds::rtps::PREALLOCATED_WITH_REALLOC_MEMORY_MODE,
                qos.endpoint().history_memory_policy);
    }

    fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(dds_participant);
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
