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
 * @file TypeObjectWriter.cpp
 */

#include <cpp_utils/Log.hpp>

#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicTypePtr.h>

#include <ddspipe_core/types/dynamic_types/schema.hpp>
#include <ddspipe_core/types/dynamic_types/types.hpp>

#include <ddspipe_participants/writer/dynamic_types/TypeObjectWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;

TypeObjectWriter::TypeObjectWriter(
        const ParticipantId& participant_id,
        std::shared_ptr<ISchemaHandler> schema_handler)
    : BaseWriter(participant_id)
    , schema_handler_(schema_handler)
{
    // Do nothing
}

utils::ReturnCode TypeObjectWriter::write_nts_(
        IRoutingData& data) noexcept
{
    // Get the subclass DynamicTypeData
    // NOTE: if this fails it is a developer failure, thus the exception is correctly raised
    auto& dynamic_type_data = dynamic_cast<DynamicTypeData&>(data);

    // Add schema
    try
    {
        schema_handler_->add_schema(dynamic_type_data.dynamic_type);
    }
    catch (const utils::Exception& e)
    {
        logError(
            DDSPIPE_TYPEOBJECT_WRITER,
            "Error generating schema for type " << dynamic_type_data.dynamic_type->get_name()
                                                << " : <" << e.what() << ">.");
        return utils::ReturnCode::RETCODE_ERROR;
    }

    return utils::ReturnCode::RETCODE_OK;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
