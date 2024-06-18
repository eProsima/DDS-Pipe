// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

/*!
 * @file complex_nested_arraysTypeObject.cpp
 * This source file contains the definition of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace { char dummy; }
#endif

#include "complex_nested_arrays.h"
#include "complex_nested_arraysTypeObject.h"
#include <utility>
#include <sstream>
#include <fastrtps/rtps/common/SerializedPayload.h>
#include <fastrtps/utils/md5.h>
#include <fastrtps/types/TypeObjectFactory.h>
#include <fastrtps/types/TypeNamesGenerator.h>
#include <fastrtps/types/AnnotationParameterValue.h>
#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>

using namespace eprosima::fastrtps::rtps;

void registercomplex_nested_arraysTypes()
{
    TypeObjectFactory *factory = TypeObjectFactory::get_instance();
    factory->add_type_object("ThirdLevelElement", GetThirdLevelElementIdentifier(true),
    GetThirdLevelElementObject(true));
    factory->add_type_object("ThirdLevelElement", GetThirdLevelElementIdentifier(false),
    GetThirdLevelElementObject(false));

    factory->add_type_object("SecondLevelElement", GetSecondLevelElementIdentifier(true),
    GetSecondLevelElementObject(true));
    factory->add_type_object("SecondLevelElement", GetSecondLevelElementIdentifier(false),
    GetSecondLevelElementObject(false));

    factory->add_type_object("FirstLevelElement", GetFirstLevelElementIdentifier(true),
    GetFirstLevelElementObject(true));
    factory->add_type_object("FirstLevelElement", GetFirstLevelElementIdentifier(false),
    GetFirstLevelElementObject(false));

    factory->add_type_object("complex_nested_arrays", Getcomplex_nested_arraysIdentifier(true),
    Getcomplex_nested_arraysObject(true));
    factory->add_type_object("complex_nested_arrays", Getcomplex_nested_arraysIdentifier(false),
    Getcomplex_nested_arraysObject(false));

}

const TypeIdentifier* GetThirdLevelElementIdentifier(bool complete)
{
    const TypeIdentifier * c_identifier = TypeObjectFactory::get_instance()->get_type_identifier("ThirdLevelElement", complete);
    if (c_identifier != nullptr && (!complete || c_identifier->_d() == EK_COMPLETE))
    {
        return c_identifier;
    }

    GetThirdLevelElementObject(complete); // Generated inside
    return TypeObjectFactory::get_instance()->get_type_identifier("ThirdLevelElement", complete);
}

const TypeObject* GetThirdLevelElementObject(bool complete)
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("ThirdLevelElement", complete);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }
    else if (complete)
    {
        return GetCompleteThirdLevelElementObject();
    }
    //else
    return GetMinimalThirdLevelElementObject();
}

const TypeObject* GetMinimalThirdLevelElementObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("ThirdLevelElement", false);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }

    TypeObject *type_object = new TypeObject();
    type_object->_d(EK_MINIMAL);
    type_object->minimal()._d(TK_STRUCTURE);

    type_object->minimal().struct_type().struct_flags().IS_FINAL(false);
    type_object->minimal().struct_type().struct_flags().IS_APPENDABLE(false);
    type_object->minimal().struct_type().struct_flags().IS_MUTABLE(false);
    type_object->minimal().struct_type().struct_flags().IS_NESTED(false);
    type_object->minimal().struct_type().struct_flags().IS_AUTOID_HASH(false); // Unsupported

    MemberId memberId = 0;
    MinimalStructMember mst_x;
    mst_x.common().member_id(memberId++);
    mst_x.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_x.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_x.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_x.common().member_flags().IS_OPTIONAL(false);
    mst_x.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_x.common().member_flags().IS_KEY(false);
    mst_x.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_x.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("double", false));

    MD5 x_hash("x");
    for(int i = 0; i < 4; ++i)
    {
        mst_x.detail().name_hash()[i] = x_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_x);

    MinimalStructMember mst_y;
    mst_y.common().member_id(memberId++);
    mst_y.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_y.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_y.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_y.common().member_flags().IS_OPTIONAL(false);
    mst_y.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_y.common().member_flags().IS_KEY(false);
    mst_y.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_y.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("double", false));

    MD5 y_hash("y");
    for(int i = 0; i < 4; ++i)
    {
        mst_y.detail().name_hash()[i] = y_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_y);


    // Header
    // TODO Inheritance
    //type_object->minimal().struct_type().header().base_type()._d(EK_MINIMAL);
    //type_object->minimal().struct_type().header().base_type().equivalence_hash()[0..13];

    TypeIdentifier identifier;
    identifier._d(EK_MINIMAL);

    SerializedPayload_t payload(static_cast<uint32_t>(
        MinimalStructType::getCdrSerializedSize(type_object->minimal().struct_type()) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::Cdr::DDS_CDR); // Object that serializes the data.
    payload.encapsulation = CDR_LE;

    type_object->serialize(ser);
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("ThirdLevelElement", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("ThirdLevelElement", false);
}

const TypeObject* GetCompleteThirdLevelElementObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("ThirdLevelElement", true);
    if (c_type_object != nullptr && c_type_object->_d() == EK_COMPLETE)
    {
        return c_type_object;
    }

    TypeObject *type_object = new TypeObject();
    type_object->_d(EK_COMPLETE);
    type_object->complete()._d(TK_STRUCTURE);

    type_object->complete().struct_type().struct_flags().IS_FINAL(false);
    type_object->complete().struct_type().struct_flags().IS_APPENDABLE(false);
    type_object->complete().struct_type().struct_flags().IS_MUTABLE(false);
    type_object->complete().struct_type().struct_flags().IS_NESTED(false);
    type_object->complete().struct_type().struct_flags().IS_AUTOID_HASH(false); // Unsupported

    MemberId memberId = 0;
    CompleteStructMember cst_x;
    cst_x.common().member_id(memberId++);
    cst_x.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_x.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_x.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_x.common().member_flags().IS_OPTIONAL(false);
    cst_x.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_x.common().member_flags().IS_KEY(false);
    cst_x.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_x.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("double", false));

    cst_x.detail().name("x");

    type_object->complete().struct_type().member_seq().emplace_back(cst_x);

    CompleteStructMember cst_y;
    cst_y.common().member_id(memberId++);
    cst_y.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_y.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_y.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_y.common().member_flags().IS_OPTIONAL(false);
    cst_y.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_y.common().member_flags().IS_KEY(false);
    cst_y.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_y.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("double", false));

    cst_y.detail().name("y");

    type_object->complete().struct_type().member_seq().emplace_back(cst_y);


    // Header
    type_object->complete().struct_type().header().detail().type_name("ThirdLevelElement");
    // TODO inheritance


    TypeIdentifier identifier;
    identifier._d(EK_COMPLETE);

    SerializedPayload_t payload(static_cast<uint32_t>(
        CompleteStructType::getCdrSerializedSize(type_object->complete().struct_type()) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::Cdr::DDS_CDR); // Object that serializes the data.
    payload.encapsulation = CDR_LE;

    type_object->serialize(ser);
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("ThirdLevelElement", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("ThirdLevelElement", true);
}

const TypeIdentifier* GetSecondLevelElementIdentifier(bool complete)
{
    const TypeIdentifier * c_identifier = TypeObjectFactory::get_instance()->get_type_identifier("SecondLevelElement", complete);
    if (c_identifier != nullptr && (!complete || c_identifier->_d() == EK_COMPLETE))
    {
        return c_identifier;
    }

    GetSecondLevelElementObject(complete); // Generated inside
    return TypeObjectFactory::get_instance()->get_type_identifier("SecondLevelElement", complete);
}

const TypeObject* GetSecondLevelElementObject(bool complete)
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("SecondLevelElement", complete);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }
    else if (complete)
    {
        return GetCompleteSecondLevelElementObject();
    }
    //else
    return GetMinimalSecondLevelElementObject();
}

const TypeObject* GetMinimalSecondLevelElementObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("SecondLevelElement", false);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }

    TypeObject *type_object = new TypeObject();
    type_object->_d(EK_MINIMAL);
    type_object->minimal()._d(TK_STRUCTURE);

    type_object->minimal().struct_type().struct_flags().IS_FINAL(false);
    type_object->minimal().struct_type().struct_flags().IS_APPENDABLE(false);
    type_object->minimal().struct_type().struct_flags().IS_MUTABLE(false);
    type_object->minimal().struct_type().struct_flags().IS_NESTED(false);
    type_object->minimal().struct_type().struct_flags().IS_AUTOID_HASH(false); // Unsupported

    MemberId memberId = 0;
    MinimalStructMember mst_an_element_alone;
    mst_an_element_alone.common().member_id(memberId++);
    mst_an_element_alone.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_an_element_alone.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_an_element_alone.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_an_element_alone.common().member_flags().IS_OPTIONAL(false);
    mst_an_element_alone.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_an_element_alone.common().member_flags().IS_KEY(false);
    mst_an_element_alone.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_an_element_alone.common().member_type_id(*GetThirdLevelElementIdentifier(false));
    MD5 an_element_alone_hash("an_element_alone");
    for(int i = 0; i < 4; ++i)
    {
        mst_an_element_alone.detail().name_hash()[i] = an_element_alone_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_an_element_alone);

    MinimalStructMember mst_a_limited_other_value;
    mst_a_limited_other_value.common().member_id(memberId++);
    mst_a_limited_other_value.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_a_limited_other_value.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_a_limited_other_value.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_a_limited_other_value.common().member_flags().IS_OPTIONAL(false);
    mst_a_limited_other_value.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_a_limited_other_value.common().member_flags().IS_KEY(false);
    mst_a_limited_other_value.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_a_limited_other_value.common().member_type_id(*TypeObjectFactory::get_instance()->get_sequence_identifier("ThirdLevelElement", 1, false));


    MD5 a_limited_other_value_hash("a_limited_other_value");
    for(int i = 0; i < 4; ++i)
    {
        mst_a_limited_other_value.detail().name_hash()[i] = a_limited_other_value_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_a_limited_other_value);


    // Header
    // TODO Inheritance
    //type_object->minimal().struct_type().header().base_type()._d(EK_MINIMAL);
    //type_object->minimal().struct_type().header().base_type().equivalence_hash()[0..13];

    TypeIdentifier identifier;
    identifier._d(EK_MINIMAL);

    SerializedPayload_t payload(static_cast<uint32_t>(
        MinimalStructType::getCdrSerializedSize(type_object->minimal().struct_type()) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::Cdr::DDS_CDR); // Object that serializes the data.
    payload.encapsulation = CDR_LE;

    type_object->serialize(ser);
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("SecondLevelElement", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("SecondLevelElement", false);
}

const TypeObject* GetCompleteSecondLevelElementObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("SecondLevelElement", true);
    if (c_type_object != nullptr && c_type_object->_d() == EK_COMPLETE)
    {
        return c_type_object;
    }

    TypeObject *type_object = new TypeObject();
    type_object->_d(EK_COMPLETE);
    type_object->complete()._d(TK_STRUCTURE);

    type_object->complete().struct_type().struct_flags().IS_FINAL(false);
    type_object->complete().struct_type().struct_flags().IS_APPENDABLE(false);
    type_object->complete().struct_type().struct_flags().IS_MUTABLE(false);
    type_object->complete().struct_type().struct_flags().IS_NESTED(false);
    type_object->complete().struct_type().struct_flags().IS_AUTOID_HASH(false); // Unsupported

    MemberId memberId = 0;
    CompleteStructMember cst_an_element_alone;
    cst_an_element_alone.common().member_id(memberId++);
    cst_an_element_alone.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_an_element_alone.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_an_element_alone.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_an_element_alone.common().member_flags().IS_OPTIONAL(false);
    cst_an_element_alone.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_an_element_alone.common().member_flags().IS_KEY(false);
    cst_an_element_alone.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_an_element_alone.common().member_type_id(*GetThirdLevelElementIdentifier(true));
    cst_an_element_alone.detail().name("an_element_alone");

    type_object->complete().struct_type().member_seq().emplace_back(cst_an_element_alone);

    CompleteStructMember cst_a_limited_other_value;
    cst_a_limited_other_value.common().member_id(memberId++);
    cst_a_limited_other_value.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_a_limited_other_value.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_a_limited_other_value.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_a_limited_other_value.common().member_flags().IS_OPTIONAL(false);
    cst_a_limited_other_value.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_a_limited_other_value.common().member_flags().IS_KEY(false);
    cst_a_limited_other_value.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_a_limited_other_value.common().member_type_id(*TypeObjectFactory::get_instance()->get_sequence_identifier("ThirdLevelElement", 1, true));


    cst_a_limited_other_value.detail().name("a_limited_other_value");

    type_object->complete().struct_type().member_seq().emplace_back(cst_a_limited_other_value);


    // Header
    type_object->complete().struct_type().header().detail().type_name("SecondLevelElement");
    // TODO inheritance


    TypeIdentifier identifier;
    identifier._d(EK_COMPLETE);

    SerializedPayload_t payload(static_cast<uint32_t>(
        CompleteStructType::getCdrSerializedSize(type_object->complete().struct_type()) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::Cdr::DDS_CDR); // Object that serializes the data.
    payload.encapsulation = CDR_LE;

    type_object->serialize(ser);
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("SecondLevelElement", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("SecondLevelElement", true);
}

const TypeIdentifier* GetFirstLevelElementIdentifier(bool complete)
{
    const TypeIdentifier * c_identifier = TypeObjectFactory::get_instance()->get_type_identifier("FirstLevelElement", complete);
    if (c_identifier != nullptr && (!complete || c_identifier->_d() == EK_COMPLETE))
    {
        return c_identifier;
    }

    GetFirstLevelElementObject(complete); // Generated inside
    return TypeObjectFactory::get_instance()->get_type_identifier("FirstLevelElement", complete);
}

const TypeObject* GetFirstLevelElementObject(bool complete)
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("FirstLevelElement", complete);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }
    else if (complete)
    {
        return GetCompleteFirstLevelElementObject();
    }
    //else
    return GetMinimalFirstLevelElementObject();
}

const TypeObject* GetMinimalFirstLevelElementObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("FirstLevelElement", false);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }

    TypeObject *type_object = new TypeObject();
    type_object->_d(EK_MINIMAL);
    type_object->minimal()._d(TK_STRUCTURE);

    type_object->minimal().struct_type().struct_flags().IS_FINAL(false);
    type_object->minimal().struct_type().struct_flags().IS_APPENDABLE(false);
    type_object->minimal().struct_type().struct_flags().IS_MUTABLE(false);
    type_object->minimal().struct_type().struct_flags().IS_NESTED(false);
    type_object->minimal().struct_type().struct_flags().IS_AUTOID_HASH(false); // Unsupported

    MemberId memberId = 0;
    MinimalStructMember mst_useless_name;
    mst_useless_name.common().member_id(memberId++);
    mst_useless_name.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_useless_name.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_useless_name.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_useless_name.common().member_flags().IS_OPTIONAL(false);
    mst_useless_name.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_useless_name.common().member_flags().IS_KEY(false);
    mst_useless_name.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_useless_name.common().member_type_id(*TypeObjectFactory::get_instance()->get_string_identifier(255, false));


    MD5 useless_name_hash("useless_name");
    for(int i = 0; i < 4; ++i)
    {
        mst_useless_name.detail().name_hash()[i] = useless_name_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_useless_name);

    MinimalStructMember mst_sub;
    mst_sub.common().member_id(memberId++);
    mst_sub.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_sub.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_sub.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_sub.common().member_flags().IS_OPTIONAL(false);
    mst_sub.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_sub.common().member_flags().IS_KEY(false);
    mst_sub.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_sub.common().member_type_id(*TypeObjectFactory::get_instance()->get_sequence_identifier("SecondLevelElement", 0, false));


    MD5 sub_hash("sub");
    for(int i = 0; i < 4; ++i)
    {
        mst_sub.detail().name_hash()[i] = sub_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_sub);

    MinimalStructMember mst_an_element_alone;
    mst_an_element_alone.common().member_id(memberId++);
    mst_an_element_alone.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_an_element_alone.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_an_element_alone.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_an_element_alone.common().member_flags().IS_OPTIONAL(false);
    mst_an_element_alone.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_an_element_alone.common().member_flags().IS_KEY(false);
    mst_an_element_alone.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_an_element_alone.common().member_type_id(*GetThirdLevelElementIdentifier(false));
    MD5 an_element_alone_hash("an_element_alone");
    for(int i = 0; i < 4; ++i)
    {
        mst_an_element_alone.detail().name_hash()[i] = an_element_alone_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_an_element_alone);


    // Header
    // TODO Inheritance
    //type_object->minimal().struct_type().header().base_type()._d(EK_MINIMAL);
    //type_object->minimal().struct_type().header().base_type().equivalence_hash()[0..13];

    TypeIdentifier identifier;
    identifier._d(EK_MINIMAL);

    SerializedPayload_t payload(static_cast<uint32_t>(
        MinimalStructType::getCdrSerializedSize(type_object->minimal().struct_type()) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::Cdr::DDS_CDR); // Object that serializes the data.
    payload.encapsulation = CDR_LE;

    type_object->serialize(ser);
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("FirstLevelElement", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("FirstLevelElement", false);
}

const TypeObject* GetCompleteFirstLevelElementObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("FirstLevelElement", true);
    if (c_type_object != nullptr && c_type_object->_d() == EK_COMPLETE)
    {
        return c_type_object;
    }

    TypeObject *type_object = new TypeObject();
    type_object->_d(EK_COMPLETE);
    type_object->complete()._d(TK_STRUCTURE);

    type_object->complete().struct_type().struct_flags().IS_FINAL(false);
    type_object->complete().struct_type().struct_flags().IS_APPENDABLE(false);
    type_object->complete().struct_type().struct_flags().IS_MUTABLE(false);
    type_object->complete().struct_type().struct_flags().IS_NESTED(false);
    type_object->complete().struct_type().struct_flags().IS_AUTOID_HASH(false); // Unsupported

    MemberId memberId = 0;
    CompleteStructMember cst_useless_name;
    cst_useless_name.common().member_id(memberId++);
    cst_useless_name.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_useless_name.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_useless_name.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_useless_name.common().member_flags().IS_OPTIONAL(false);
    cst_useless_name.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_useless_name.common().member_flags().IS_KEY(false);
    cst_useless_name.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_useless_name.common().member_type_id(*TypeObjectFactory::get_instance()->get_string_identifier(255, false));


    cst_useless_name.detail().name("useless_name");

    type_object->complete().struct_type().member_seq().emplace_back(cst_useless_name);

    CompleteStructMember cst_sub;
    cst_sub.common().member_id(memberId++);
    cst_sub.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_sub.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_sub.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_sub.common().member_flags().IS_OPTIONAL(false);
    cst_sub.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_sub.common().member_flags().IS_KEY(false);
    cst_sub.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_sub.common().member_type_id(*TypeObjectFactory::get_instance()->get_sequence_identifier("SecondLevelElement", 0, true));


    cst_sub.detail().name("sub");

    type_object->complete().struct_type().member_seq().emplace_back(cst_sub);

    CompleteStructMember cst_an_element_alone;
    cst_an_element_alone.common().member_id(memberId++);
    cst_an_element_alone.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_an_element_alone.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_an_element_alone.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_an_element_alone.common().member_flags().IS_OPTIONAL(false);
    cst_an_element_alone.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_an_element_alone.common().member_flags().IS_KEY(false);
    cst_an_element_alone.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_an_element_alone.common().member_type_id(*GetThirdLevelElementIdentifier(true));
    cst_an_element_alone.detail().name("an_element_alone");

    type_object->complete().struct_type().member_seq().emplace_back(cst_an_element_alone);


    // Header
    type_object->complete().struct_type().header().detail().type_name("FirstLevelElement");
    // TODO inheritance


    TypeIdentifier identifier;
    identifier._d(EK_COMPLETE);

    SerializedPayload_t payload(static_cast<uint32_t>(
        CompleteStructType::getCdrSerializedSize(type_object->complete().struct_type()) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::Cdr::DDS_CDR); // Object that serializes the data.
    payload.encapsulation = CDR_LE;

    type_object->serialize(ser);
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("FirstLevelElement", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("FirstLevelElement", true);
}

const TypeIdentifier* Getcomplex_nested_arraysIdentifier(bool complete)
{
    const TypeIdentifier * c_identifier = TypeObjectFactory::get_instance()->get_type_identifier("complex_nested_arrays", complete);
    if (c_identifier != nullptr && (!complete || c_identifier->_d() == EK_COMPLETE))
    {
        return c_identifier;
    }

    Getcomplex_nested_arraysObject(complete); // Generated inside
    return TypeObjectFactory::get_instance()->get_type_identifier("complex_nested_arrays", complete);
}

const TypeObject* Getcomplex_nested_arraysObject(bool complete)
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("complex_nested_arrays", complete);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }
    else if (complete)
    {
        return GetCompletecomplex_nested_arraysObject();
    }
    //else
    return GetMinimalcomplex_nested_arraysObject();
}

const TypeObject* GetMinimalcomplex_nested_arraysObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("complex_nested_arrays", false);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }

    TypeObject *type_object = new TypeObject();
    type_object->_d(EK_MINIMAL);
    type_object->minimal()._d(TK_STRUCTURE);

    type_object->minimal().struct_type().struct_flags().IS_FINAL(false);
    type_object->minimal().struct_type().struct_flags().IS_APPENDABLE(false);
    type_object->minimal().struct_type().struct_flags().IS_MUTABLE(false);
    type_object->minimal().struct_type().struct_flags().IS_NESTED(false);
    type_object->minimal().struct_type().struct_flags().IS_AUTOID_HASH(false); // Unsupported

    MemberId memberId = 0;
    MinimalStructMember mst_array_of_elements;
    mst_array_of_elements.common().member_id(memberId++);
    mst_array_of_elements.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_array_of_elements.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_array_of_elements.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_array_of_elements.common().member_flags().IS_OPTIONAL(false);
    mst_array_of_elements.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_array_of_elements.common().member_flags().IS_KEY(false);
    mst_array_of_elements.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_array_of_elements.common().member_type_id(*TypeObjectFactory::get_instance()->get_array_identifier("FirstLevelElement", {3}, false));


    MD5 array_of_elements_hash("array_of_elements");
    for(int i = 0; i < 4; ++i)
    {
        mst_array_of_elements.detail().name_hash()[i] = array_of_elements_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_array_of_elements);


    // Header
    // TODO Inheritance
    //type_object->minimal().struct_type().header().base_type()._d(EK_MINIMAL);
    //type_object->minimal().struct_type().header().base_type().equivalence_hash()[0..13];

    TypeIdentifier identifier;
    identifier._d(EK_MINIMAL);

    SerializedPayload_t payload(static_cast<uint32_t>(
        MinimalStructType::getCdrSerializedSize(type_object->minimal().struct_type()) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::Cdr::DDS_CDR); // Object that serializes the data.
    payload.encapsulation = CDR_LE;

    type_object->serialize(ser);
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("complex_nested_arrays", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("complex_nested_arrays", false);
}

const TypeObject* GetCompletecomplex_nested_arraysObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("complex_nested_arrays", true);
    if (c_type_object != nullptr && c_type_object->_d() == EK_COMPLETE)
    {
        return c_type_object;
    }

    TypeObject *type_object = new TypeObject();
    type_object->_d(EK_COMPLETE);
    type_object->complete()._d(TK_STRUCTURE);

    type_object->complete().struct_type().struct_flags().IS_FINAL(false);
    type_object->complete().struct_type().struct_flags().IS_APPENDABLE(false);
    type_object->complete().struct_type().struct_flags().IS_MUTABLE(false);
    type_object->complete().struct_type().struct_flags().IS_NESTED(false);
    type_object->complete().struct_type().struct_flags().IS_AUTOID_HASH(false); // Unsupported

    MemberId memberId = 0;
    CompleteStructMember cst_array_of_elements;
    cst_array_of_elements.common().member_id(memberId++);
    cst_array_of_elements.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_array_of_elements.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_array_of_elements.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_array_of_elements.common().member_flags().IS_OPTIONAL(false);
    cst_array_of_elements.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_array_of_elements.common().member_flags().IS_KEY(false);
    cst_array_of_elements.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_array_of_elements.common().member_type_id(*TypeObjectFactory::get_instance()->get_array_identifier("FirstLevelElement", {3}, true));


    cst_array_of_elements.detail().name("array_of_elements");

    type_object->complete().struct_type().member_seq().emplace_back(cst_array_of_elements);


    // Header
    type_object->complete().struct_type().header().detail().type_name("complex_nested_arrays");
    // TODO inheritance


    TypeIdentifier identifier;
    identifier._d(EK_COMPLETE);

    SerializedPayload_t payload(static_cast<uint32_t>(
        CompleteStructType::getCdrSerializedSize(type_object->complete().struct_type()) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::Cdr::DDS_CDR); // Object that serializes the data.
    payload.encapsulation = CDR_LE;

    type_object->serialize(ser);
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("complex_nested_arrays", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("complex_nested_arrays", true);
}