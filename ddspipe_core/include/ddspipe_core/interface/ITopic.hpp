// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <cpp_utils/Formatter.hpp>
#include <cpp_utils/memory/Heritable.hpp>

#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/types/topic/TopicInternalTypeDiscriminator.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * Generic data struct that represents an ITopic of data flow in the Router.
 */
class ITopic
{
public:

    /**
     * @brief Virtual dtor to allow inheritance.
     */
    DDSPIPE_CORE_DllAPI
    virtual ~ITopic() = default;

    DDSPIPE_CORE_DllAPI
    virtual bool operator < (
            const ITopic& other) const noexcept = 0;

    DDSPIPE_CORE_DllAPI
    virtual bool operator == (
            const ITopic& other) const noexcept = 0;

    //! ITopic name
    DDSPIPE_CORE_DllAPI
    virtual std::string topic_name() const noexcept = 0;

    //! ITopic unique name in processs
    DDSPIPE_CORE_DllAPI
    virtual std::string topic_unique_name() const noexcept = 0;

    //! ITopic serialization to string
    DDSPIPE_CORE_DllAPI
    virtual std::string serialize() const noexcept = 0;

    //! Make a copy of the ITopic
    DDSPIPE_CORE_DllAPI
    virtual utils::Heritable<ITopic> copy() const noexcept = 0;

    /**
     * This refers to an internal used identifier that declares which kind of data type is going to be
     * transmitted in this Itopic inside the core.
     */
    DDSPIPE_CORE_DllAPI
    virtual types::TopicInternalTypeDiscriminator internal_type_discriminator() const noexcept = 0;

    //! ITopic discoverer participant id
    DDSPIPE_CORE_DllAPI
    virtual types::ParticipantId topic_discoverer() const noexcept = 0;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
