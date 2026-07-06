# Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#######################
# Yaml Validator Test #
#######################

set(TEST_NAME YamlValidatorTest)

file(
    GLOB_RECURSE THIRDPARTY_FILES
        "${PROJECT_SOURCE_DIR}/../thirdparty/nlohmann_json_schema_validator/*.cpp"
)

set(TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlValidator.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlManager.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReader_generic.cpp
        ${THIRDPARTY_FILES}
        YamlValidatorTest.cpp
    )

set(TEST_LIST
        constructors
        validation_without_schema
        validation_with_invalid_yaml_file
        validation_is_correct
        validation_edge_cases
        schema_loading_error_paths_preserve_previous_schema
        validation_reports_invalid_ip_formats
        validation_supports_nulls_and_root_level_error_output
    )

set(TEST_EXTRA_LIBRARIES
        yaml-cpp
        fastcdr
        fastdds
        cpp_utils
        ddspipe_core
        ddspipe_participants
    )

file(GLOB_RECURSE TEST_NEEDED_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.json"
    "test_yaml_files/*.yaml"
)

add_unittest_executable(
    "${TEST_NAME}"
    "${TEST_SOURCES}"
    "${TEST_LIST}"
    "${TEST_EXTRA_LIBRARIES}"
    "${TEST_NEEDED_SOURCES}"
)

