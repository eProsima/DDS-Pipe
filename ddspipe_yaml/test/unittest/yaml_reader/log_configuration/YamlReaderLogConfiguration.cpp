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

#include <ddspipe_core/configuration/DdsPipeLogConfiguration.hpp>
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
            publish:
              enable: true
              domain: 1
              topic-name: "DdsPipeLogs"
            stdout: false
            verbosity: info
            filter:
                error: "DDSPIPE"
                warning: ""
                info: "DEBUG"
        )";

    Yaml yml = YAML::Load(yml_str);

    // Load the configuration from the YAML
    const auto conf = ddspipe::yaml::YamlReader::get<ddspipe::core::DdsPipeLogConfiguration>(yml,
                    ddspipe::yaml::YamlReaderVersion::LATEST);

    // Verify that the configuration is valid
    utils::Formatter error_msg;
    ASSERT_TRUE(conf.is_valid(error_msg));

    // Verify that the publishing configuration is correct
    ASSERT_TRUE(conf.publish.enable);
    ASSERT_EQ(conf.publish.domain, 1);
    ASSERT_EQ(conf.publish.topic_name, "DdsPipeLogs");
    ASSERT_FALSE(conf.stdout_enable);

    // Verify that the verbosity and filters are correct
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
    const auto conf = ddspipe::yaml::YamlReader::get<ddspipe::core::DdsPipeLogConfiguration>(yml,
                    ddspipe::yaml::YamlReaderVersion::LATEST);

    utils::Formatter error_msg;

    ASSERT_TRUE(conf.is_valid(error_msg));
    ASSERT_EQ(conf.verbosity.get_value(), utils::VerbosityKind::Warning);
    ASSERT_EQ(conf.filter.at(utils::VerbosityKind::Error).get_value(), "");
    ASSERT_EQ(conf.filter.at(utils::VerbosityKind::Warning).get_value(), "");
    ASSERT_EQ(conf.filter.at(utils::VerbosityKind::Info).get_value(), "DEBUG");
}

/**
 * Verify that, when the publishing is disabled, the configuration is valid regardless of the domain and topic name.
 *
 * CASES:
 *  Checks:
 *  - The publishing is disabled.
 */
TEST(YamlReaderLogConfiguration, publishing_disabled)
{
    const char* yml_str =
            R"(
            publish:
              enable: false
              domain: 300
              topic-name: ""
        )";

    Yaml yml = YAML::Load(yml_str);

    // Load configuration from YAML
    const auto conf = ddspipe::yaml::YamlReader::get<ddspipe::core::DdsPipeLogConfiguration>(yml,
                    ddspipe::yaml::YamlReaderVersion::LATEST);

    // Verify that the configuration is valid
    utils::Formatter error_msg;
    ASSERT_TRUE(conf.is_valid(error_msg));
}

/**
 * Verify that the configuration is invalid when the domain is invalid.
 *
 * CASES:
 *  Checks:
 *  - The domain is too high.
 */
TEST(YamlReaderLogConfiguration, invalid_domain)
{
    const char* yml_str =
            R"(
            publish:
              enable: true
              domain: 300
              topic-name: "DdsPipeLogs"
        )";

    Yaml yml = YAML::Load(yml_str);

    // Load configuration from YAML
    const auto conf = ddspipe::yaml::YamlReader::get<ddspipe::core::DdsPipeLogConfiguration>(yml,
                    ddspipe::yaml::YamlReaderVersion::LATEST);

    // Verify that the configuration is invalid
    utils::Formatter error_msg;
    ASSERT_FALSE(conf.is_valid(error_msg));

    // Verify that the error message is correct
    ASSERT_EQ(error_msg.to_string(), "Invalid domain: 300");
}

/**
 * Verify that the configuration is invalid when the topic name is invalid.
 *
 * CASES:
 *  Checks:
 *  - The topic name is missing.
 *  - The topic name is empty.
 */
TEST(YamlReaderLogConfiguration, invalid_topic_name)
{
    const char* yml_str =
            R"(
            publish:
              enable: true
              domain: 1
        )";

    Yaml yml = YAML::Load(yml_str);

    // Load configuration from YAML
    const auto conf = ddspipe::yaml::YamlReader::get<ddspipe::core::DdsPipeLogConfiguration>(yml,
                    ddspipe::yaml::YamlReaderVersion::LATEST);

    // Verify that the configuration is invalid
    utils::Formatter error_msg;
    ASSERT_FALSE(conf.is_valid(error_msg));

    // Verify that the error message is correct
    ASSERT_EQ(error_msg.to_string(), "Empty topic name.");
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
