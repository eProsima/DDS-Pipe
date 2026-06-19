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

#include <fstream>
#include <iostream>

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>

#include <cpp_utils/exception/ConfigurationException.hpp>

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/YamlManager.hpp>

#include <ddspipe_yaml/YamlValidator.hpp>

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe::yaml;

namespace test {
    // Paths and files to test the constructors
    std::ifstream valid_schema_file("./ddsrouter_config_schema.json");
    nlohmann::json valid_schema = nlohmann::json::parse(valid_schema_file);

    std::ifstream invalid_schema_file("./invalid_draft07_schema.json");
    nlohmann::json invalid_schema = nlohmann::json::parse(invalid_schema_file);

    std::string valid_schema_path = "./ddsrouter_config_schema.json";

    std::string invalid_schema_path_1 = "./invalid_config_files/ddsrouter_config_schema.json";
    std::string invalid_schema_path_2 = "./invalid_json_schema.json";
    std::string invalid_schema_path_3 = "./invalid_draft07_schema.json";

    // Invalid yaml file/node
    Yaml valid_yml = YamlManager::load_file("./valid_config_files/client-ddsrouter.yaml");
    Yaml invalid_yml = valid_yml["missing_key"];

    // Vectors with the valid and invalid YAML files
    std::vector<std::string> valid_files = {
        "./valid_config_files/client-ddsrouter.yaml",
        "./valid_config_files/server-ddsrouter.yaml",
        "./valid_config_files/change_domain_allowlist.yaml",
        "./valid_config_files/change_domain.yaml",
        "./valid_config_files/echo.yaml",
        "./valid_config_files/forwarding_routes.yaml",
        "./valid_config_files/repeater_client.yaml",
        "./valid_config_files/repeater_server.yaml",
        "./valid_config_files/ros_discovery_client.yaml",
        "./valid_config_files/ros_discovery_server.yaml",
        "./valid_config_files/wan_client.yaml",
        "./valid_config_files/wan_ds_client.yaml",
        "./valid_config_files/wan_ds_server.yaml",
        "./valid_config_files/wan_server.yaml",
        "./valid_config_files/xml.yaml"
    };

    std::vector<std::string> invalid_files = {
        "./invalid_config_files/address_no_port.yaml",
        "./invalid_config_files/builtin_topic_no_name.yaml",
        "./invalid_config_files/builtin_topic_no_type.yaml",
        "./invalid_config_files/address_no_ip_nor_domain.yaml",
        "./invalid_config_files/ds_participant_no_discovery_server_guid.yaml",
        "./invalid_config_files/ds_participant_no_listening_nor_connection_addresses.yaml",
        "./invalid_config_files/filter_topic_no_name.yaml",
        "./invalid_config_files/initial_peers_no_addresses.yaml",
        "./invalid_config_files/no_participant_kind.yaml",
        "./invalid_config_files/no_participant_name.yaml",
        "./invalid_config_files/participants_same_name.yaml",
        "./invalid_config_files/invalid_version.yaml",
        "./invalid_config_files/tls_ca_no_private_key_provided_cert.yaml",
        "./invalid_config_files/tls_no_ca.yaml"
    };
} // namespace test


/**
 * Test the validator constructors
 *
 * CASES:
 *  constructor with json: valid json draft07   -> no exception
 *  constructor with json: invalid json draft07 -> ConfigurationException
 * 
 *  constructor with path to file: valid path and file                             -> no exception
 *  constructor with path to file: invalid path                                    -> ConfigurationException
 *  constructor with path to file: valid path and invalid file                     -> ConfigurationException
 *  constructor with path to file: valid path and valid file, but not json draft07 -> ConfigurationException
 */
TEST(YamlValidatorTest, constructors)
{
    // constructor with json: valid json draft07
    {
        ASSERT_NO_THROW(YamlValidator(test::valid_schema));
    }

    // constructor with json: invalid json draft07 
    {
        ASSERT_THROW(YamlValidator(test::invalid_schema), utils::ConfigurationException);
    }

    // constructor with path to file: valid path and file
    {
        ASSERT_NO_THROW(YamlValidator(test::valid_schema_path));
    }

    // constructor with path to file: invalid path
    {
        ASSERT_THROW(YamlValidator(test::invalid_schema_path_1), utils::ConfigurationException);
    }

    // constructor with path to file: valid path and invalid file
    {
        ASSERT_THROW(YamlValidator(test::invalid_schema_path_2), utils::ConfigurationException);
    }

    // constructor with path to file: valid path and valid file, but not json draft07
    {
        ASSERT_THROW(YamlValidator(test::invalid_schema_path_3), utils::ConfigurationException);
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
        YamlValidator validator = YamlValidator(test::valid_schema);
        ASSERT_THROW(validator.validate_YAML(test::invalid_yml), utils::ConfigurationException);
    }
}

/**
 * Test that a set of valid YAML configurations pass the validation
 */
TEST(YamlValidatorTest, validation_success)
{
    YamlValidator validator = YamlValidator(test::valid_schema);

    // valid files
    {
        for (std::string st : test::valid_files)
        {
            Yaml yml = YamlManager::load_file(st);
            ASSERT_TRUE(validator.validate_YAML(yml)) << "Failed for file: " << st;
        }
    }
}

/**
 * Test that a set of invalid YAML configuration don't pass the validation
 */
TEST(YamlValidatorTest, validation_fail)
{
    YamlValidator validator = YamlValidator(test::valid_schema);

    // valid files
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
