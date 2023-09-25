// Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <cpp_utils/exception/ConfigurationException.hpp>
#include <cpp_utils/testing/LogChecker.hpp>

#include <ddspipe_yaml/YamlReader.hpp>
#include <ddspipe_yaml/yaml_configuration_tags.hpp>
#include <ddspipe_yaml/testing/generate_yaml.hpp>

#include <ddspipe_core/configuration/RoutesConfiguration.hpp>


using namespace eprosima;
using namespace eprosima::ddspipe;
using namespace eprosima::ddspipe::yaml;
using namespace eprosima::ddspipe::core::testing;
using namespace eprosima::ddspipe::yaml::testing;

/**
 * Check the get function for RoutesConfiguration.
 *
 * CASES:
 *  Check that an exception is thrown when the route is not a list.
 */
TEST(YamlReaderRoutesTest, throw_exception_when_not_list)
{
    const char* yml_str =
            R"(
            src: P1
            dst: P1
        )";

    Yaml yml = YAML::Load(yml_str);

    ASSERT_THROW(
        YamlReader::get<core::RoutesConfiguration>(yml, YamlReaderVersion::LATEST),
        eprosima::utils::ConfigurationException);
}

/**
 * Check the get function for RoutesConfiguration.
 *
 * CASES:
 *  Check that an exception is thrown when there is a dst without a src.
 */
TEST(YamlReaderRoutesTest, throw_exception_when_no_src)
{
    const char* yml_str =
            R"(
            - dst:
                - P1
                - P2
        )";

    Yaml yml = YAML::Load(yml_str);

    ASSERT_THROW(
        YamlReader::get<core::RoutesConfiguration>(yml, YamlReaderVersion::LATEST),
        eprosima::utils::ConfigurationException);
}

/**
 * Check the get function for RoutesConfiguration.
 *
 * CASES:
 *  Check that an exception is thrown when the src is set twice for the same participant.
 */
TEST(YamlReaderRoutesTest, throw_exception_when_repeat_src_participant)
{
    const char* yml_str =
            R"(
            - src: P1
              dst:
                - P1
            - src: P1
              dst:
                - P1
        )";

    Yaml yml = YAML::Load(yml_str);

    ASSERT_THROW(
        YamlReader::get<core::RoutesConfiguration>(yml, YamlReaderVersion::LATEST),
        eprosima::utils::ConfigurationException);
}

/**
 * Check the get function for RoutesConfiguration.
 *
 * CASES:
 *  Check that a participant without a destination makes up a valid configuration.
 */
TEST(YamlReaderRoutesTest, is_valid_conf_when_no_dst)
{
    const char* yml_str =
            R"(
            - src: P1
        )";

    Yaml yml = YAML::Load(yml_str);

    core::RoutesConfiguration conf = YamlReader::get<core::RoutesConfiguration>(yml, YamlReaderVersion::LATEST);

    utils::Formatter error_msg;
    ASSERT_TRUE(conf.is_valid(error_msg));
}

/**
 * Check the get function for RoutesConfiguration.
 *
 * CASES:
 *  Check that two participants with multiples destinations make up a valid configuration.
 */
TEST(YamlReaderRoutesTest, is_valid_conf_with_multi_participants)
{
    const char* yml_str =
            R"(
            - src: P1
              dst:
                - P2
            - src: P2
              dst:
                - P1
                - P2
        )";

    Yaml yml = YAML::Load(yml_str);

    core::RoutesConfiguration conf = YamlReader::get<core::RoutesConfiguration>(yml, YamlReaderVersion::LATEST);

    utils::Formatter error_msg;
    ASSERT_TRUE(conf.is_valid(error_msg));
}


int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
