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

#include <ddspipe_core/configuration/TopicRoutesConfiguration.hpp>


using namespace eprosima;
using namespace eprosima::ddspipe;
using namespace eprosima::ddspipe::yaml;
using namespace eprosima::ddspipe::core::testing;
using namespace eprosima::ddspipe::yaml::testing;

/**
 * Check the get function for TopicRoutesConfiguration.
 *
 * CASES:
 *  Check that an exception is thrown when the topic route is not a list.
 */
TEST(YamlReaderTopicRoutesTest, throw_exception_when_not_list)
{
    const char* yml_str =
            R"(
            name: HelloWorld
            type: HelloWorld
            routes:
        )";

    Yaml yml = YAML::Load(yml_str);

    ASSERT_THROW(
        YamlReader::get<core::TopicRoutesConfiguration>(yml, YamlReaderVersion::LATEST),
        eprosima::utils::ConfigurationException);
}

/**
 * Check the get function for TopicRoutesConfiguration.
 *
 * CASES:
 *  Check that an exception is thrown when there is a route without a name.
 *  Check that an exception is thrown when there is a route without a type.
 *  Check that an exception is thrown when there is a route without a routes.
 */
TEST(YamlReaderTopicRoutesTest, throw_exception_when_missing_required_attrs)
{
    // Check that an exception is thrown when there is a route without a name.
    {
        const char* yml_str =
                R"(
                - type: HelloWorld
                  routes:
                    - src: P1
                      dst:
                        - P1
            )";

        Yaml yml = YAML::Load(yml_str);

        ASSERT_THROW(
            YamlReader::get<core::TopicRoutesConfiguration>(yml, YamlReaderVersion::LATEST),
            eprosima::utils::ConfigurationException);
    }

    // Check that an exception is thrown when there is a route without a type.
    {
        const char* yml_str =
                R"(
                - name: HelloWorld
                  routes:
                    - src: P1
                      dst:
                        - P1
            )";

        Yaml yml = YAML::Load(yml_str);

        ASSERT_THROW(
            YamlReader::get<core::TopicRoutesConfiguration>(yml, YamlReaderVersion::LATEST),
            eprosima::utils::ConfigurationException);
    }

    {
        const char* yml_str =
                R"(
                - name: HelloWorld
                  type: HelloWorld
            )";

        Yaml yml = YAML::Load(yml_str);

        ASSERT_THROW(
            YamlReader::get<core::TopicRoutesConfiguration>(yml, YamlReaderVersion::LATEST),
            eprosima::utils::ConfigurationException);
    }
}

/**
 * Check the get function for TopicRoutesConfiguration.
 *
 * CASES:
 *  Check that two participants with multiples destinations make up a valid configuration.
 */
TEST(YamlReaderTopicRoutesTest, throw_exception_when_repeat_name)
{
    const char* yml_str =
            R"(
            - name: HelloWorld
              type: HelloWorld
              routes:
                - src: P1
            - name: HelloWorld
              type: HelloWorld
              routes:
                - src: P1
        )";

    Yaml yml = YAML::Load(yml_str);

    ASSERT_THROW(
        YamlReader::get<core::TopicRoutesConfiguration>(yml, YamlReaderVersion::LATEST),
        eprosima::utils::ConfigurationException);
}

/**
 * Check the get function for TopicRoutesConfiguration.
 *
 * CASES:
 *  Check that two participants with multiples destinations make up a valid configuration.
 */
TEST(YamlReaderTopicRoutesTest, is_valid_conf_with_multi_participants)
{
    const char* yml_str =
            R"(
            - name: HelloWorld
              type: HelloWorld
              routes:
              - src: P1
                dst:
                  - P2
              - src: P2
            - name: AdiosWorld
              type: AdiosWorld
              routes:
              - src: P1
              - src: P2
                dst:
                  - P1
        )";

    Yaml yml = YAML::Load(yml_str);

    core::TopicRoutesConfiguration conf =
            YamlReader::get<core::TopicRoutesConfiguration>(yml, YamlReaderVersion::LATEST);

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
