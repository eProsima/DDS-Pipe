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

#pragma once

#include <memory>

#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/Yaml.hpp>

namespace nlohmann {
namespace json_schema {
class json_validator;
} // namespace json_schema
} // namespace nlohmann

namespace eprosima {
namespace ddspipe {
namespace yaml {

/**
 * @brief Validates YAML objects against a JSON Schema (draft-07).
 *
 * Loads a schema from a file path or a raw string via \c InputType, then
 * validates \c Yaml nodes against it. Custom format checkers (e.g. IPv4/IPv6)
 * are applied automatically during validation.
 */
class DDSPIPE_YAML_DllAPI YamlValidator
{
private:

    std::unique_ptr<nlohmann::json_schema::json_validator> validator;

protected:

    /**
     * @brief Custom format checker registered with the JSON Schema validator.
     *
     * Called automatically during validation when the schema uses the \c format keyword.
     * Supported format values are \c "v4" (IPv4) and \c "v6" (IPv6).
     *
     * @param format Name of the format to check (e.g. \c "v4" or \c "v6").
     * @param value  Value to validate against the given format.
     *
     * @throws std::invalid_argument if \c value does not conform to \c format.
     */
    static void format_checker(
            const std::string& format,
            const std::string& value);

public:
    
    enum class InputType
    {
        FROM_STRING,
        FROM_FILE
    };

    /**
     * @brief Default constructor. Creates a \c YamlValidator with no schema loaded.
     *
     * \c validate_YAML will return \c false until a schema is set via \c set_schema.
     */
    YamlValidator();

    /**
     * @brief Construct a \c YamlValidator and load a JSON Schema (draft-07).
     *
     * @param input_type    Whether \c schema_string is a file path (\c InputType::FROM_FILE)
     *                      or raw JSON content (\c InputType::FROM_STRING).
     * @param schema_string File path or raw JSON string of the schema to load.
     *
     * @throws utils::ConfigurationException if the schema cannot be read, parsed, or is not
     *         a valid JSON Schema draft-07.
     */
    explicit YamlValidator(
            InputType input_type,
            const std::string& schema_string);

    /**
     * @brief Default destructor. Needed because \c std::unique_ptr needs the full definition
     * of \c nlohmann::json_schema::json_validator when destroying it.
     */
    ~YamlValidator();

    /**
     * @brief Move assignment operator. Needed because \c std::unique_ptr needs the full definition
     * of \c nlohmann::json_schema::json_validator when move-assigning (the old value is destroyed).
     */
    YamlValidator& operator=(YamlValidator&&);

    /**
     * @brief Set or replace the root schema of the validator.
     *
     * @param input_type    Whether \c schema_string is a file path (\c InputType::FROM_FILE)
     *                      or raw JSON content (\c InputType::FROM_STRING).
     * @param schema_string File path or raw JSON string of the schema to load.
     *
     * @throws utils::ConfigurationException if the schema cannot be read, parsed, or is not
     *         a valid JSON Schema draft-07.
     */
    void set_schema(
            InputType input_type,
            const std::string& schema_string);

    /**
     * @brief Validate a YAML object against the loaded schema.
     *
     * Returns \c false immediately if no schema has been loaded.
     *
     * @param yml Yaml object to validate.
     * @param display_errors If \c true, prints validation errors to \c stderr.
     * @return \c true if \c yml conforms to the schema, \c false otherwise.
     *
     * @throws utils::ConfigurationException if \c yml cannot be converted to JSON.
     */
    bool validate_YAML(
            const Yaml& yml,
            bool display_errors = true);
};

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */