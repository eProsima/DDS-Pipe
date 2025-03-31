// Copyright 2025 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
#include <cpp_utils/exception/ConfigurationException.hpp>
#include <gtest/gtest.h>

#include <ddspipe_core/efficiency/payload/FastPayloadPool.hpp>

#include <ddspipe_participants/participant/rtps/SimpleParticipant.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe;

namespace test {

class SimpleParticipantTest : public participants::rtps::SimpleParticipant
{

public:

    SimpleParticipantTest(
            const std::shared_ptr<participants::SimpleParticipantConfiguration>& participant_configuration,
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            const std::shared_ptr<core::DiscoveryDatabase>& discovery_database)
        : SimpleParticipant(
            participant_configuration,
            payload_pool,
            discovery_database)
    {
    }

    fastdds::rtps::RTPSParticipantAttributes get_attributes() const
    {
        return reckon_participant_attributes_();
    }

    static const std::string easy_mode_invalid_transport_error_msg()
    {
        return "Easy mode configuration is incompatible with transport tag configurations.";
    }

    static const std::string easy_mode_invalid_ip_error_msg()
    {
        return "Invalid Easy Mode IP value. It must be a valid IPv4 address.";
    }

};

} // test

/**
 * Test to check that Easy Mode is set (resp. not set) in RTPSParticipantAttributes
 * if the configuration is valid (resp. invalid).
 *
 * POSITIVE CASES:
 * - Valid IPv4 + default transport
 * NEGATIVE CASES:
 * - transport set to non-builtin
 * - IP is not a valid IPv4 address
 *
 */
TEST(SimpleParticipantTests, simple_participant_easy_mode_configuration)
{
    // Auxiliar objects
    std::shared_ptr<core::PayloadPool> payload_pool(new core::FastPayloadPool());
    std::shared_ptr<core::DiscoveryDatabase> discovery_database(new core::DiscoveryDatabase());
    core::types::ParticipantId part_id("testPart");

    // Case 1: Valid IPv4 + default transport
    {
        std::shared_ptr<participants::SimpleParticipantConfiguration> conf(
            new participants::SimpleParticipantConfiguration());
        conf->id = part_id;
        conf->easy_mode_ip = "127.0.0.1";

        test::SimpleParticipantTest participant(conf, payload_pool, discovery_database);

        // Check that easy mode is configured
        fastdds::rtps::RTPSParticipantAttributes att = participant.get_attributes();
        ASSERT_EQ(att.easy_mode_ip, "127.0.0.1");

        // Creating a RTPSParticipant with Easy Mode enabled is not supported yet on Windows
        #ifndef _WIN32
        participant.init();
        #endif // _WIN32
    }

    // Case 2: transport set to non-builtin
    {
        std::shared_ptr<participants::SimpleParticipantConfiguration> conf(
            new participants::SimpleParticipantConfiguration());
        conf->id = part_id;
        conf->transport = core::types::TransportDescriptors::udp_only;
        conf->easy_mode_ip = "127.0.0.1";

        test::SimpleParticipantTest participant(conf, payload_pool, discovery_database);

        // Non-valid SimpleParticipantconfiguration: SimpleParticipantConfiguration::is_valid should return false
        utils::Formatter error_msg;
        std::ostringstream ss;
        ASSERT_FALSE(conf->is_valid(error_msg));
        ss << error_msg;
        ASSERT_EQ(
            ss.str(),
            test::SimpleParticipantTest::easy_mode_invalid_transport_error_msg());
    }

    // Case 3: IP is not a valid IPv4 address
    {
        std::shared_ptr<participants::SimpleParticipantConfiguration> conf(
            new participants::SimpleParticipantConfiguration());
        conf->id = part_id;
        conf->easy_mode_ip = "Foo";

        test::SimpleParticipantTest participant(conf, payload_pool, discovery_database);

        // Non-valid SimpleParticipantconfiguration: SimpleParticipantConfiguration::is_valid should return false
        utils::Formatter error_msg;
        std::ostringstream ss;
        ASSERT_FALSE(conf->is_valid(error_msg));
        ss << error_msg;
        ASSERT_EQ(
            ss.str(),
            test::SimpleParticipantTest::easy_mode_invalid_ip_error_msg());
    }
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
