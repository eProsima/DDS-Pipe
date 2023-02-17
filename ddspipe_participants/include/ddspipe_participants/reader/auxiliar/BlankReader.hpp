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

#include <ddspipe_core/interface/IReader.hpp>

#include <ddspipe_participants/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * Reader that has an empty implementation.
 * It does not receive anything.
 * It does not have messages to take.
 */
class BlankReader : public core::IReader
{
public:

    //! Override enable() IReader method
    DDSPIPE_PARTICIPANTS_DllAPI
    void enable() noexcept override;

    //! Override disable() IReader method
    DDSPIPE_PARTICIPANTS_DllAPI
    void disable() noexcept override;

    //! Override set_on_data_available_callback() IReader method
    DDSPIPE_PARTICIPANTS_DllAPI
    void set_on_data_available_callback(
            std::function<void()> on_data_available_lambda) noexcept override;

    //! Override unset_on_data_available_callback() IReader method
    DDSPIPE_PARTICIPANTS_DllAPI
    void unset_on_data_available_callback() noexcept override;

    //! Override take() IReader method
    DDSPIPE_PARTICIPANTS_DllAPI
    utils::ReturnCode take(
            std::unique_ptr<core::IRoutingData>& data) noexcept override;

    /////////////////////////
    // RPC REQUIRED METHODS
    /////////////////////////
    // TODO remove these methods once the double reference is solved

    //! Get GUID of internal RTPS reader
    DDSPIPE_PARTICIPANTS_DllAPI
    core::types::Guid guid() const override;

    //! Get internal RTPS reader mutex
    DDSPIPE_PARTICIPANTS_DllAPI
    fastrtps::RecursiveTimedMutex& get_rtps_mutex() const override;

    //! Get number of unread cache changes in internal RTPS reader
    DDSPIPE_PARTICIPANTS_DllAPI
    uint64_t get_unread_count() const override;

    DDSPIPE_PARTICIPANTS_DllAPI
    core::types::DdsTopic topic() const override;

    DDSPIPE_PARTICIPANTS_DllAPI
    core::types::ParticipantId participant_id() const override;
    /////////////////////////
};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
