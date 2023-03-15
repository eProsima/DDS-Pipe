// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

namespace eprosima {
namespace ddspipe {
namespace yaml {

template <typename T>
void YamlWriter::set(
        Yaml& yml,
        const TagType& tag,
        const T& value)
{
    auto yml_under_tag = YamlWriter::add_tag(yml, tag);
    YamlWriter::set<T>(yml_under_tag, value);
}

} /* namespace yaml */
} /* namespace ddspipe */
} /* namespace eprosima */
