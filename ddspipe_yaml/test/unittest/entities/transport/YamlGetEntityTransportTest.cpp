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

#include <ddspipe_core/types/dds/CustomTransport.hpp>
#include <ddspipe_yaml/YamlReader.hpp>
#include <ddspipe_yaml/yaml_configuration_tags.hpp>

#include <ddspipe_yaml/testing/generate_yaml.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe;
using namespace eprosima::ddspipe::yaml;
using namespace eprosima::ddspipe::yaml::testing;

/**
 * Test read core::types::TransportDescriptors from yaml
 * has been parsed correctly with the tag set to builtin
 */
TEST(YamlGetEntityTransportTest, get_transport_builtin)
{
    {
        Yaml yml;

        add_field_to_yaml(
            yml,
            YamlField<std::string>(TRANSPORT_DESCRIPTORS_BUILTIN_TAG),
            ADDRESS_TRANSPORT_TAG);

        // Get core::types::TransportDescriptors from Yaml
        core::types::TransportDescriptors result = YamlReader::get<core::types::TransportDescriptors>(yml,
                        TRANSPORT_DESCRIPTORS_TRANSPORT_TAG, LATEST);

        // Check result
        ASSERT_EQ(core::types::TransportDescriptors::builtin, result);
    }
}

/**
 * Test read core::types::TransportDescriptors from yaml
 * has been parsed correctly with the tag set to udp_only
 */
TEST(YamlGetEntityTransportTest, get_transport_udp_only)
{
    {
        Yaml yml;

        add_field_to_yaml(
            yml,
            YamlField<std::string>(TRANSPORT_DESCRIPTORS_UDP_TAG),
            ADDRESS_TRANSPORT_TAG);

        // Get core::types::TransportDescriptors from Yaml
        core::types::TransportDescriptors result = YamlReader::get<core::types::TransportDescriptors>(yml,
                        TRANSPORT_DESCRIPTORS_TRANSPORT_TAG, LATEST);

        // Check result
        ASSERT_EQ(core::types::TransportDescriptors::udp_only, result);
    }
}

/**
 * Test read core::types::TransportDescriptors from yaml
 * has been parsed correctly with the tag set to shm_only
 */
TEST(YamlGetEntityTransportTest, get_transport_shm_only)
{
    {
        Yaml yml;

        add_field_to_yaml(
            yml,
            YamlField<std::string>(TRANSPORT_DESCRIPTORS_SHM_TAG),
            TRANSPORT_DESCRIPTORS_TRANSPORT_TAG);

        // Get core::types::TransportDescriptors from Yaml
        core::types::TransportDescriptors result = YamlReader::get<core::types::TransportDescriptors>(yml,
                        TRANSPORT_DESCRIPTORS_TRANSPORT_TAG, LATEST);

        // Check result
        ASSERT_EQ(core::types::TransportDescriptors::shm_only, result);
    }
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
