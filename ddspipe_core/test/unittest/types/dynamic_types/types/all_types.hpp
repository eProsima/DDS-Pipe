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

#include <fastdds/dds/core/ReturnCode.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObject.hpp>

#include "type_objects/hello_worldTypeObjectSupport.hpp"
#include "type_objects/numeric_arrayTypeObjectSupport.hpp"
#include "type_objects/char_sequenceTypeObjectSupport.hpp"
#include "type_objects/basic_structTypeObjectSupport.hpp"
#include "type_objects/basic_array_structTypeObjectSupport.hpp"
#include "type_objects/float_bounded_sequenceTypeObjectSupport.hpp"
#include "type_objects/arrays_and_sequencesTypeObjectSupport.hpp"
#include "type_objects/complex_nested_arraysTypeObjectSupport.hpp"
#include "type_objects/enum_structTypeObjectSupport.hpp"
#include "type_objects/union_structTypeObjectSupport.hpp"
#include "type_objects/map_structTypeObjectSupport.hpp"

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

eprosima::fastdds::dds::DynamicType::_ref_type get_dynamic_type( // traits<eprosima::fastdds::dds::DynamicType>::ref_type
        SupportedType type)
{
    register_hello_world_type_objects();
    register_numeric_array_type_objects();
    register_char_sequence_type_objects();
    register_basic_struct_type_objects();
    register_basic_array_struct_type_objects();
    register_float_bounded_sequence_type_objects();
    register_arrays_and_sequences_type_objects();
    register_complex_nested_arrays_type_objects();
    register_enum_struct_type_objects();
    register_union_struct_type_objects();
    register_map_struct_type_objects();

    auto type_name = to_string(type);
    fastdds::dds::xtypes::TypeInformation type_information;
    if (fastdds::dds::RETCODE_OK != fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_information(
            type_name,
            type_information))
    {
        return nullptr;
    }
        fastdds::dds::xtypes::TypeIdentifier type_id;
        type_id = type_information.complete().typeid_with_size().type_id();

        fastdds::dds::xtypes::TypeObject type_obj;
        if (fastdds::dds::RETCODE_OK == fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_object(
                type_id,
                type_obj))
    }

    else
    {
        throw eprosima::utils::InconsistencyException("No Type Object");
    }

    return eprosima::fastdds::dds::DynamicTypeBuilderFactory::get_instance()->create_type_w_type_object(type_objects.complete_type_object)->build();
}

} /* namespace test */
