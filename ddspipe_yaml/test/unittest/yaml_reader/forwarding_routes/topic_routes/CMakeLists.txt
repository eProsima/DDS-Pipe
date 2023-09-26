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

#################################
# Yaml Reader Topic Routes Test #
#################################

set(TEST_NAME YamlReaderTopicRoutesTest)

set(TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReader_features.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReader_generic.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReader_participants.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReader_types.cpp
        YamlReaderTopicRoutesTest.cpp
    )

set(TEST_LIST
        throw_exception_when_not_list
        throw_exception_when_missing_required_attrs
        throw_exception_when_repeat_name
        is_valid_conf_with_multi_participants
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
