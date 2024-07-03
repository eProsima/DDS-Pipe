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
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObject.hpp>

#include "type_objects/arrays_and_sequences.hpp"
#include "type_objects/arrays_and_sequencesPubSubTypes.hpp"

#include "type_objects/basic_array_struct.hpp"
#include "type_objects/basic_array_structPubSubTypes.hpp"

#include "type_objects/basic_struct.hpp"
#include "type_objects/basic_structPubSubTypes.hpp"

#include "type_objects/char_sequence.hpp"
#include "type_objects/char_sequencePubSubTypes.hpp"

#include "type_objects/complex_nested_arrays.hpp"
#include "type_objects/complex_nested_arraysPubSubTypes.hpp"

#include "type_objects/enum_struct.hpp"
#include "type_objects/enum_structPubSubTypes.hpp"

#include "type_objects/float_bounded_sequence.hpp"
#include "type_objects/float_bounded_sequencePubSubTypes.hpp"

#include "type_objects/hello_world.hpp"
#include "type_objects/hello_worldPubSubTypes.hpp"

#include "type_objects/map_struct.hpp"
#include "type_objects/map_structPubSubTypes.hpp"

#include "type_objects/numeric_array.hpp"
#include "type_objects/numeric_arrayPubSubTypes.hpp"

#include "type_objects/union_struct.hpp"
#include "type_objects/union_structPubSubTypes.hpp"


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
        SupportedType dyn_type)
{
    // Register the type
    eprosima::fastdds::dds::TypeSupport type_arrays_and_sequences(new arrays_and_sequencesPubSubType());
    type_arrays_and_sequences->register_type_object_representation();

    eprosima::fastdds::dds::TypeSupport type_basic_array_struct(new basic_array_structPubSubType());
    type_basic_array_struct->register_type_object_representation();

    eprosima::fastdds::dds::TypeSupport type_basic_struct(new basic_structPubSubType());
    type_basic_struct->register_type_object_representation();

    eprosima::fastdds::dds::TypeSupport type_complex_nested_arrays(new complex_nested_arraysPubSubType());
    type_complex_nested_arrays->register_type_object_representation();

    eprosima::fastdds::dds::TypeSupport type_char_sequence(new char_sequencePubSubType());
    type_char_sequence->register_type_object_representation();

    eprosima::fastdds::dds::TypeSupport type_enum_struct(new enum_structPubSubType());
    type_enum_struct->register_type_object_representation();

    eprosima::fastdds::dds::TypeSupport type_float_bounded_sequence(new float_bounded_sequencePubSubType());
    type_float_bounded_sequence->register_type_object_representation();

    eprosima::fastdds::dds::TypeSupport type_hello_world(new hello_worldPubSubType());
    type_hello_world->register_type_object_representation();

    eprosima::fastdds::dds::TypeSupport type_map_struct(new map_structPubSubType());
    type_map_struct->register_type_object_representation();

    eprosima::fastdds::dds::TypeSupport type_numeric_array(new numeric_arrayPubSubType());
    type_numeric_array->register_type_object_representation();


    eprosima::fastdds::dds::TypeSupport type_union_struct(new union_structPubSubType());
    type_union_struct->register_type_object_representation();

    auto type_name = to_string(dyn_type);

    eprosima::fastdds::dds::xtypes::TypeObjectPair type_objs;
    if (eprosima::fastdds::dds::RETCODE_OK == eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_objects(
            type_name,
            type_objs))
    {
        return eprosima::fastdds::dds::DynamicTypeBuilderFactory::get_instance()->create_type_w_type_object(type_objs.complete_type_object)->build();
    }

    else
    {
        throw eprosima::utils::InconsistencyException("No Type Object");
    }
}

} /* namespace test */
