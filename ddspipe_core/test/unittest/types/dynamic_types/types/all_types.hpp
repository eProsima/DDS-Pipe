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

#include "type_objects/float_bounded_sequence.hpp"
#include "type_objects/float_bounded_sequencePubSubTypes.hpp"

#include "type_objects/hello_world.hpp"
#include "type_objects/hello_worldPubSubTypes.hpp"

#include "type_objects/numeric_array.hpp"
#include "type_objects/numeric_arrayPubSubTypes.hpp"

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
    );

void register_type_object_representation(
        const SupportedType& type)
{
    using namespace eprosima::fastdds::dds;

    TypeSupport type_support;

    if (type == SupportedType::arrays_and_sequences)
    {
        type_support.reset(new arrays_and_sequencesPubSubType());
    }
    else if (type == SupportedType::basic_array_struct)
    {
        type_support.reset(new basic_array_structPubSubType());
    }
    else if (type == SupportedType::basic_struct)
    {
        type_support.reset(new basic_structPubSubType());
    }
    else if (type == SupportedType::char_sequence)
    {
        type_support.reset(new char_sequencePubSubType());
    }
    else if (type == SupportedType::complex_nested_arrays)
    {
        type_support.reset(new complex_nested_arraysPubSubType());
    }
    else if (type == SupportedType::float_bounded_sequence)
    {
        type_support.reset(new float_bounded_sequencePubSubType());
    }
    else if (type == SupportedType::hello_world)
    {
        type_support.reset(new hello_worldPubSubType());
    }
    else if (type == SupportedType::numeric_array)
    {
        type_support.reset(new numeric_arrayPubSubType());
    }
    else
    {
        ASSERT_FALSE(true) << "Type not supported";
    }

    type_support->register_type_object_representation();
}

void get_dynamic_type(
        const SupportedType& type,
        eprosima::fastdds::dds::DynamicType::_ref_type& dyn_type)
{
    auto type_name = to_string(type);

    // Find TypeObjects for the type
    eprosima::fastdds::dds::xtypes::TypeObjectPair type_objs;
    ASSERT_EQ(eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_objects(
                type_name, type_objs),
            eprosima::fastdds::dds::RETCODE_OK);

    // Create DynamicType from TypeObject
    dyn_type = eprosima::fastdds::dds::DynamicTypeBuilderFactory::get_instance()->create_type_w_type_object(
        type_objs.complete_type_object)->build();
}

} /* namespace test */
