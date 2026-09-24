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
// limitations under the License\.

#pragma once

#include <iostream>
#include <string>

#include <cpp_utils/types/Fuzzy.hpp>

#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/types/dds/TopicQoS.hpp>
#include <ddspipe_core/types/topic/Topic.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

/**
 * Data struct that represents a Distributed Topic of data flow in the Router.
 *
 * @note this not represents a real DDS network topic, but an "abstract" distributed one.
 */
struct
DistributedTopic : public Topic
{
    /////////////////////////
    // METHODS
    /////////////////////////

    //! Record QoS coming exclusively from user configuration. No-op unless overridden by a subclass.
    DDSPIPE_CORE_DllAPI
    virtual void set_user_configured_qos(
            const TopicQoS& qos,
            const utils::FuzzyLevelValues& fuzzy_level) noexcept
    {
    }

};

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
