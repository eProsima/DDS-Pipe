// Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <string>

#include <yaml-cpp/yaml.h>

namespace eprosima {

/**
 * Configuration is in dictionary format
 *
 * YAML spec: https://yaml.org/spec/1.2.2/
 *
 * @note It is not legal to repeat keys in a YAML
 *
 * @note this object contains a reference to the actual yaml object. Thus passing it by value or by reference
 * makes no different.
 */
using Yaml = YAML::Node;

//! Type of tag in the yaml
using TagType = std::string;

} /* namespace eprosima */
