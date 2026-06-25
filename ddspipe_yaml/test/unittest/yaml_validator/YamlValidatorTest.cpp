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

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/YamlManager.hpp>
#include <ddspipe_yaml/YamlValidator.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe::yaml;

namespace test {
// Paths and files to test the constructors
std::string valid_schema_path = "./valid_draft07_schema.json";

std::string invalid_schema_path_1 = "./test_yaml_files/valid_draft07_schema.json";
std::string invalid_schema_path_2 = "./invalid_json_schema.json";
std::string invalid_schema_path_3 = "./invalid_draft07_schema.json";

std::string valid_schema_string= R"(
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

std::string invalid_schema_string= R"(
{
    "$schema":"http://json-schema.org/draft-07/schema#",
    "properties":["not", "an", "object"]
}
)";

// Invalid yaml file/node
Yaml valid_yml = YamlManager::load_file("./test_yaml_files/valid_test.yaml");
Yaml invalid_yml = valid_yml["missing_key"];

// Vectors with the valid and invalid YAML files
std::vector<std::string> valid_files = {
    "./test_yaml_files/valid_test.yaml"
};

std::vector<std::string> invalid_files = {
    "./test_yaml_files/invalid_version.yaml",
    "./test_yaml_files/invalid_uint.yaml",
    "./test_yaml_files/invalid_array.yaml",
    "./test_yaml_files/invalid_object.yaml",
    "./test_yaml_files/invalid_new_property.yaml"
};
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
        ASSERT_NO_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_FILE, test::valid_schema_path););
    }

    // constructor with path to file: invalid path
    {
        ASSERT_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_FILE, test::invalid_schema_path_1), utils::ConfigurationException);
    }

    // constructor with path to file: valid path and invalid file
    {
        ASSERT_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_FILE, test::invalid_schema_path_2), utils::ConfigurationException);
    }

    // constructor with path to file: valid path and valid file, but not json draft07
    {
        ASSERT_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_FILE, test::invalid_schema_path_3), utils::ConfigurationException);
    }

    // constructor with string with file content: valid file
    {
        ASSERT_NO_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_STRING, test::valid_schema_string));
    }

    // constructor with string with file content: valid file, but not json draft07
    {
        ASSERT_THROW(validator = YamlValidator(YamlValidator::InputType::FROM_STRING, test::invalid_schema_string), utils::ConfigurationException);
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

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
