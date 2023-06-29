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
// limitations under the License.

#pragma once

#include <cpp_utils/ReturnCode.hpp>

#include <ddspipe_core/interface/IRoutingData.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * Interface that represents a generic Writer as part of a DdsPipe.
 *
 * This class manages the sending of data to remote endpoints in a specific topic.
 *
 * @note In order to implement new Writers, create a subclass of this Interface and implement every method.
 * @note Also it is needed to add the creation of the Writer in the Participant required.
 *
 * Writers will start being disabled.
 */
class IClient
{
public:

    /**
     * @brief Virtual dtor to allow inheritance.
     */
    DDSPIPE_CORE_DllAPI
    virtual ~IClient() = default;

    /**
     * @brief Enable Writer
     *
     * A Writer enabled can send messages.
     *
     * By default the Writer is disabled. Call this method to activate it.
     */
    DDSPIPE_CORE_DllAPI
    virtual void enable() noexcept = 0;

    /**
     * @brief Disable Writer
     *
     * A Writer disabled does not send data.
     * @note Method \c write should never be called from a disabled writer
     */
    DDSPIPE_CORE_DllAPI
    virtual void disable() noexcept = 0;

    DDSPIPE_CORE_DllAPI
    virtual utils::ReturnCode send_request(
            IRoutingData& data) noexcept = 0;

    DDSPIPE_CORE_DllAPI
    virtual void set_on_reply_callback(
            std::function<void()> on_reply_lambda) noexcept = 0;

    DDSPIPE_CORE_DllAPI
    virtual void unset_on_reply_callback() noexcept = 0;

    DDSPIPE_CORE_DllAPI
    virtual utils::ReturnCode take(
            std::unique_ptr<IRoutingData>& data) noexcept = 0;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
