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

set(TEST_NAME DdsPipeCommunicationMockTest)

set(TEST_SOURCES
        DdsPipeCommunicationMockTest.cpp
    )

set(TEST_LIST
        mock_communication_trivial
        mock_communication_before_enabling
        mock_communication_topic_discovery
        mock_communication_topic_allow
        mock_communication_multiple_participant_topics
    )

set(TEST_NEEDED_SOURCES
    )

add_blackbox_executable(
        "${TEST_NAME}"
        "${TEST_SOURCES}"
        "${TEST_LIST}"
        "${TEST_NEEDED_SOURCES}"
    )
