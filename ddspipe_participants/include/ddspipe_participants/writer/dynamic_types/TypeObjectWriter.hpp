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
 * @file TypeObjectWriter.hpp
 */

#pragma once

#include <condition_variable>
#include <mutex>

#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/participant/dynamic_types/ISchemaHandler.hpp>
#include <ddspipe_participants/writer/auxiliar/BaseWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * TODO
 */
class TypeObjectWriter : public BaseWriter
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    TypeObjectWriter(
            const core::types::ParticipantId& participant_id,
            std::shared_ptr<ISchemaHandler> schema_handler);

protected:

    /**
     * @brief Write specific method
     *
     * @param data : data to simulate publication
     * @return RETCODE_OK always
     */
    utils::ReturnCode write_nts_(
            core::IRoutingData& data) noexcept override;

    std::shared_ptr<ISchemaHandler> schema_handler_;
};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
