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

/**
 * @file InternalReader.hpp
 */

#pragma once

#include <queue>
#include <memory>

#include <cpp_utils/types/Atomicable.hpp>

#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>

#include <ddspipe_participants/reader/auxiliar/BaseReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * Reader implementation that allows to introduce custom data to DDS Pipe.
 */
class InternalReader : public BaseReader
{
public:

    /**
     * @brief Construct a new Internal Reader object
     *
     * @param participant_id parent participant id
     * @param payload_pool DDS Pipe shared PayloadPool
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    InternalReader(
            const core::types::ParticipantId& participant_id);

    ~InternalReader() = default;

    /**
     * @brief Simulate data reception on Reader
     *
     * @param data : The data received (by simulation)
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    void simulate_data_reception(
            std::unique_ptr<core::IRoutingData>&& data) noexcept;

protected:

    void enable_nts_() noexcept override;

    /**
     * @brief Take specific method
     *
     * After \c take method, the data will be removed from \c data_to_send_ .
     *
     * @param data : oldest data to take
     * @return \c RETCODE_OK if data has been correctly taken
     * @return \c RETCODE_NO_DATA if \c data_to_send_ is empty
     */
    utils::ReturnCode take_nts_(
            std::unique_ptr<core::IRoutingData>& data) noexcept override;

    //! Stores the data that must be retrieved with \c take() method
    using DataReceivedType = utils::Atomicable<std::queue<std::unique_ptr<core::IRoutingData>>>;
    DataReceivedType data_to_send_;
};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
