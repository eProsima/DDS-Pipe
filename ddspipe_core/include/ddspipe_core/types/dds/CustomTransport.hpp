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
// See the License for the specific language governing permissions and
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// limitations under the License.

#pragma once

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

//! Different options for transport configuration
enum class TransportDescriptors
{
    builtin,
    udp_only,
    shm_only
};

//! Possible values for Ignore Participant Flags
enum class IgnoreParticipantFlags
{
    no_filter,
    filter_different_host,
    filter_different_process,
    filter_same_process,
    filter_different_and_same_process
};

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
