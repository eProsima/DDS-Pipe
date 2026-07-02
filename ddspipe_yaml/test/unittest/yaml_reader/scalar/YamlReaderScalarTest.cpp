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
#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_yaml/YamlReader.hpp>
#include <ddspipe_yaml/yaml_configuration_tags.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe::yaml;

/**
 * Check to get a boolean from a yaml
 *
 * CASES:
 *  true
 *  false
 *  fail with int
 */
TEST(YamlReaderScalarTest, get_scalar_bool)
{
    // true
    {
        Yaml yml(true);
        ASSERT_TRUE(YamlReader::get_scalar<bool>(yml));
    }

    // false
    {
        Yaml yml(false);
        ASSERT_FALSE(YamlReader::get_scalar<bool>(yml));
    }

    // fail with int
    {
        Yaml yml(0);
        ASSERT_THROW(YamlReader::get_scalar<bool>(yml), eprosima::utils::ConfigurationException);
    }
}

/**
 * Check to get an int from a yaml
 *
 * For each case are tested arbitrary numbers (power of 2) and negative ones in signed cases
 *
 * CASES:
 *  int
 *  uint
 *  uint16
 *  int64
 *  fail with bool
 */
TEST(YamlReaderScalarTest, get_scalar_int)
{
    // int
    {
        for (int i = 1; i < 0x1000; i *= 2)
        {
            ASSERT_EQ(YamlReader::get_scalar<int>(Yaml(i)), i) << i;
            ASSERT_EQ(YamlReader::get_scalar<int>(Yaml(-i)), -i) << -i;
        }
    }

    // uint
    {
        for (unsigned int i = 1; i < 0x1000; i *= 2)
        {
            ASSERT_EQ(YamlReader::get_scalar<unsigned int>(Yaml(i)), i) << i;
        }
    }

    // uint16
    {
        for (uint16_t i = 1; i < 0x1000; i *= 2)
        {
            ASSERT_EQ(YamlReader::get_scalar<uint16_t>(Yaml(i)), i) << i;
        }
    }

    // int64
    {
        for (int64_t i = 1; i < 0x1000; i *= 2)
        {
            ASSERT_EQ(YamlReader::get_scalar<int64_t>(Yaml(i)), i) << i;
            ASSERT_EQ(YamlReader::get_scalar<int64_t>(Yaml(-i)), -i) << -i;
        }
    }

    // fail with bool
    {
        ASSERT_THROW(YamlReader::get_scalar<int>(Yaml(false)), eprosima::utils::ConfigurationException);
    }
}

/**
 * Check to get a decimal number from a yaml (note: integers are also valid)
 *
 * For each case are tested arbitrary numbers (power of 2) and negative ones in signed cases
 *
 * CASES:
 *  float
 *  double
 *  int
 *  uint
 *  uint16
 *  int64
 *  fail with bool
 */
TEST(YamlReaderScalarTest, get_scalar_decimal)
{
    // float
    {
        float delta = 0.123;
        for (int i = 1; i < 0x1000; i *= 2)
        {
            float decimal = i + delta;
            ASSERT_EQ(YamlReader::get_scalar<float>(Yaml(decimal)), decimal) << decimal;
            ASSERT_EQ(YamlReader::get_scalar<float>(Yaml(-decimal)), -decimal) << -decimal;
        }
    }

    // double
    {
        double delta = 0.123;
        for (int i = 1; i < 0x1000; i *= 2)
        {
            double decimal = i + delta;
            ASSERT_EQ(YamlReader::get_scalar<double>(Yaml(decimal)), decimal) << decimal;
            ASSERT_EQ(YamlReader::get_scalar<double>(Yaml(-decimal)), -decimal) << -decimal;
        }
    }

    // int
    {
        for (int i = 1; i < 0x1000; i *= 2)
        {
            // float
            ASSERT_EQ(YamlReader::get_scalar<float>(Yaml(i)), i) << i;
            ASSERT_EQ(YamlReader::get_scalar<float>(Yaml(-i)), -i) << -i;

            // double
            ASSERT_EQ(YamlReader::get_scalar<double>(Yaml(i)), i) << i;
            ASSERT_EQ(YamlReader::get_scalar<double>(Yaml(-i)), -i) << -i;
        }
    }

    // uint
    {
        for (unsigned int i = 1; i < 0x1000; i *= 2)
        {
            // float
            ASSERT_EQ(YamlReader::get_scalar<float>(Yaml(i)), i) << i;

            // double
            ASSERT_EQ(YamlReader::get_scalar<double>(Yaml(i)), i) << i;
        }
    }

    // uint16
    {
        for (uint16_t i = 1; i < 0x1000; i *= 2)
        {
            // float
            ASSERT_EQ(YamlReader::get_scalar<float>(Yaml(i)), i) << i;

            // double
            ASSERT_EQ(YamlReader::get_scalar<double>(Yaml(i)), i) << i;
        }
    }

    // int64
    {
        for (int64_t i = 1; i < 0x1000; i *= 2)
        {
            // float
            ASSERT_EQ(YamlReader::get_scalar<float>(Yaml(i)), i) << i;
            ASSERT_EQ(YamlReader::get_scalar<float>(Yaml(-i)), -i) << -i;

            // double
            ASSERT_EQ(YamlReader::get_scalar<double>(Yaml(i)), i) << i;
            ASSERT_EQ(YamlReader::get_scalar<double>(Yaml(-i)), -i) << -i;
        }
    }

    // fail with bool
    {
        // double
        ASSERT_THROW(YamlReader::get_scalar<float>(Yaml(false)), eprosima::utils::ConfigurationException);

        // double
        ASSERT_THROW(YamlReader::get_scalar<double>(Yaml(false)), eprosima::utils::ConfigurationException);
    }
}

/**
 * Check to get a string from a yaml

 * CASES:
 *  empty string
 *  short string
 *  long string
 *  numeric string
 *  numeric
 *  bool
 */
TEST(YamlReaderScalarTest, get_scalar_string)
{
    // empty string
    {
        ASSERT_EQ(YamlReader::get_scalar<std::string>(Yaml("")), "");
    }

    // short string
    {
        ASSERT_EQ(YamlReader::get_scalar<std::string>(Yaml("short_text")), "short_text");
    }

    // long string
    {
        std::string st =
                "This text is pretty long, but will be "
                "concatenated into just a single string. "
                "The disadvantage is that you have to quote "
                "each part, and newlines must be literal as "
                "usual.";

        ASSERT_EQ(YamlReader::get_scalar<std::string>(Yaml(st)), st);
    }

    // numeric string
    {
        ASSERT_EQ(YamlReader::get_scalar<std::string>(Yaml("1234567890")), "1234567890");
    }

    // numeric
    {
        ASSERT_EQ(YamlReader::get_scalar<std::string>(Yaml(111)), "111");
    }

    // bool
    {
        ASSERT_EQ(YamlReader::get_scalar<std::string>(Yaml(false)), "false");
    }
}

/**
 * Check to get a scalar type under a specific tag

 * CASES:
 *  bool
 *  int
 *  string
 *  no tag
 */
TEST(YamlReaderScalarTest, get_scalar_from_tag)
{
    const char* tag = "specific_tag";

    // bool
    {
        Yaml yml;
        yml[tag] = true;
        ASSERT_TRUE(YamlReader::get_scalar<bool>(yml, tag));
    }

    // int
    {
        Yaml yml;
        yml[tag] = 12345;
        ASSERT_EQ(YamlReader::get_scalar<int>(yml, tag), 12345);
    }

    // string
    {
        Yaml yml;
        yml[tag] = "this is a text";
        ASSERT_EQ(YamlReader::get_scalar<std::string>(yml, tag), "this is a text");
    }

    // no tag
    {
        Yaml yml;
        yml["other_tag"] = 3;
        ASSERT_THROW(YamlReader::get_scalar<int>(yml, tag), eprosima::utils::ConfigurationException);
    }
}

/**
 * Check that get_scalar fails in case that yaml is not a scalar
 *
 * CASES:
 *  null
 *  sequence
 *  map
 */
TEST(YamlReaderScalarTest, get_scalar_negative_cases)
{
    // null
    {
        Yaml yml;
        ASSERT_TRUE(yml.IsNull());
        ASSERT_THROW(YamlReader::get_scalar<int>(yml), eprosima::utils::ConfigurationException);
    }

    // sequence
    {
        Yaml yml;
        yml.push_back(3);
        yml.push_back(4);
        ASSERT_TRUE(yml.IsSequence());
        ASSERT_THROW(YamlReader::get_scalar<int>(yml), eprosima::utils::ConfigurationException);
    }

    // map
    {
        Yaml yml;
        yml["1"] = 3;
        yml["2"] = 4;
        ASSERT_TRUE(yml.IsMap());
        ASSERT_THROW(YamlReader::get_scalar<int>(yml), eprosima::utils::ConfigurationException);
    }
}
// TODO. danip
/**
 * Check reading nonnegative and positive numeric values from tagged yaml nodes
 */
TEST(YamlReaderScalarTest, get_positive_and_nonnegative_numbers_from_tag)
{
    // int values
    {
        Yaml yml;
        yml["nonnegative"] = 0;
        yml["positive"] = 7;
        yml["negative"] = -1;

        ASSERT_EQ(YamlReader::get_nonnegative_int(yml, "nonnegative"), 0u);
        ASSERT_EQ(YamlReader::get_positive_int(yml, "positive"), 7u);
        ASSERT_THROW(YamlReader::get_nonnegative_int(yml, "negative"), eprosima::utils::ConfigurationException);
        ASSERT_THROW(YamlReader::get_positive_int(yml, "nonnegative"), eprosima::utils::ConfigurationException);

        ASSERT_THROW(YamlReader::get_positive_int(yml, "missing"), eprosima::utils::ConfigurationException);
    }

    // float values
    {
        Yaml yml;
        yml["nonnegative"] = 0.0f;
        yml["positive"] = 2.5f;
        yml["negative"] = -0.25f;

        ASSERT_FLOAT_EQ(YamlReader::get_nonnegative_float(yml, "nonnegative"), 0.0f);
        ASSERT_FLOAT_EQ(YamlReader::get_positive_float(yml, "positive"), 2.5f);
        ASSERT_THROW(YamlReader::get_nonnegative_float(yml, "negative"), eprosima::utils::ConfigurationException);
        ASSERT_THROW(YamlReader::get_positive_float(yml, "nonnegative"), eprosima::utils::ConfigurationException);

        ASSERT_THROW(YamlReader::get_positive_float(yml, "missing"), eprosima::utils::ConfigurationException);
    }
}

/**
 * Check yaml list helpers for success and wrapped error paths
 */
TEST(YamlReaderScalarTest, get_list_and_set_helpers)
{
    // sequence success
    {
        Yaml yml;
        yml.push_back(1);
        yml.push_back(2);
        yml.push_back(3);

        std::list<int> result = YamlReader::get_list<int>(yml, LATEST);
        ASSERT_EQ(result, std::list<int>({1, 2, 3}));
    }

    // list under tag failure wraps nested exception
    {
        Yaml yml;
        yml["values"] = 5;
        ASSERT_THROW(YamlReader::get_list<int>(yml, "values", LATEST), eprosima::utils::ConfigurationException);
    }

    // set conversion removes duplicates
    {
        Yaml yml;
        yml["values"].push_back("alpha");
        yml["values"].push_back("beta");
        yml["values"].push_back("alpha");

        std::set<std::string> result = YamlReader::get_set<std::string>(yml, "values", LATEST);
        ASSERT_EQ(result.size(), 2u);
        ASSERT_TRUE(result.find("alpha") != result.end());
        ASSERT_TRUE(result.find("beta") != result.end());
    }
}

/**
 * Check timestamp parsing with optional units and failing cases
 */
TEST(YamlReaderScalarTest, get_timestamp)
{
    // valid timestamp with optional subsecond units
    {
        Yaml yml;
        yml[TIMESTAMP_DATETIME_TAG] = "2026-01-02_03-04-05";
        yml[TIMESTAMP_MILLISECONDS_TAG] = 6;
        yml[TIMESTAMP_MICROSECONDS_TAG] = 7;
        yml[TIMESTAMP_NANOSECONDS_TAG] = 8;

        const auto expected_base = utils::string_to_timestamp("2026-01-02_03-04-05", "%Y-%m-%d_%H-%M-%S", true);
        const auto expected = std::chrono::time_point_cast<utils::Timestamp::duration>(
            expected_base + std::chrono::milliseconds(6) + std::chrono::microseconds(7) + std::chrono::nanoseconds(8));

        ASSERT_EQ(YamlReader::get<utils::Timestamp>(yml, LATEST), expected);
    }

    // missing required datetime
    {
        Yaml yml;
        ASSERT_THROW(YamlReader::get<utils::Timestamp>(yml, LATEST), eprosima::utils::ConfigurationException);
    }

    // malformed datetime
    {
        Yaml yml;
        yml[TIMESTAMP_DATETIME_TAG] = "bad-datetime";
        ASSERT_THROW(YamlReader::get<utils::Timestamp>(yml, LATEST), eprosima::utils::ConfigurationException);
    }
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
