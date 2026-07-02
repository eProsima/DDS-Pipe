# Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
# Yaml Reader Participants Test #
#################################

set(TEST_NAME YamlReaderParticipantsTest)

set(TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReader_generic.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReader_participants.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/YamlReader_types.cpp
        YamlReaderParticipantsTest.cpp
    )

set(TEST_LIST
        read_participant_and_echo_configuration
        read_simple_participant_optional_fields
        read_discovery_server_participant_across_versions
        read_initial_peers_and_xml_participant_configuration
        xml_handler_configuration_invalid_file_message
    )

set(TEST_EXTRA_LIBRARIES
        yaml-cpp
        fastcdr
        fastdds
        cpp_utils
        ddspipe_core
        ddspipe_participants
    )

add_unittest_executable(
    "${TEST_NAME}"
    "${TEST_SOURCES}"
    "${TEST_LIST}"
    "${TEST_EXTRA_LIBRARIES}")
