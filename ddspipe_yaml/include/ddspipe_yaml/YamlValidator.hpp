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

//#include <ddspipe_yaml/library/library_dll.h>
#include <ddspipe_yaml/Yaml.hpp>
#include <ddspipe_yaml/YamlReader.hpp>

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

namespace eprosima {
namespace ddspipe {
namespace yaml {

/**
 * @brief Yaml Validator
 *
 * This class is used to validate Yaml objects.
 */
class YamlValidator
{
private:
    nlohmann::json_schema::json_validator validator;

    /**
     * @brief Convert \c yml to a nlohmann::json type
     *
     * @param yml Yaml object to convert.
     */
    nlohmann::json yaml_to_json(const Yaml& yml);

public:
    /**
     * @brief Default constructor. Creates a \c YamlValidator with an empty schema validator.
     */
    YamlValidator() = default;
    
    /**
     * @brief Construct a \c YamlValidator and load the given JSON schema.
     *
     * @param schema JSON schema to set as the root schema of the validator.
     */
    explicit YamlValidator(const nlohmann::json& schema);
    
    /**
     * @brief Construct a \c YamlValidator by loading a JSON schema from a file.
     *
     * @param schema_path Path to the JSON schema file.
     */
    explicit YamlValidator(const std::string& schema_path);

    /**
     * @brief Set or replace the root schema of the validator.
     *
     * @param schema JSON schema to set as the root schema of the validator.
     */
    void set_schema(const nlohmann::json& schema);

    /**
     * @brief Validate a YAML object against the loaded schema.
     *
     * @param yml Yaml object to validate.
     * @return \c true if \c yml conforms to the schema, \c false otherwise.
     */
    bool validateYAML(const Yaml& yml);

};

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */