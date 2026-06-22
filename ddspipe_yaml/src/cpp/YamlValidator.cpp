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

#include <ddspipe_yaml/yaml_configuration_tags.hpp>
#include <ddspipe_yaml/Yaml.hpp>
#include <ddspipe_yaml/YamlReader.hpp>

#include <ddspipe_yaml/YamlValidator.hpp>


namespace eprosima {
namespace ddspipe {
namespace yaml {

nlohmann::json YamlValidator::yaml_to_json(
        const Yaml& yml)
{
    if (yml.IsNull())
    {
        return nullptr;
    }

    if (yml.IsScalar())
    {
        // The only allowed scalar types are: boolean, integer, number (double) and string
        const std::string value = yml.as<std::string>();

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
            array.push_back(yaml_to_json(item));
        }
        return array;
    }

    if (yml.IsMap())
    {
        nlohmann::json object = nlohmann::json::object();
        for (const auto& item : yml)
        {
            std::string key = item.first.as<std::string>();
            object[key] = yaml_to_json(item.second);
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
        throw eprosima::utils::ConfigurationException("Unsupported YAML file, cannot be converted to JSON.");
    }

    throw eprosima::utils::ConfigurationException(
              utils::Formatter() << "Unsupported YAML file, cannot be converted to JSON.\n"
                                 << "Error in node: " << yml_as_string);
}

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
}

YamlValidator::YamlValidator()
    : validator(nullptr, format_checker)
{
}

YamlValidator::YamlValidator(
        const nlohmann::json& schema)
    : validator(nullptr, format_checker)
{
    try
    {
        validator.set_root_schema(schema);
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error occured while setting the JSON schema in the YamlValidator: "
                                     << e.what());
    }
}

YamlValidator::YamlValidator(
        const std::string& schema_path)
    : validator(nullptr, format_checker)
{
    try
    {
        std::ifstream schema_file;
        schema_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        schema_file.open(schema_path);
        nlohmann::json schema = nlohmann::json::parse(schema_file);
        validator.set_root_schema(schema);
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error occured while loading JSON schema from file: "
                                     << schema_path << " : " << e.what());
    }
}

void YamlValidator::set_schema(
        const nlohmann::json& schema)
{
    try
    {
        validator.set_root_schema(schema);
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error occured while setting the JSON schema in the YamlValidator: "
                                     << e.what());
    }
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

    // Covert YAML to JSON
    nlohmann::json instance = yaml_to_json(yml);

    // Create instance of the error collection
    CollectingErrorHandler err;

    // Validate the YAML file
    validator.validate(instance, err);

    // Check duplicated participant names
    std::set<std::string> participant_names;
    std::set<std::string> repeated_participant_names;
    if (YamlReader::is_tag_present(yml, COLLECTION_PARTICIPANTS_TAG))
    {
        if (yml[COLLECTION_PARTICIPANTS_TAG].IsSequence())
        {
            for (auto part : yml[COLLECTION_PARTICIPANTS_TAG])
            {
                if (YamlReader::is_tag_present(part, PARTICIPANT_NAME_TAG))
                {
                    std::string name = YamlReader::get<std::string>(part[PARTICIPANT_NAME_TAG],
                                    YamlReaderVersion::LATEST);
                    if (participant_names.count(name))
                    {
                        repeated_participant_names.insert(name);
                    }
                    else
                    {
                        participant_names.insert(name);
                    }
                }
            }
        }
    }

    // If there are errors, show them and return false
    if (!err.errors.empty() || !repeated_participant_names.empty())
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

            if (!repeated_participant_names.empty())
            {
                std::cerr << "Location: /" << COLLECTION_PARTICIPANTS_TAG << std::endl
                          << "Error: Participant names cannot be repeated. The following names are repeated:"
                          << std::endl;
                for (const auto& st : repeated_participant_names)
                {
                    std::cerr << "  - " << st << std::endl;
                }
                std::cerr << std::endl;
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