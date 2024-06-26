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
 * @file SchemaHandler.hpp
 */

#pragma once

#include <string>

#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/type_representation/detail/dds_xtypes_typeobject.hpp>

#include <ddspipe_core/types/data/RtpsPayloadData.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * TODO
 */
class ISchemaHandler
{
public:

    virtual void add_schema(
            const fastdds::dds::DynamicType::_ref_type& dynamic_type,
            const std::string& type_name,
            const fastdds::dds::xtypes::TypeIdentifier& type_id) = 0;

    virtual void add_data(
            const core::types::DdsTopic& topic,
            core::types::RtpsPayloadData& data) = 0;
};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
