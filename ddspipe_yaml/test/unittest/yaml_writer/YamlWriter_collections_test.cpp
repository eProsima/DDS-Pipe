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

#include <list>
#include <set>
#include <vector>

#include <cpp_utils/exception/PreconditionNotMet.hpp>

#include <ddspipe_yaml/YamlWriter.hpp>
#include <ddspipe_yaml/YamlReader.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe::yaml;

namespace test {

struct A
{
    std::string name {};
    int value {0};

    bool operator ==(
            const A& other) const
    {
        return name == other.name && value == other.value;
    }

};

struct B
{
    bool active {true};
    A a {};

    bool operator ==(
            const B& other) const
    {
        return active == other.active && a == other.a;
    }

};

const TagType A_VALUE_TAG = "value";
const TagType A_NAME_TAG = "name";
const TagType B_ACTIVE_TAG = "active";
const TagType B_A_TAG = "a";

} // namespace test

namespace eprosima {
namespace ddspipe {
namespace yaml {

template <>
void set(
        Yaml& yml,
        const test::A& a)
{
    set(yml, test::A_VALUE_TAG, a.value);
    set(yml, test::A_NAME_TAG, a.name);
}

template <>
void set(
        Yaml& yml,
        const test::B& b)
{
    set(yml, test::B_ACTIVE_TAG, b.active);
    set(yml, test::B_A_TAG, b.a);
}

template <>
test::A YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    test::A a;
    a.value = get<int>(yml, test::A_VALUE_TAG, version);
    a.name = get<std::string>(yml, test::A_NAME_TAG, version);
    return a;
}

template <>
test::B YamlReader::get(
        const Yaml& yml,
        const YamlReaderVersion version)
{
    test::B b;
    b.active = get<bool>(yml, test::B_ACTIVE_TAG, version);
    b.a = get<test::A>(yml, test::B_A_TAG, version);
    return b;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */

/**
 * Test set with int vector
 */
TEST(YamlWriter_collections_test, set_int_vector)
{
    std::vector<int> values = {
        0,
        1,
        -1,
        99999,
        0
    };

    Yaml yml;
    set(yml, values);

    ASSERT_EQ(yml.size(), values.size());

    std::list<int> result_list = YamlReader::get_list<int>(yml, YamlReaderVersion::LATEST);
    std::vector<int> result(result_list.begin(), result_list.end());

    ASSERT_EQ(result, values);
}

/**
 * Test set with int set
 */
TEST(YamlWriter_collections_test, set_int_set)
{
    std::set<int> values = {
        0,
        1,
        -1,
        99999
    };

    Yaml yml;
    set(yml, values);

    ASSERT_EQ(yml.size(), values.size());

    std::list<int> result_list = YamlReader::get_list<int>(yml, YamlReaderVersion::LATEST);
    std::set<int> result(result_list.begin(), result_list.end());

    ASSERT_EQ(result, values);
}

/**
 * Test set with int set
 */
TEST(YamlWriter_collections_test, test_set_map)
{
    std::map<int, int> values = {
        {0, 0},
        {1, 10},
        {2, 20},
        {3, 0},
    };

    Yaml yml;
    set(yml, values);

    ASSERT_EQ(yml.size(), values.size());

    for (const auto& x : values)
    {
        ASSERT_EQ(YamlReader::get<int>(yml, std::to_string(x.first), YamlReaderVersion::LATEST), x.second);
    }
}

/**
 * Test set with B vector
 */
TEST(YamlWriter_collections_test, set_specific_type_vector)
{
    std::vector<test::B> values;
    values.push_back(test::B());
    values.push_back(test::B());

    Yaml yml;
    set(yml, values);

    ASSERT_EQ(yml.size(), values.size());

    std::list<test::B> result_list = YamlReader::get_list<test::B>(yml, YamlReaderVersion::LATEST);

    ASSERT_EQ(result_list.size(), 2);
    ASSERT_EQ(result_list.front(), test::B());
    ASSERT_EQ(result_list.back(), test::B());
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
