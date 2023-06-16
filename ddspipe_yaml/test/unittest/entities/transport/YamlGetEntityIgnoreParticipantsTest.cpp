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
 * Test read core::types::IgnoreParticipantFlags from yaml
 * has been parsed correctly with the tag set to no_filter
 */
TEST(YamlGetEntityIgnoreParticipantsTest, get_ignore_participnats_no_filter)
{
    {
        Yaml yml;

        add_field_to_yaml(
            yml,
            YamlField<std::string>(IGNORE_PARTICIPANT_FLAGS_NO_FILTER_TAG),
            IGNORE_PARTICIPANT_FLAGS_TAG);

        // Get core::types::IgnoreParticipantFlags from Yaml
        core::types::IgnoreParticipantFlags result = YamlReader::get<core::types::IgnoreParticipantFlags>(yml,
                        IGNORE_PARTICIPANT_FLAGS_TAG, LATEST);

        // Check result
        ASSERT_EQ(core::types::IgnoreParticipantFlags::no_filter, result);
    }
}

/**
 * Test read core::types::IgnoreParticipantFlags from yaml
 * has been parsed correctly with the tag set to filter_different_host
 */
TEST(YamlGetEntityIgnoreParticipantsTest, get_ignore_participnats_filter_different_host)
{
    {
        Yaml yml;

        add_field_to_yaml(
            yml,
            YamlField<std::string>(IGNORE_PARTICIPANT_FLAGS_DIFFERENT_HOST_TAG),
            IGNORE_PARTICIPANT_FLAGS_TAG);

        // Get core::types::IgnoreParticipantFlags from Yaml
        core::types::IgnoreParticipantFlags result = YamlReader::get<core::types::IgnoreParticipantFlags>(yml,
                        IGNORE_PARTICIPANT_FLAGS_TAG, LATEST);

        // Check result
        ASSERT_EQ(core::types::IgnoreParticipantFlags::filter_different_host, result);
    }
}

/**
 * Test read core::types::IgnoreParticipantFlags from yaml
 * has been parsed correctly with the tag set to filter_different_process
 */
TEST(YamlGetEntityAddressTest, get_ignore_participnats_filter_different_process)
{
    {
        Yaml yml;

        add_field_to_yaml(
            yml,
            YamlField<std::string>(IGNORE_PARTICIPANT_FLAGS_DIFFERENT_PROCESS_TAG),
            IGNORE_PARTICIPANT_FLAGS_TAG);

        // Get core::types::IgnoreParticipantFlags from Yaml
        core::types::IgnoreParticipantFlags result = YamlReader::get<core::types::IgnoreParticipantFlags>(yml,
                        IGNORE_PARTICIPANT_FLAGS_TAG, LATEST);

        // Check result
        ASSERT_EQ(core::types::IgnoreParticipantFlags::filter_different_process, result);
    }
}

/**
 * Test read core::types::IgnoreParticipantFlags from yaml
 * has been parsed correctly with the tag set to filter_same_process
 */
TEST(YamlGetEntityIgnoreParticipantsTest, get_ignore_participnats_filter_same_process)
{
    {
        Yaml yml;

        add_field_to_yaml(
            yml,
            YamlField<std::string>(IGNORE_PARTICIPANT_FLAGS_SAME_PROCESS_TAG),
            IGNORE_PARTICIPANT_FLAGS_TAG);

        // Get core::types::IgnoreParticipantFlags from Yaml
        core::types::IgnoreParticipantFlags result = YamlReader::get<core::types::IgnoreParticipantFlags>(yml,
                        IGNORE_PARTICIPANT_FLAGS_TAG, LATEST);

        // Check result
        ASSERT_EQ(core::types::IgnoreParticipantFlags::filter_same_process, result);
    }
}

/**
 * Test read core::types::IgnoreParticipantFlags from yaml
 * has been parsed correctly with the tag set to
 * filter_different_and_same_process
 */
TEST(YamlGetEntityIgnoreParticipantsTest, get_ignore_participnats_filter_different_and_same_process)
{
    {
        Yaml yml;

        add_field_to_yaml(
            yml,
            YamlField<std::string>(IGNORE_PARTICIPANT_FLAGS_DIFFERENT_AND_SAME_PROCESS_TAG),
            IGNORE_PARTICIPANT_FLAGS_TAG);

        // Get core::types::IgnoreParticipantFlags from Yaml
        core::types::IgnoreParticipantFlags result = YamlReader::get<core::types::IgnoreParticipantFlags>(yml,
                        IGNORE_PARTICIPANT_FLAGS_TAG, LATEST);

        // Check result
        ASSERT_EQ(core::types::IgnoreParticipantFlags::filter_different_and_same_process, result);
    }
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
