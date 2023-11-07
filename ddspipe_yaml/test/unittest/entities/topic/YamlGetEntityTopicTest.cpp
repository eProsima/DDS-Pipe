// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <cpp_utils/memory/Heritable.hpp>
#include <cpp_utils/types/cast.hpp>

#include <ddspipe_participants/types/address/Address.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/dds/TopicQoS.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>
#include <ddspipe_yaml/YamlReader.hpp>
#include <ddspipe_yaml/yaml_configuration_tags.hpp>

#include <ddspipe_yaml/testing/generate_yaml.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe;
using namespace eprosima::ddspipe::yaml;
using namespace eprosima::ddspipe::yaml::testing;

namespace test {

// Check the values of a real topic are the expected ones
void compare_topic(
        const core::types::DdsTopic& topic,
        std::string name,
        std::string type,
        bool has_reliability_set = false,
        bool reliable = false)
{
    ASSERT_EQ(topic.m_topic_name, name);
    ASSERT_EQ(topic.type_name, type);

    if (has_reliability_set)
    {
        eprosima::ddspipe::core::types::ReliabilityKind expected_reliability_qos;
        if (reliable)
        {
            expected_reliability_qos = core::types::ReliabilityKind::RELIABLE;
        }
        else
        {
            expected_reliability_qos = core::types::ReliabilityKind::BEST_EFFORT;
        }

        ASSERT_EQ(topic.topic_qos.reliability_qos, expected_reliability_qos);
    }
}

// Check the values of a wildcard topic are the expected ones
void compare_wildcard_topic(
        const core::types::WildcardDdsFilterTopic& topic,
        std::string name,
        bool type_set,
        std::string type)
{
    ASSERT_EQ(topic.topic_name, name);

    if (type_set)
    {
        ASSERT_TRUE(topic.type_name.is_set());
        ASSERT_EQ(topic.type_name, type);
    }
}

const std::string TOPIC_NAME = "topic_name";
const std::string TOPIC_TYPE = "topic_type";

} /* namespace test */

/**
 * Test read core::types::DdsTopic from yaml
 *
 * POSITIVE CASES:
 * - Topic Std
 * - Topic with QoS
 */
TEST(YamlGetEntityTopicTest, get_real_topic)
{
    // Topic Std
    {
        core::types::DdsTopic real_topic;
        real_topic.m_topic_name = test::TOPIC_NAME;
        real_topic.type_name = test::TOPIC_TYPE;

        Yaml yml_topic;
        real_topic_to_yaml(
            yml_topic,
            real_topic);

        Yaml yml;
        yml["topic"] = yml_topic;

        core::types::DdsTopic topic = YamlReader::get<core::types::DdsTopic>(yml, "topic", LATEST);

        ASSERT_EQ(topic, real_topic);
    }
}

/**
 * Test read core::types::DdsTopic from yaml in negative cases
 * CASES:
 * - Empty
 * - Topic without name
 * - Topic without type
 */
TEST(YamlGetEntityTopicTest, get_real_topic_negative)
{
    // Empty
    {
        Yaml yml_topic;
        Yaml yml;
        yml["topic"] = yml_topic;

        ASSERT_THROW(YamlReader::get<core::types::DdsTopic>(yml, "topic",
                LATEST), eprosima::utils::ConfigurationException);
    }

    // Topic without name
    {
        Yaml yml_topic;
        add_field_to_yaml(yml_topic, YamlField<std::string>(test::TOPIC_TYPE), TOPIC_TYPE_NAME_TAG);

        Yaml yml;
        yml["topic"] = yml_topic;

        ASSERT_THROW(YamlReader::get<core::types::DdsTopic>(yml, "topic",
                LATEST), eprosima::utils::ConfigurationException);
    }

    // Topic without type
    {
        Yaml yml_topic;
        add_field_to_yaml(yml_topic, YamlField<std::string>(test::TOPIC_NAME), TOPIC_NAME_TAG);

        Yaml yml;
        yml["topic"] = yml_topic;

        ASSERT_THROW(YamlReader::get<core::types::DdsTopic>(yml, "topic",
                LATEST), eprosima::utils::ConfigurationException);
    }
}

/**
 * Test read correct core::types::WildcardDdsFilterTopic from yaml
 *
 * POSITIVE CASES:
 * - Topic Std
 * - Topic without name
 * - Topic without type
 */
TEST(YamlGetEntityTopicTest, get_wildcard_topic)
{
    // Topic Std
    {
        core::types::WildcardDdsFilterTopic w_topic;
        w_topic.topic_name.set_value(test::TOPIC_NAME);
        w_topic.type_name.set_value(test::TOPIC_TYPE);

        Yaml yml_topic;
        filter_topic_to_yaml(
            yml_topic,
            w_topic);

        Yaml yml;
        yml["topic"] = yml_topic;

        core::types::WildcardDdsFilterTopic topic = YamlReader::get<core::types::WildcardDdsFilterTopic>(yml, "topic",
                        LATEST);

        ASSERT_EQ(topic, w_topic);
    }

    // Topic without type
    {
        core::types::WildcardDdsFilterTopic w_topic;
        w_topic.topic_name.set_value(test::TOPIC_NAME);

        Yaml yml_topic;
        filter_topic_to_yaml(
            yml_topic,
            w_topic);

        Yaml yml;
        yml["topic"] = yml_topic;

        core::types::WildcardDdsFilterTopic topic = YamlReader::get<core::types::WildcardDdsFilterTopic>(yml, "topic",
                        LATEST);

        ASSERT_EQ(topic, w_topic);
        ASSERT_TRUE(topic.topic_name.is_set());
        ASSERT_FALSE(topic.type_name.is_set());
    }
}

/**
 * Test read correct a DdsTopic from a Heritable object
 */
TEST(YamlGetEntityTopicTest, get_real_topic_heritable)
{
    core::types::DdsTopic real_topic;
    real_topic.m_topic_name = test::TOPIC_NAME;
    real_topic.type_name = test::TOPIC_TYPE;

    Yaml yml_topic;
    real_topic_to_yaml(
        yml_topic,
        real_topic);

    Yaml yml;
    yml["topic"] = yml_topic;

    auto topic = YamlReader::get<utils::Heritable<core::types::DistributedTopic>>(yml, "topic", LATEST);

    ASSERT_EQ(real_topic, topic.get_reference());
    ASSERT_TRUE(utils::can_cast<core::types::DdsTopic>(topic.get_reference()));
}

/**
 * Test read correct a DdsTopic from a Heritable object
 */
TEST(YamlGetEntityTopicTest, get_wildcard_topic_heritable)
{
    core::types::WildcardDdsFilterTopic w_topic;
    w_topic.topic_name.set_value(test::TOPIC_NAME);
    w_topic.type_name.set_value(test::TOPIC_TYPE);

    Yaml yml_topic;
    filter_topic_to_yaml(
        yml_topic,
        w_topic);

    Yaml yml;
    yml["topic"] = yml_topic;

    auto topic = YamlReader::get<utils::Heritable<core::types::IFilterTopic>>(yml, "topic", LATEST);

    ASSERT_EQ(w_topic, topic.get_reference());
    ASSERT_TRUE(utils::can_cast<core::types::WildcardDdsFilterTopic>(topic.get_reference()));
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
