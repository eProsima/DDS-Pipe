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

/**
 * @file YamlValidator.cpp
 */

#include <fstream>
#include <iostream>

#include <fastdds/utils/IPLocator.hpp>

#include <cpp_utils/exception/ConfigurationException.hpp>

#include <ddspipe_yaml/YamlValidator.hpp>

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

void YamlValidator::format_checker(
        const std::string& format,
        const std::string& value)
{
    if (format == "v4")
    {
        if (!eprosima::fastdds::rtps::IPLocator::isIPv4(value))
        {
            throw std::invalid_argument(value + " is not a valid IPv4 address");
        }
    }
    else if (format == "v6")
    {
        if (!eprosima::fastdds::rtps::IPLocator::isIPv6(value))
        {
            throw std::invalid_argument(value + " is not a valid IPv6 address");
        }
    }
    else
    {
        EPROSIMA_LOG_WARNING(
            DDSPIPE_YAML,
            "Unsupported format '" << format << "' in JSON schema. Value '" << value << "' was not validated.");
    }
}

YamlValidator::YamlValidator()
    : validator(std::make_unique<nlohmann::json_schema::json_validator>(nullptr, format_checker))
{
}

YamlValidator::YamlValidator(
        InputType input_type,
        const std::string& schema_string)
    : validator(std::make_unique<nlohmann::json_schema::json_validator>(nullptr, format_checker))
{
    this->set_schema(input_type, schema_string);
}

YamlValidator::~YamlValidator() = default;

YamlValidator& YamlValidator::operator =(
        YamlValidator&&) = default;

void YamlValidator::set_schema(
        InputType input_type,
        const std::string& schema_string)
{
    nlohmann::json schema; // Define schema

    switch (input_type)
    {
        case InputType::FROM_FILE: // Treat input string as the path to the file with the schema
            try
            {
                std::ifstream schema_file;
                schema_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                schema_file.open(schema_string);
                schema = nlohmann::json::parse(schema_file);
            }
            catch (const std::exception& e)
            {
                throw eprosima::utils::ConfigurationException(
                          utils::Formatter() << "Error occured while loading JSON schema from file: "
                                             << schema_string << " :\n" << e.what());
            }
            break;

        case InputType::FROM_STRING: // Treat input string as the raw schema
            try
            {
                schema = nlohmann::json::parse(schema_string);
            }
            catch (const std::exception& e)
            {
                throw eprosima::utils::ConfigurationException(
                          utils::Formatter() << "Error occured while loading JSON schema from input string:\n"
                                             << e.what());
            }
            break;

        default:
            throw eprosima::utils::ConfigurationException(
                      utils::Formatter()
                          << "Error occured while loading JSON schema, unsupported YamlValidator::InpuType.\n");
    }

    // Set the validator schema preventing deletion of the current one
    auto new_validator = std::make_unique<nlohmann::json_schema::json_validator>(nullptr, format_checker);
    try
    {
        new_validator->set_root_schema(schema);
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error occured while setting the JSON schema in the YamlValidator:\n"
                                     << e.what());
    }
    validator = std::move(new_validator);
}

bool YamlValidator::validate_YAML(
        const Yaml& yml,
        bool display_errors)
{
    // Define nested struct and class for processing validation errors
    struct ValidationError
    {
        std::string path;
        std::string message;
        nlohmann::json value;
    };
    class CollectingErrorHandler : public nlohmann::json_schema::error_handler
    {
    public:

        std::vector<ValidationError> errors;

        void error(
                const nlohmann::json::json_pointer& ptr,
                const nlohmann::json& instance,
                const std::string& message) override
        {
            errors.push_back( {ptr.to_string(), message, instance} );
        }

    };

    // Create lambda function to convert from YAML to JSON
    auto yaml_to_json = [](auto&& self, const Yaml& yml) -> nlohmann::json
            {
                if (yml.IsNull())
                {
                    return nullptr;
                }

                if (yml.IsScalar())
                {
                    // The only allowed scalar types are: boolean, integer, number (double) and string
                    const std::string value = yml.as<std::string>();

                    // Ensure quoted numbers as well as quoted "true" and "false" are parsed to strings
                    if (yml.Tag() == "!")
                    {
                        return value;
                    }

                    if (value == "true")
                    {
                        return true;
                    }
                    if (value == "false")
                    {
                        return false;
                    }

                    try
                    {
                        return yml.as<int>();
                    } catch (...)
                    {
                    }
                    try
                    {
                        return yml.as<long>();
                    } catch (...)
                    {
                    }
                    try
                    {
                        return yml.as<u_int64_t>();
                    } catch (...)
                    {
                    }

                    try
                    {
                        return yml.as<double>();
                    } catch (...)
                    {
                    }

                    return value;
                }

                if (yml.IsSequence())
                {
                    nlohmann::json array = nlohmann::json::array();
                    for (const auto& item : yml)
                    {
                        array.push_back(self(self, item));
                    }
                    return array;
                }

                if (yml.IsMap())
                {
                    nlohmann::json object = nlohmann::json::object();
                    for (const auto& item : yml)
                    {
                        std::string key = item.first.as<std::string>();
                        object[key] = self(self, item.second);
                    }
                    return object;
                }

                std::string yml_as_string;
                try
                {
                    yml_as_string = yml.as<std::string>();
                }
                catch (...)
                {
                    throw eprosima::utils::ConfigurationException(
                              "Unsupported YAML file, cannot be converted to JSON.");
                }

                throw eprosima::utils::ConfigurationException(
                          utils::Formatter() << "Unsupported YAML file, cannot be converted to JSON.\n"
                                             << "Error in node: " << yml_as_string);
            };

    // Covert YAML to JSON
    nlohmann::json instance = yaml_to_json(yaml_to_json, yml);

    // Create instance of the error collection
    CollectingErrorHandler err;

    // Validate the YAML file
    validator->validate(instance, err);

    // If there are errors, show them and return false
    if (!err.errors.empty())
    {
        if (display_errors)
        {
            std::cerr << "YAML VALIDATION FAILED:" << std::endl << std::endl;

            for (const auto& e : err.errors)
            {
                std::cerr << "Location: "
                          << (e.path.empty() ? "/  (root of the YAML file)" : e.path)
                          << std::endl << "Error: " << e.message
                          << std::endl << "Value: " << e.value.dump(4)
                          << std::endl << std::endl;
            }
        }
        // Return false when the file is not valid
        return false;
    }
    // Return true when there are no errors validating
    return true;
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */