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
 * @file SchemaWriter.cpp
 */

#include <cpp_utils/Log.hpp>
#include <cpp_utils/exception/InconsistencyException.hpp>

#include <ddspipe_core/types/data/RtpsPayloadData.hpp>

#include <ddspipe_participants/writer/dynamic_types/SchemaWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;
using eprosima::ddspipe::core::types::operator <<;

SchemaWriter::SchemaWriter(
        const ParticipantId& participant_id,
        const DdsTopic& topic,
        std::shared_ptr<PayloadPool> payload_pool,
        std::shared_ptr<ISchemaHandler> schema_handler)
    : BaseWriter(participant_id)
    , topic_(topic)
    , schema_handler_(schema_handler)
    , payload_pool_(payload_pool)
{
    // Do nothing
}

utils::ReturnCode SchemaWriter::write_nts_(
        core::IRoutingData& data) noexcept
{
    auto& rtps_data = dynamic_cast<RtpsPayloadData&>(data);

    logInfo(DDSPIPE_SCHEMA_WRITER,
            "Data in topic: "
            << topic_ << " received: "
            << rtps_data.payload
            );

    // Add this data to the schema handler
    try
    {
        schema_handler_->add_data(topic_, rtps_data);
    }
    catch (const utils::Exception& e)
    {
        logWarning(
            DDSPIPE_SCHEMA_WRITER,
            "Error writting data in topic " << topic_ << " : <" << e.what() << ">.");
        return utils::ReturnCode::RETCODE_ERROR;
    }

    return utils::ReturnCode::RETCODE_OK;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
