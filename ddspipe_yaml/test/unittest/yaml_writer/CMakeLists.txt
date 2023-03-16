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

############################
# Yaml Writer Test         #
############################

set(TEST_NAME YamlWriterTest)

set(TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReader_generic.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlWriter.cpp
        YamlWriterTest.cpp
    )

set(TEST_LIST
        set_scalar_bool
        set_scalar_int
        set_scalar_string

        add_tag
        add_tag_initialize
        add_tag_initialize_no_overwrite
        add_tag_initialize_no_overwrite_negative

        set_under_tags
        set_specific_type
    )

set(TEST_EXTRA_LIBRARIES
        yaml-cpp
        fastcdr
        fastrtps
        cpp_utils
        ddspipe_core
        ddspipe_participants
    )

add_unittest_executable(
    "${TEST_NAME}"
    "${TEST_SOURCES}"
    "${TEST_LIST}"
    "${TEST_EXTRA_LIBRARIES}")

####################################
# Yaml Writer Test for Collections #
####################################

set(TEST_NAME YamlWriter_collections_test)

set(TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReader_generic.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlWriter.cpp
        YamlWriter_collections_test.cpp
    )

set(TEST_LIST
        set_int_vector
        set_int_set
        test_set_map
        set_specific_type_vector
    )

set(TEST_EXTRA_LIBRARIES
        yaml-cpp
        fastcdr
        fastrtps
        cpp_utils
        ddspipe_core
        ddspipe_participants
    )

add_unittest_executable(
    "${TEST_NAME}"
    "${TEST_SOURCES}"
    "${TEST_LIST}"
    "${TEST_EXTRA_LIBRARIES}")
