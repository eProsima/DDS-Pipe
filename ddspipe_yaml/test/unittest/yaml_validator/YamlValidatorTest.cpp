// Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <iostream>

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>

#include <cpp_utils/exception/ConfigurationException.hpp>

#include <fastdds/dds/log/Log.hpp>

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/YamlManager.hpp>
#include <ddspipe_yaml/YamlValidator.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe::yaml;

namespace test {
// Valid schema file used by the happy-path constructor and validation tests
std::string valid_schema_path = "./valid_draft07_schema.json";

// Invalid constructor inputs:
// 1) wrong relative path, 2) malformed JSON file, 3) valid JSON but invalid draft-07 schema
std::string invalid_schema_path_1 = "./test_yaml_files/valid_draft07_schema.json";
std::string invalid_schema_path_2 = "./invalid_json_schema.json";
std::string invalid_schema_path_3 = "./invalid_draft07_schema.json";

// Minimal valid draft-07 schema used when the validator is constructed from a raw string
std::string valid_schema_string =
        R"(
{
    "$schema":"http://json-schema.org/draft-07/schema#",
    "type":"object",
    "additionalProperties":false,
    "properties":{
        "uint":{
            "type":"integer",
            "minimum": 0
        },
        "number-array":{
            "type":"array",
            "items":{
                "type":"number"
            }
        }
    }
}
)";

// Structurally invalid draft-07 schema: "properties" must be an object, not an array
std::string invalid_schema_string =
        R"(
{
    "$schema":"http://json-schema.org/draft-07/schema#",
    "properties":["not", "an", "object"]
}
)";

// Malformed JSON string used to trigger the parsing-error path in set_schema(FROM_STRING, ...)
std::string malformed_schema_string =
        R"(
{
    "$schema":"http://json-schema.org/draft-07/schema#",
    "type":"object",
)";

// Valid YAML instance used as the main schema-validation input.
Yaml valid_yml = YamlManager::load_file("./test_yaml_files/valid_test.yaml");

// Missing child node extracted from a valid file to exercise the YAML-to-JSON conversion failure path
Yaml invalid_yml = valid_yml["missing_key"];

// Files expected to pass validation with valid_draft07_schema.json
std::vector<std::string> valid_files = {
    "./test_yaml_files/valid_test.yaml"
};

// Files expected to fail validation for different reasons: wrong version, negative integer,
// wrong array type, invalid object contents, and unexpected additional property
std::vector<std::string> invalid_files = {
    "./test_yaml_files/invalid_version.yaml",
    "./test_yaml_files/invalid_uint.yaml",
    "./test_yaml_files/invalid_array.yaml",
    "./test_yaml_files/invalid_object.yaml",
    "./test_yaml_files/invalid_new_property.yaml"
};

// Schema used to exercise scalar conversion edge cases:
// quoted values that must remain strings, large numeric values, and custom format checks
std::string edge_cases_schema_string =
        R"(
{
    "$schema":"http://json-schema.org/draft-07/schema#",
    "type":"object",
    "additionalProperties":false,
    "properties":{
        "quoted-int":{
            "type":"string"
        },
        "quoted-float":{
            "type":"string"
        },
        "quoted-bool":{
            "type":"string"
        },
        "plain-int":{
            "type":"integer"
        },
        "plain-bool":{
            "type":"boolean"
        },
        "large-int-1":{
            "type":"integer"
        },
        "large-int-2":{
            "type":"integer"
        },
        "large-int-3":{
            "type":"integer"
        },
        "large-int-4":{
            "type":"integer"
        },
        "large-float-1":{
            "type":"number"
        },
        "large-float-2":{
            "type":"number"
        },
        "string-ipv4":{
            "type":"string",
            "format":"v4"
        },
        "string-ipv6":{
            "type":"string",
            "format":"v6"
        },
        "string-undefined-format":{
            "type":"string",
            "format":"undefined-format"
        }
    }
}
)";

// Schema restricted to the custom IPv4 and IPv6 format validators
std::string ip_format_schema_string =
        R"(
{
    "$schema":"http://json-schema.org/draft-07/schema#",
    "type":"object",
    "additionalProperties":false,
    "properties":{
        "string-ipv4":{
            "type":"string",
            "format":"v4"
        },
        "string-ipv6":{
            "type":"string",
            "format":"v6"
        }
    }
}
)";

// Schema used to verify YAML null nodes are converted to JSON null values
std::string nullable_schema_string =
        R"(
{
    "$schema":"http://json-schema.org/draft-07/schema#",
    "type":"object",
    "additionalProperties":false,
    "properties":{
        "nullable":{
            "type":"null"
        }
    }
}
)";

// Root-level integer schema used to trigger validation errors on the YAML root itself
std::string root_integer_schema_string =
        R"(
{
    "$schema":"http://json-schema.org/draft-07/schema#",
    "type":"integer"
}
)";

// YAML fixture covering scalar conversion edge cases:
// quoted scalars, large integers, and large floating-point values
Yaml edge_cases_types_yml = YAML::Load(
    "quoted-int: \"123\"\n"
    "quoted-float: \"123.456\"\n"
    "quoted-bool: \"false\"\n"
    "plain-int: 123\n"
    "plain-bool: true\n"
    "large-int-1: 2147483648\n"                 // 2^31
    "large-int-2: 4294967296\n"                 // 2^32
    "large-int-3: 9223372036854775808\n"        // 2^63
    "large-int-4: 18446744073709551615\n"       // 2^64 - 1
    "large-float-1: 18446744073709551616.0\n"   // 2^64 float
    "large-float-2: 1.7976931348623157e308\n"   // std::numeric_limits<double>::max()
    );

// YAML fixture with valid IPv4/IPv6 values plus an unsupported custom format,
// which should validate successfully while emitting a warning
Yaml edge_cases_format_yml = YAML::Load(
    "string-ipv4: 192.168.1.1\n"
    "string-ipv6: 2001:db8:85a3::8a2e:370:7334\n" //001:0db8:85a3:0000:0000:8a2e:0370:7334\n"
    "string-undefined-format: abc.123.def.456\n"
    );

// YAML fixture with invalid IPv4/IPv6 values to exercise format-validation failures
Yaml invalid_ip_format_yml = YAML::Load(
    "string-ipv4: 999.999.999.999\n"
    "string-ipv6: not-an-ipv6\n"
    );

// YAML fixture containing an explicit null value.
Yaml nullable_yml = YAML::Load(
    "nullable: ~\n"
    );

// Root scalar that violates root_integer_schema_string and produces a root-level validation error
Yaml root_scalar_string_yml = YAML::Load("not-an-integer");
} // namespace test


/**
 * Test the validator constructors
 *
 * CASES:
 *  constructor with path to file: valid path and file                             -> no exception
 *  constructor with path to file: invalid path                                    -> ConfigurationException
 *  constructor with path to file: valid path and invalid file                     -> ConfigurationException
 *  constructor with path to file: valid path and valid file, but not json draft07 -> ConfigurationException
 *
 *  constructor with string with file content: valid file                          -> no exception
 *  constructor with string with file content: valid file, but not json draft07    -> ConfigurationException
 */
TEST(YamlValidatorTest, constructors)
{
    // Define a YamlValidator variable to use in the ASSERTS to prevent ambiguous syntax
    YamlValidator validator;

    // constructor with path to file: valid path and file
    {
        ASSERT_NO_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_FILE, test::valid_schema_path));
    }

    // constructor with path to file: invalid path
    {
        ASSERT_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_FILE, test::invalid_schema_path_1),
                utils::ConfigurationException);
    }

    // constructor with path to file: valid path and invalid file
    {
        ASSERT_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_FILE, test::invalid_schema_path_2),
                utils::ConfigurationException);
    }

    // constructor with path to file: valid path and valid file, but not json draft07
    {
        ASSERT_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_FILE, test::invalid_schema_path_3),
                utils::ConfigurationException);
    }

    // constructor with string with file content: valid file
    {
        ASSERT_NO_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_STRING, test::valid_schema_string));
    }

    // constructor with string with file content: valid file, but not json draft07
    {
        ASSERT_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_STRING, test::invalid_schema_string),
                utils::ConfigurationException);
    }
}

/**
 * Test that the validation fails with no schema
 *
 * Create a validator without schema and try to validate a file
 */
TEST(YamlValidatorTest, validation_without_schema)
{
    // validation fails with validator without schema
    {
        YamlValidator validator = YamlValidator();
        ASSERT_FALSE(validator.validate_YAML(test::valid_yml));
    }
}

/**
 * Test that an exception is raised validating an invalid YAML file
 *
 * Try to validate an invalid YAML file
 */
TEST(YamlValidatorTest, validation_with_invalid_yaml_file)
{
    // validation throws an exception with invalid YAML file
    {
        YamlValidator validator = YamlValidator(YamlValidator::InputType::FROM_FILE, test::valid_schema_path);
        ASSERT_THROW(validator.validate_YAML(test::invalid_yml), utils::ConfigurationException);
    }
}

/**
 * Test that a set of valid YAML configurations pass the validation and that a
 * set of invalid YAML configuration don't pass it
 */
TEST(YamlValidatorTest, validation_is_correct)
{
    YamlValidator validator = YamlValidator(YamlValidator::InputType::FROM_FILE, test::valid_schema_path);

    // valid files
    {
        for (std::string st : test::valid_files)
        {
            Yaml yml = YamlManager::load_file(st);
            ASSERT_TRUE(validator.validate_YAML(yml)) << "Failed for file: " << st;
        }
    }

    // invalid files
    {
        for (std::string st : test::invalid_files)
        {
            Yaml yml = YamlManager::load_file(st);
            ASSERT_FALSE(validator.validate_YAML(yml, false)) << "Failed for file: " << st;
        }
    }
}

/**
 * Test a set of edge cases to ensure some edge cases are processed correctly
 */
TEST(YamlValidatorTest, validation_edge_cases)
{
    YamlValidator validator = YamlValidator(YamlValidator::InputType::FROM_STRING, test::edge_cases_schema_string);

    // all the types are processed correctly
    {
        ASSERT_TRUE(validator.validate_YAML(test::edge_cases_types_yml));
    }

    // all the formats are processed correctly
    {
        eprosima::fastdds::dds::Log::SetVerbosity(eprosima::fastdds::dds::Log::Kind::Warning);
        testing::internal::CaptureStderr();

        ASSERT_TRUE(validator.validate_YAML(test::edge_cases_format_yml));

        eprosima::fastdds::dds::Log::Flush();
        std::string output = testing::internal::GetCapturedStderr();
        ASSERT_NE(output.find("undefined-format"), std::string::npos); // the string is different from not found

        eprosima::fastdds::dds::Log::SetVerbosity(eprosima::fastdds::dds::Log::Kind::Error); // restore verbosity
    }

}

/**
 * Test invalid IPv4 and IPv6 values are reported as validation errors
 */
TEST(YamlValidatorTest, validation_reports_invalid_ip_formats)
{
    YamlValidator validator = YamlValidator(YamlValidator::InputType::FROM_STRING, test::ip_format_schema_string);

    testing::internal::CaptureStderr();
    ASSERT_FALSE(validator.validate_YAML(test::invalid_ip_format_yml));
    std::string output = testing::internal::GetCapturedStderr();

    ASSERT_NE(output.find("/string-ipv4"), std::string::npos);
    ASSERT_NE(output.find("/string-ipv6"), std::string::npos);
    ASSERT_NE(output.find("valid IPv4 address"), std::string::npos);
    ASSERT_NE(output.find("valid IPv6 address"), std::string::npos);
}

/**
 * Test additional schema-loading error paths and ensure a failed schema update
 * does not replace the current valid schema
 */
TEST(YamlValidatorTest, schema_loading_error_paths_preserve_previous_schema)
{
    YamlValidator validator = YamlValidator(YamlValidator::InputType::FROM_FILE, test::valid_schema_path);

    ASSERT_TRUE(validator.validate_YAML(test::valid_yml, false));

    ASSERT_THROW(
        validator.set_schema(YamlValidator::InputType::FROM_STRING, test::malformed_schema_string),
        utils::ConfigurationException);

    ASSERT_THROW(
        validator.set_schema(static_cast<YamlValidator::InputType>(99), test::valid_schema_string),
        utils::ConfigurationException);

    ASSERT_TRUE(validator.validate_YAML(test::valid_yml, false));
}

/**
 * Test null YAML values are converted correctly and root-level validation
 * errors are displayed with the root marker
 */
TEST(YamlValidatorTest, validation_supports_nulls_and_root_level_error_output)
{
    {
        YamlValidator validator = YamlValidator(YamlValidator::InputType::FROM_STRING, test::nullable_schema_string);
        ASSERT_TRUE(validator.validate_YAML(test::nullable_yml, false));
    }

    {
        YamlValidator validator = YamlValidator(YamlValidator::InputType::FROM_STRING,
                        test::root_integer_schema_string);

        testing::internal::CaptureStderr();
        ASSERT_FALSE(validator.validate_YAML(test::root_scalar_string_yml));
        std::string output = testing::internal::GetCapturedStderr();

        ASSERT_NE(output.find("YAML VALIDATION FAILED"), std::string::npos);
        ASSERT_NE(output.find("/  (root of the YAML file)"), std::string::npos);
    }
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
