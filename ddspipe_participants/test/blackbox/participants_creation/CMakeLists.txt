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

set(TEST_NAME ParticipantsCreationgTest)

set(TEST_SOURCES
        ParticipantsCreationgTest.cpp
    )

set(TEST_LIST
        default_configuration
        creation_trivial
        ddspipe_all_creation_builtin_topic
    )

set(TEST_NEEDED_SOURCES
    )

add_blackbox_executable(
        "${TEST_NAME}"
        "${TEST_SOURCES}"
        "${TEST_LIST}"
        "${TEST_NEEDED_SOURCES}"
    )

# Set as TSAN errors due to Fast DDS
# TODO: use a cmake_utils macro
set(
    XTSAN_TEST_LIST
        "ParticipantsCreationgTest.creation_trivial"
        "ParticipantsCreationgTest.ddspipe_all_creation_builtin_topic"
)

foreach(XTSAN_TEST ${XTSAN_TEST_LIST})
    set_property(TEST ${XTSAN_TEST} PROPERTY LABELS xtsan)
endforeach()
