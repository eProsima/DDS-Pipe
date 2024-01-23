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
// limitations under the License.

/**
 * @file all_types.hpp
 */

/*
 * USEFUL COMMAND
 *
 * for TYPE in hello_world numeric_array char_sequence basic_struct basic_array_struct float_bounded_sequence arrays_and_sequences complex_nested_arrays; do ${FASTDDSGEN_WS}/scripts/fastddsgen -replace -d ${WS}/src/recorder/ddsrecorder/test/unittest/dynamic_types/types/type_objects/ -typeobject -cs ${WS}/src/recorder/ddsrecorder/test/unittest/dynamic_types/types/idls/${TYPE}.idl; done
 */

#pragma once

#include <cpp_utils/macros/custom_enumeration.hpp>
#include <cpp_utils/utils.hpp>
#include <cpp_utils/exception/InconsistencyException.hpp>

#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/TypeObjectFactory.h>
#include <fastrtps/types/TypeIdentifier.h>

#include "type_objects/hello_worldTypeObject.h"
#include "type_objects/numeric_arrayTypeObject.h"
#include "type_objects/char_sequenceTypeObject.h"
#include "type_objects/basic_structTypeObject.h"
#include "type_objects/basic_array_structTypeObject.h"
#include "type_objects/float_bounded_sequenceTypeObject.h"
#include "type_objects/arrays_and_sequencesTypeObject.h"
#include "type_objects/complex_nested_arraysTypeObject.h"
#include "type_objects/enum_structTypeObject.h"
#include "type_objects/union_structTypeObject.h"
#include "type_objects/map_structTypeObject.h"

namespace test {

ENUMERATION_BUILDER(
    SupportedType,
    hello_world,
    numeric_array,
    char_sequence,
    basic_struct,
    basic_array_struct,
    float_bounded_sequence,
    arrays_and_sequences,
    complex_nested_arrays,
    enum_struct,
    union_struct,  // NOTE: default case currently not supported in dynamic types
    map_struct
    );

eprosima::fastrtps::types::DynamicType_ptr get_dynamic_type(
        SupportedType type)
{
    registerhello_worldTypes();
    registernumeric_arrayTypes();
    registerchar_sequenceTypes();
    registerbasic_structTypes();
    registerbasic_array_structTypes();
    registerfloat_bounded_sequenceTypes();
    registerarrays_and_sequencesTypes();
    registercomplex_nested_arraysTypes();
    registerenum_structTypes();
    registerunion_structTypes();
    registermap_structTypes();

    auto type_name = to_string(type);

    auto type_obj_factory = eprosima::fastrtps::types::TypeObjectFactory::get_instance();
    auto type_id = type_obj_factory->get_type_identifier(type_name, true);
    if (type_id == nullptr)
    {
        throw eprosima::utils::InconsistencyException("Not Type Id");
    }

    auto type_obj = type_obj_factory->get_type_object(type_id);
    if (type_obj == nullptr)
    {
        throw eprosima::utils::InconsistencyException("Not Type Object");
    }

    return type_obj_factory->build_dynamic_type(type_name, type_id, type_obj);
}

} /* namespace test */
