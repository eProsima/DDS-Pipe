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


using namespace eprosima;
using namespace eprosima::ddspipe;
using namespace eprosima::ddspipe::yaml;
using namespace eprosima::ddspipe::core::testing;
using namespace eprosima::ddspipe::yaml::testing;

/**
 * TODO
 *
 * CASES:
 *  TODO
 */
TEST(YamlReaderMonitorTest, is_valid_conf_with_status_and_topics)
{
    const char* yml_str =
            R"(
            domain: 10
            status:
              enable: true
              domain: 11
              period: 2000
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

    ASSERT_EQ(conf.domain, 10);

    ASSERT_TRUE(conf.producers["status"].enabled);
    ASSERT_EQ(conf.producers["status"].period, 2000);
    ASSERT_EQ(conf.consumers["status"].domain, 11);
    ASSERT_EQ(conf.consumers["status"].topic_name, "DdsPipeStatus");

    ASSERT_TRUE(conf.producers["topics"].enabled);
    ASSERT_EQ(conf.producers["topics"].period, 3000);
    ASSERT_EQ(conf.consumers["topics"].domain, 10);
    ASSERT_EQ(conf.consumers["topics"].topic_name, "DdsPipeTopics");
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
