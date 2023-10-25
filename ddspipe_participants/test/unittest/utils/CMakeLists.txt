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

#################
# Demangle Test #
#################

set(TEST_NAME ROS2ManglingTest)

set(TEST_SOURCES
        ROS2ManglingTest.cpp
        ${PROJECT_SOURCE_DIR}/src/cpp/utils/ros2_mangling.cpp
    )

set(TEST_LIST
        remove_prefix
        get_ros_prefix_if_exists
        remove_ros_prefix_if_exists
        get_all_ros_prefixes
        demangle_if_ros_topic
        demangle_if_ros_type
        demangle_ros_topic_from_topic
        demangle_service_from_topic
        demangle_service_request_from_topic
        demangle_service_reply_from_topic
        demangle_service_type_only
    )

set(TEST_EXTRA_LIBRARIES
        cpp_utils
    )

add_unittest_executable(
        "${TEST_NAME}"
        "${TEST_SOURCES}"
        "${TEST_LIST}"
        "${TEST_EXTRA_LIBRARIES}"
    )
