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
// limitations under the License\.

#pragma once

#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

using ManualTopic = std::pair<utils::Heritable<WildcardDdsFilterTopic>, std::set<ParticipantId>>;

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
