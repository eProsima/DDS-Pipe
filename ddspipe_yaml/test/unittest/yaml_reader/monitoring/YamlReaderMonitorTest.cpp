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

#include <cpp_utils/exception/ConfigurationException.hpp>
#include <cpp_utils/testing/LogChecker.hpp>

#include <ddspipe_yaml/YamlReader.hpp>
#include <ddspipe_yaml/yaml_configuration_tags.hpp>
#include <ddspipe_yaml/testing/generate_yaml.hpp>

#include <ddspipe_core/configuration/MonitorConfiguration.hpp>
#include <ddspipe_core/configuration/MonitorConsumerConfiguration.hpp>
#include <ddspipe_core/monitoring/producers/StatusMonitorProducer.hpp>
#include <ddspipe_core/monitoring/producers/TopicsMonitorProducer.hpp>


using namespace eprosima;
using namespace eprosima::ddspipe;
using namespace eprosima::ddspipe::yaml;
using namespace eprosima::ddspipe::core::testing;
using namespace eprosima::ddspipe::yaml::testing;

/**
 * Check the get function for the MonitorConfiguration.
 *
 * CASES:
 *  Verify that the configuration is invalid when the Status's topic-name is missing.
 */
TEST(YamlReaderMonitorTest, missing_status_topic_name)
{
    const char* yml_str =
            R"(
            domain: 10
            status:
              enable: true
              period: 2000
              domain: 11
            topics:
              enable: true
              period: 3000
              topic-name: "DdsPipeTopics"
        )";

    Yaml yml = YAML::Load(yml_str);

    core::MonitorConfiguration conf = YamlReader::get<core::MonitorConfiguration>(yml, YamlReaderVersion::LATEST);

    utils::Formatter error_msg;
    ASSERT_FALSE(conf.is_valid(error_msg));
    ASSERT_EQ(error_msg.to_string(), "Empty topic name.");
}

/**
 * Check the get function for the MonitorConfiguration.
 *
 * CASES:
 *  Verify that the configuration is invalid when the Topics's topic-name is missing.
 */
TEST(YamlReaderMonitorTest, missing_topics_topic_name)
{
    const char* yml_str =
            R"(
            domain: 10
            status:
              enable: true
              period: 2000
              domain: 11
              topic-name: "DdsPipeStatus"
            topics:
              enable: true
              period: 3000
        )";

    Yaml yml = YAML::Load(yml_str);

    core::MonitorConfiguration conf = YamlReader::get<core::MonitorConfiguration>(yml, YamlReaderVersion::LATEST);

    utils::Formatter error_msg;
    ASSERT_FALSE(conf.is_valid(error_msg));
    ASSERT_EQ(error_msg.to_string(), "Empty topic name.");
}

/**
 * Check the get function for the MonitorConfiguration.
 *
 * CASES:
 *  Verify that the configuration is parsed correctly.
 */
TEST(YamlReaderMonitorTest, is_valid_conf_with_status_and_topics)
{
    const char* yml_str =
            R"(
            domain: 10
            status:
              enable: true
              period: 2000
              domain: 11
              topic-name: "DdsPipeStatus"
            topics:
              enable: true
              period: 3000
              topic-name: "DdsPipeTopics"
        )";

    Yaml yml = YAML::Load(yml_str);

    core::MonitorConfiguration conf = YamlReader::get<core::MonitorConfiguration>(yml, YamlReaderVersion::LATEST);

    utils::Formatter error_msg;
    ASSERT_TRUE(conf.is_valid(error_msg));

    ASSERT_TRUE(conf.producers[ddspipe::core::STATUS_MONITOR_PRODUCER_ID].enabled);
    ASSERT_EQ(conf.producers[ddspipe::core::STATUS_MONITOR_PRODUCER_ID].period, 2000);
    ASSERT_EQ(conf.consumers[ddspipe::core::STATUS_MONITOR_PRODUCER_ID].domain, 11);
    ASSERT_EQ(conf.consumers[ddspipe::core::STATUS_MONITOR_PRODUCER_ID].topic_name, "DdsPipeStatus");

    ASSERT_TRUE(conf.producers[ddspipe::core::TOPICS_MONITOR_PRODUCER_ID].enabled);
    ASSERT_EQ(conf.producers[ddspipe::core::TOPICS_MONITOR_PRODUCER_ID].period, 3000);
    ASSERT_EQ(conf.consumers[ddspipe::core::TOPICS_MONITOR_PRODUCER_ID].domain, 10);
    ASSERT_EQ(conf.consumers[ddspipe::core::TOPICS_MONITOR_PRODUCER_ID].topic_name, "DdsPipeTopics");
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
