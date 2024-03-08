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

#include <cpp_utils/logging/LogConfiguration.hpp>

#include <ddspipe_yaml/YamlReader.hpp>

using namespace eprosima;

/**
 * Check the get function for LogConfiguration when parsing from YAML all Log Configuration tags.
 *
 * CASES:
 *  Checks:
 *  - If logging shows a valid configuration
 *  - If it chooses correctly log configuration when parsing from YAML all Log Configuration tags.
 */
TEST(YamlReaderLogConfiguration, parse_correct_LogConfiguration_yaml)
{
    const char* yml_str =
            R"(
            verbosity: info
            filter:
                error: "DDSPIPE"
                warning: ""
                info: "DEBUG"
        )";

    Yaml yml = YAML::Load(yml_str);

    // Load configuration from YAML
    const utils::LogConfiguration conf = ddspipe::yaml::YamlReader::get<utils::LogConfiguration>(yml,
                    ddspipe::yaml::YamlReaderVersion::LATEST);

    utils::Formatter error_msg;

    ASSERT_TRUE(conf.is_valid(error_msg));
    ASSERT_EQ(conf.verbosity.get_value(), utils::VerbosityKind::Info);
    ASSERT_EQ(conf.filter.at(utils::VerbosityKind::Error).get_value(), "DDSPIPE");
    ASSERT_EQ(conf.filter.at(utils::VerbosityKind::Warning).get_value(), "");
    ASSERT_EQ(conf.filter.at(utils::VerbosityKind::Info).get_value(), "DEBUG");
}

/**
 * Check the get function for LogConfiguration when parsing from YAML just a few Log Configuration tags.
 *
 * CASES:
 *  Checks:
 *  - If logging shows a valid configuration
 *  - If it chooses correctly log configuration when parsing from YAML some tags and sets the others by default.
 */
TEST(YamlReaderLogConfiguration, parse_correct_LogConfiguration_yaml_and_default)
{
    const char* yml_str =
            R"(
            filter:
                info: "DEBUG"
        )";

    Yaml yml = YAML::Load(yml_str);

    // Load configuration from YAML
    const utils::LogConfiguration conf = ddspipe::yaml::YamlReader::get<utils::LogConfiguration>(yml,
                    ddspipe::yaml::YamlReaderVersion::LATEST);

    utils::Formatter error_msg;

    ASSERT_TRUE(conf.is_valid(error_msg));
    ASSERT_EQ(conf.verbosity.get_value(), utils::VerbosityKind::Warning);
    ASSERT_EQ(conf.filter.at(utils::VerbosityKind::Error).get_value(), "");
    ASSERT_EQ(conf.filter.at(utils::VerbosityKind::Warning).get_value(), "");
    ASSERT_EQ(conf.filter.at(utils::VerbosityKind::Info).get_value(), "DEBUG");
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
