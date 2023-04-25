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

#include <ddspipe_participants/types/address/Address.hpp>

#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/dds/TopicQoS.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

#include <ddspipe_yaml/NewYamlReader.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe::yaml;

TEST(NewYamlGetEntityTopicTest, get_real_topic)
{
    Yaml yml_topic;
    // yml_topic["name"] = "something";
    yml_topic["type"] = "s";
    yml_topic["qos"] = "something";
    // yml_topic["r"] = "something";

    Yaml yml;
    yml["topic"] = yml_topic;

    ddspipe::core::types::DdsTopic topic;
    fill<ddspipe::core::types::DdsTopic>(topic, yml, "topic");

    ddspipe::core::types::DdsTopic real_topic;
    real_topic.m_topic_name = "something";
    real_topic.type_name = "s";

    ASSERT_EQ(topic, real_topic);

}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
