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

#include <cpp_utils/exception/PreconditionNotMet.hpp>

#include <ddspipe_yaml/YamlWriter.hpp>
#include <ddspipe_yaml/YamlReader.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe::yaml;

namespace test {

const TagType tag = "tag";
const int value = 42;

//! Helper function to avoid calling YamlReader get with YamlReaderVersion
template <typename T>
T get(
        const Yaml& yml)
{
    return YamlReader::get<T>(yml, YamlReaderVersion::LATEST);
}

//! Helper function to avoid calling YamlReader get with YamlReaderVersion
template <typename T>
T get(
        const Yaml& yml,
        const TagType& tag)
{
    return YamlReader::get<T>(yml, tag, YamlReaderVersion::LATEST);
}

//! Simple data struct to implement a serialization and deserialization method to yaml.
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

//! Complex data struct with an internal A to implement a serialization and deserialization method to yaml.
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

// Tags to serialize the A and B classes in a yaml
const TagType A_VALUE_TAG = "value";
const TagType A_NAME_TAG = "name";
const TagType B_ACTIVE_TAG = "active";
const TagType B_A_TAG = "a";

} // namespace test

namespace eprosima {
namespace ddspipe {
namespace yaml {

//! Serialize an object A in a yaml
template <>
void set(
        Yaml& yml,
        const test::A& a)
{
    set(yml, test::A_VALUE_TAG, a.value);
    set(yml, test::A_NAME_TAG, a.name);
}

//! Serialize an object B in a yaml
template <>
void set(
        Yaml& yml,
        const test::B& b)
{
    set(yml, test::B_ACTIVE_TAG, b.active);
    set(yml, test::B_A_TAG, b.a);
}

//! Deserialize an object A from a yaml
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

//! Deserialize an object B from a yaml
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
 * Test << operator with bool value
 *
 * CASES:
 * - true
 * - false
 */
TEST(YamlWriterTest, set_scalar_bool)
{
    // true
    {
        Yaml yml;
        set(yml, true);
        ASSERT_TRUE(test::get<bool>(yml));
    }

    // false
    {
        Yaml yml;
        set(yml, false);
        ASSERT_FALSE(test::get<bool>(yml));
    }
}

/**
 * Test << operator with int value
 *
 * Set an array of numbers and test each of them
 */
TEST(YamlWriterTest, set_scalar_int)
{
    std::vector<int> cases = {
        0,
        1,
        999999,
        -1
    };

    for (int i : cases)
    {
        Yaml yml;
        set(yml, i);
        ASSERT_EQ(test::get<int>(yml), i);
    }
}

/**
 * Test << operator with int value
 *
 * Set an array of strings and test each of them
 */
TEST(YamlWriterTest, set_scalar_string)
{
    std::vector<std::string> cases = {
        "0",
        "a",
        "Some text with spaces and punctuation.",
        "(☞ ͡° ͜ʖ ͡°)☞"
    };

    for (std::string st : cases)
    {
        Yaml yml;
        set(yml, st);
        ASSERT_EQ(test::get<std::string>(yml), st);
    }
}

/**
 * Test add_tag function with default variables.
 *
 * CASES:
 * - create new tag
 * - create new tag and set it
 * - add tag already existent
 * - add multiple tags
 */
TEST(YamlWriterTest, add_tag)
{
    // create new tag
    {
        Yaml yml;
        Yaml under_tag = add_tag(yml, test::tag);

        ASSERT_FALSE(under_tag);
        ASSERT_EQ(yml[test::tag], under_tag);
    }

    // create new tag and set it
    {
        Yaml yml;
        Yaml under_tag = add_tag(yml, test::tag);
        set(under_tag, test::value);

        ASSERT_EQ(test::get<int>(under_tag), test::value);
        ASSERT_EQ(test::get<int>(yml, test::tag), test::value);
    }

    // add tag already existent
    {
        Yaml yml;
        Yaml under_tag = add_tag(yml, test::tag);
        set(under_tag, test::value);

        Yaml under_tag_again = add_tag(yml, test::tag);
        set(under_tag_again, 0);

        ASSERT_EQ(test::get<int>(under_tag), 0);
        ASSERT_EQ(test::get<int>(under_tag_again), 0);
        ASSERT_EQ(test::get<int>(yml, test::tag), 0);
    }

    // add multiple tags
    {
        TagType tag2 = "tag2";
        int value2 = 24;

        Yaml yml;
        Yaml under_tag1 = add_tag(yml, test::tag);
        set(under_tag1, test::value);

        Yaml under_tag2 = add_tag(yml, tag2);
        set(under_tag2, value2);

        ASSERT_EQ(test::get<int>(under_tag1), test::value);
        ASSERT_EQ(test::get<int>(under_tag2), value2);
    }
}

/**
 * Test add_tag function with initialize variable
 *
 * CASES:
 * - create new tag and set it
 * - create new tag already existent
 */
TEST(YamlWriterTest, add_tag_initialize)
{
    // create new tag and set it
    {
        Yaml yml;
        Yaml under_tag = add_tag(yml, test::tag, true);
        set(under_tag, test::value);

        ASSERT_EQ(test::get<int>(under_tag), test::value);
        ASSERT_EQ(test::get<int>(yml, test::tag), test::value);
    }

    // create new tag already existent
    {
        TagType tag2 = "tag2";

        Yaml yml;
        Yaml under_tag = add_tag(yml, test::tag);
        set(under_tag, test::value);

        Yaml under_tag_again = add_tag(yml, test::tag, true);

        ASSERT_FALSE(under_tag_again)
            << under_tag_again;
        ASSERT_EQ(yml[test::tag], under_tag_again);
    }
}

/**
 * Test add_tag function with initialize variable
 *
 * CASES:
 * - create new tag and set it
 */
TEST(YamlWriterTest, add_tag_initialize_no_overwrite)
{
    // create new tag and set it
    {
        Yaml yml;
        Yaml under_tag = add_tag(yml, test::tag, true, false);
        set(under_tag, test::value);

        ASSERT_EQ(test::get<int>(under_tag), test::value);
        ASSERT_EQ(test::get<int>(yml, test::tag), test::value);
    }
}

/**
 * Test add_tag function with initialize variable
 *
 * CASES:
 * - attempt to create a new tag that already exist defining no overwrite
 */
TEST(YamlWriterTest, add_tag_initialize_no_overwrite_negative)
{
    // attempt to create a new tag that already exist defining no overwrite
    {
        Yaml yml;
        Yaml under_tag = add_tag(yml, test::tag, true, false);
        set(under_tag, test::value);

        ASSERT_THROW(add_tag(yml, test::tag, true, false), utils::PreconditionNotMet);
    }
}

/**
 * Test << operator with a complex tree example
 *
 * Final tree:
 * os:
 *   linux:
 *     name: ubuntu
 *     version: 20
 *   windows:
 *     useful: false
 */
TEST(YamlWriterTest, set_under_tags)
{
    // Create Tree
    Yaml yml;
    Yaml yml_os = add_tag(yml, "os");
    Yaml yml_linux = add_tag(yml_os, "linux");
    Yaml yml_name = add_tag(yml_linux, "name");
    Yaml yml_version = add_tag(yml_linux, "version");
    Yaml yml_windows = add_tag(yml_os, "windows");
    Yaml yml_useful = add_tag(yml_windows, "useful");

    // Set values
    set<std::string>(yml_name, "ubuntu");
    set(yml_version, 20);
    set(yml_useful, false);

    // Construct yml by native API
    Yaml yml_expected;
    yml_expected["os"]["linux"]["name"] = "ubuntu";
    yml_expected["os"]["linux"]["version"] = 20;
    yml_expected["os"]["windows"]["useful"] = false;

    // Check yamls
    ASSERT_EQ(
        utils::generic_to_string(yml),
        utils::generic_to_string(yml_expected));
}

/**
 * This test the \c set functions implemented in this same test for classes A and B.
 * These classes specialize function \c set to serialize themselves in a yaml.
 * There is also a specialization of YamlReader::get for them, so they can be read from yaml afterwards.
 *
 * In the test a value of type B (that contains an object of type A) is created and serialized in a yaml.
 * This yaml is then introspected to check it has correctly being set, and also is deserialized in other B object.
 * Then 2 b objects are compared and should be the same.
 *
 * It also tries to serialize a different B object and check everything works as expected.
 */
TEST(YamlWriterTest, set_specific_type)
{
    // Create object
    test::B b;
    b.a.name = "test";
    b.a.value = 666;

    // Create yml
    Yaml yml;
    set(yml, "b", b);

    // Check the b tag exists, and some inside
    ASSERT_TRUE(yml["b"]);
    ASSERT_TRUE(yml["b"]["a"]);

    // Check that reading this object it gets the origina object
    auto b_read = YamlReader::get<test::B>(yml, "b", YamlReaderVersion::LATEST);
    ASSERT_EQ(b, b_read);

    // Parse other object
    test::B b2;
    b.a.value = 3;
    Yaml yml2;
    set(yml2, "b", b2);

    // Check that ymls are not equal
    ASSERT_NE(
        utils::generic_to_string(yml),
        utils::generic_to_string(yml2));

    // Get new b and check is as expected and different from first
    auto b_read2 = YamlReader::get<test::B>(yml2, "b", YamlReaderVersion::LATEST);
    ASSERT_EQ(b2, b_read2);
    ASSERT_FALSE(b_read == b_read2);
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
