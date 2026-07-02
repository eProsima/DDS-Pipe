// Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <sstream>

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>

#include <ddspipe_core/types/dds/CustomTransport.hpp>
#include <ddspipe_core/types/dds/GuidPrefix.hpp>

#include <ddspipe_participants/configuration/DiscoveryServerParticipantConfiguration.hpp>
#include <ddspipe_participants/configuration/EchoParticipantConfiguration.hpp>
#include <ddspipe_participants/configuration/InitialPeersParticipantConfiguration.hpp>
#include <ddspipe_participants/configuration/ParticipantConfiguration.hpp>
#include <ddspipe_participants/configuration/SimpleParticipantConfiguration.hpp>
#include <ddspipe_participants/configuration/XmlParticipantConfiguration.hpp>
#include <ddspipe_participants/types/address/Address.hpp>
#include <ddspipe_participants/types/security/tls/TlsConfiguration.hpp>
#include <ddspipe_participants/xml/XmlHandlerConfiguration.hpp>

#include <ddspipe_yaml/YamlReader.hpp>
#include <ddspipe_yaml/testing/generate_yaml.hpp>
#include <ddspipe_yaml/yaml_configuration_tags.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe;
using namespace eprosima::ddspipe::yaml;
using namespace eprosima::ddspipe::yaml::testing;

namespace test {

participants::types::Address make_udp_address(
        const std::string& ip,
        uint16_t port)
{
    return participants::types::Address(
        ip,
        port,
        port,
        participants::types::IpVersion::v4,
        participants::types::TransportProtocol::udp);
}

participants::types::Address make_tcp_address(
        const std::string& ip,
        uint16_t port)
{
    return participants::types::Address(
        ip,
        port,
        port,
        participants::types::IpVersion::v4,
        participants::types::TransportProtocol::tcp);
}

Yaml address_sequence(
        const std::vector<participants::types::Address>& addresses)
{
    Yaml yml;
    for (const auto& address : addresses)
    {
        Yaml address_yml;
        address_to_yaml(address_yml, address);
        yml.push_back(address_yml);
    }

    return yml;
}

Yaml tls_yaml()
{
    Yaml yml;
    yml[TLS_PRIVATE_KEY_TAG] = "server.key";
    yml[TLS_CERT_TAG] = "server.crt";
    yml[TLS_DHPARAMS_TAG] = "dh.pem";
    yml[TLS_PEER_VERIFICATION_TAG] = false;
    return yml;
}

} // namespace test

/**
 * Test that the base participant configuration reads the required id and that
 * the echo participant reads all its optional boolean flags.
 */
TEST(YamlReaderParticipantsTest, read_participant_and_echo_configuration)
{
    Yaml participant_yml;
    participantid_to_yaml(participant_yml, core::types::ParticipantId("base_participant"));

    auto participant = YamlReader::get<participants::ParticipantConfiguration>(participant_yml, LATEST);
    ASSERT_EQ(participant.id, core::types::ParticipantId("base_participant"));

    Yaml echo_yml = participant_yml;
    echo_yml[ECHO_DATA_TAG] = true;
    echo_yml[ECHO_DISCOVERY_TAG] = false;
    echo_yml[ECHO_VERBOSE_TAG] = true;

    auto echo = YamlReader::get<participants::EchoParticipantConfiguration>(echo_yml, LATEST);
    ASSERT_EQ(echo.id, core::types::ParticipantId("base_participant"));
    ASSERT_TRUE(echo.echo_data);
    ASSERT_FALSE(echo.echo_discovery);
    ASSERT_TRUE(echo.verbose);
}

/**
 * Test that the simple participant configuration reads all optional fields:
 * domain, whitelist, easy mode ip, transport, ignore flags, and participant QoS.
 */
TEST(YamlReaderParticipantsTest, read_simple_participant_optional_fields)
{
    Yaml yml;
    participantid_to_yaml(yml, core::types::ParticipantId("simple_participant"));
    domain_to_yaml(yml, core::types::DomainId(static_cast<core::types::DomainIdType>(42)));
    yml[WHITELIST_INTERFACES_TAG].push_back("127.0.0.1");
    yml[WHITELIST_INTERFACES_TAG].push_back("lo");
    easy_mode_ip_to_yaml(yml, "192.168.1.50");
    yml[TRANSPORT_DESCRIPTORS_TRANSPORT_TAG] = TRANSPORT_DESCRIPTORS_UDP_TAG;
    yml[IGNORE_PARTICIPANT_FLAGS_TAG] = IGNORE_PARTICIPANT_FLAGS_SAME_PROCESS_TAG;
    yml[PARTICIPANT_QOS_TAG][QOS_RELIABLE_TAG] = true;

    auto participant = YamlReader::get<participants::SimpleParticipantConfiguration>(yml, LATEST);

    ASSERT_EQ(participant.id, core::types::ParticipantId("simple_participant"));
    ASSERT_EQ(participant.domain.domain_id, 42);
    ASSERT_EQ(participant.whitelist.size(), 2u);
    ASSERT_TRUE(participant.whitelist.find("127.0.0.1") != participant.whitelist.end());
    ASSERT_TRUE(participant.whitelist.find("lo") != participant.whitelist.end());
    ASSERT_EQ(participant.easy_mode_ip, "192.168.1.50");
    ASSERT_EQ(participant.transport, core::types::TransportDescriptors::udp_only);
    ASSERT_EQ(participant.ignore_participant_flags, core::types::IgnoreParticipantFlags::filter_same_process);
    ASSERT_TRUE(participant.topic_qos.is_reliable());
}

/**
 * Test discovery server participant parsing across the versions that differ in
 * guid-prefix handling, and check optional addresses and TLS configuration.
 */
TEST(YamlReaderParticipantsTest, read_discovery_server_participant_across_versions)
{
    const auto listening = test::make_udp_address("127.0.0.1", 7400);
    const auto connection = test::make_tcp_address("192.168.1.20", 7410);
    const core::types::GuidPrefix guid_prefix("01.0f.00.00.00.00.00.00.00.00.ab.cd");

    // V_1_0 reads the guid directly from the participant root.
    {
        Yaml yml;
        participantid_to_yaml(yml, core::types::ParticipantId("ds_v1"));
        guid_prefix_to_yaml(yml, guid_prefix);

        auto participant = YamlReader::get<participants::DiscoveryServerParticipantConfiguration>(yml, V_1_0);
        ASSERT_EQ(participant.id, core::types::ParticipantId("ds_v1"));
        ASSERT_EQ(participant.discovery_server_guid_prefix, guid_prefix);
    }

    // V_4_0 uses the dedicated discovery-server-guid tag and parses the optional branches.
    {
        Yaml yml;
        participantid_to_yaml(yml, core::types::ParticipantId("ds_v4"));
        yml[LISTENING_ADDRESSES_TAG] = test::address_sequence({listening});
        yml[CONNECTION_ADDRESSES_TAG] = test::address_sequence({connection});
        yml[TLS_TAG] = test::tls_yaml();
        discovery_server_guid_prefix_to_yaml(yml, guid_prefix);

        auto participant = YamlReader::get<participants::DiscoveryServerParticipantConfiguration>(yml, V_4_0);
        ASSERT_EQ(participant.id, core::types::ParticipantId("ds_v4"));
        ASSERT_EQ(participant.listening_addresses.size(), 1u);
        ASSERT_EQ(participant.connection_addresses.size(), 1u);
        ASSERT_EQ(*participant.listening_addresses.begin(), listening);
        ASSERT_EQ(*participant.connection_addresses.begin(), connection);
        ASSERT_EQ(participant.discovery_server_guid_prefix, guid_prefix);
        ASSERT_EQ(participant.tls_configuration.kind, participants::types::TlsKind::both);
        ASSERT_FALSE(participant.tls_configuration.verify_peer);
        ASSERT_EQ(participant.tls_configuration.private_key_file, "server.key");
    }

    // V_5_0 keeps the guid optional.
    {
        Yaml yml;
        participantid_to_yaml(yml, core::types::ParticipantId("ds_v5"));

        auto participant = YamlReader::get<participants::DiscoveryServerParticipantConfiguration>(yml, V_5_0);
        ASSERT_EQ(participant.id, core::types::ParticipantId("ds_v5"));
        ASSERT_EQ(participant.discovery_server_guid_prefix, core::types::GuidPrefix());
    }
}

/**
 * Test that the initial peers and xml participant configurations read their
 * participant-specific optional fields correctly.
 */
TEST(YamlReaderParticipantsTest, read_initial_peers_and_xml_participant_configuration)
{
    const auto listening = test::make_udp_address("10.0.0.1", 11811);
    const auto connection = test::make_tcp_address("10.0.0.2", 11812);

    {
        Yaml yml;
        participantid_to_yaml(yml, core::types::ParticipantId("initial_peers"));
        yml[LISTENING_ADDRESSES_TAG] = test::address_sequence({listening});
        yml[CONNECTION_ADDRESSES_TAG] = test::address_sequence({connection});
        yml[TLS_TAG] = test::tls_yaml();
        repeater_to_yaml(yml, true);

        auto participant = YamlReader::get<participants::InitialPeersParticipantConfiguration>(yml, LATEST);
        ASSERT_EQ(participant.id, core::types::ParticipantId("initial_peers"));
        ASSERT_TRUE(participant.is_repeater);
        ASSERT_EQ(participant.listening_addresses.size(), 1u);
        ASSERT_EQ(participant.connection_addresses.size(), 1u);
        ASSERT_EQ(*participant.listening_addresses.begin(), listening);
        ASSERT_EQ(*participant.connection_addresses.begin(), connection);
        ASSERT_EQ(participant.tls_configuration.kind, participants::types::TlsKind::both);
    }

    {
        Yaml yml;
        participantid_to_yaml(yml, core::types::ParticipantId("xml_participant"));
        yml[XML_PARTICIPANT_PROFILE_TAG] = "participant_profile";
        repeater_to_yaml(yml, true);

        auto participant = YamlReader::get<participants::XmlParticipantConfiguration>(yml, LATEST);
        ASSERT_EQ(participant.id, core::types::ParticipantId("xml_participant"));
        ASSERT_TRUE(participant.is_repeater);
        ASSERT_TRUE(participant.participant_profile.is_set());
        ASSERT_EQ(participant.participant_profile.get_reference(), "participant_profile");
    }
}

/**
 * Test that the XML handler configuration reports the updated missing-file
 * validation message when a configured XML file is not accessible.
 */
TEST(YamlReaderParticipantsTest, xml_handler_configuration_invalid_file_message)
{
    participants::XmlHandlerConfiguration conf;
    conf.files.insert("./missing_xml_handler_configuration_test_file.xml");

    utils::Formatter error_msg;
    std::ostringstream ss;

    ASSERT_FALSE(conf.is_valid(error_msg));
    ss << error_msg;
    ASSERT_EQ(
        ss.str(),
        "File ./missing_xml_handler_configuration_test_file.xml does not exist or does not have read access. ");
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
