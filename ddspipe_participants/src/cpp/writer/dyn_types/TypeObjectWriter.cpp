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

#include <ddspipe_core/types/data/DynamicTypeData.hpp>
#include <ddspipe_core/types/dynamic_types/schema.hpp>
#include <ddspipe_core/types/dynamic_types/types.hpp>
#include <ddspipe_core/types/dynamic_types/utils.hpp>

#include <ddspipe_participants/writer/dyn_types/TypeObjectWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;

TypeObjectWriter::TypeObjectWriter(
        const ParticipantId& participant_id,
        std::shared_ptr<PayloadPool> payload_pool,
        std::shared_ptr<ISchemaHandler> schema_handler)
    : BaseWriter(participant_id, payload_pool)
    , schema_handler_(schema_handler)
{
    // Do nothing
}

utils::ReturnCode TypeObjectWriter::write_(
        IRoutingData& data) noexcept
{
    auto& dynamic_type_data = dynamic_cast<DynamicTypeData&>(data);
    auto dyn_type = dynamic_type_data.dynamic_type;

    if (nullptr == dyn_type)
    {
        logError(DDSPIPE_TYPEOBJECT_WRITER, "Could not write schema, Dynamic Type not valid.");
        return utils::ReturnCode::RETCODE_PRECONDITION_NOT_MET;
    }

    auto type_name = dyn_type->get_name();
    logInfo(DDSPIPE_TYPEOBJECT_WRITER,
            "Type Object received: "
            << type_name
            );

    // Add schema
    try
    {
        // Check if schema previously generated
        // NOTE: thread safe as write_ is called from BaseWriter::write with mutex
        auto it = schemas_.find(type_name);
        std::string schema;
        if (it != schemas_.end())
        {
            // TypeObject previously received, fetch from storage
            // WARNING: undefined behaviour if different types (schemas) happen to use the same name
            schema = it->second;
        }
        else
        {
            // New TypeObject received: generate and store schema
            schema = generate_dyn_type_schema(dyn_type);
            schemas_[type_name] = schema;
        }

        schema_handler_->add_schema(type_name, schema);
    }
    catch (const utils::Exception& e)
    {
        logError(
            DDSPIPE_TYPEOBJECT_WRITER,
            "Error generating schema for type " << type_name << ": <" << e.what() << ">.");
    }

    return utils::ReturnCode::RETCODE_OK;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
