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
 * @file LogEntryTypeObject.cpp
 * This source file contains the definition of the described types in the IDL file.
 *
 * This file was generated by the tool fastddsgen.
 */

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace { char dummy; }
#endif

#include <ddspipe_core/types/logging/v2/LogEntry.h>
#include <ddspipe_core/types/logging/v2/LogEntryTypeObject.h>
#include <mutex>
#include <utility>
#include <sstream>
#include <fastdds/rtps/common/CdrSerialization.hpp>
#include <fastrtps/rtps/common/SerializedPayload.h>
#include <fastrtps/utils/md5.h>
#include <fastrtps/types/TypeObjectFactory.h>
#include <fastrtps/types/TypeNamesGenerator.h>
#include <fastrtps/types/AnnotationParameterValue.h>

using namespace eprosima::fastrtps::rtps;

void registerLogEntryTypes()
{
    static std::once_flag once_flag;
    std::call_once(once_flag, []()
            {
                TypeObjectFactory *factory = TypeObjectFactory::get_instance();
                factory->add_type_object("Kind", GetKindIdentifier(true),
                GetKindObject(true));
                factory->add_type_object("Kind", GetKindIdentifier(false),
                GetKindObject(false));


                factory->add_type_object("LogEntry", GetLogEntryIdentifier(true),
                GetLogEntryObject(true));
                factory->add_type_object("LogEntry", GetLogEntryIdentifier(false),
                GetLogEntryObject(false));

            });
}









const TypeIdentifier* GetKindIdentifier(bool complete)
{
    const TypeIdentifier* c_identifier = TypeObjectFactory::get_instance()->get_type_identifier("Kind", complete);
    if (c_identifier != nullptr && (!complete || c_identifier->_d() == EK_COMPLETE))
    {
        return c_identifier;
    }

    GetKindObject(complete); // Generated inside
    return TypeObjectFactory::get_instance()->get_type_identifier("Kind", complete);
}

const TypeObject* GetKindObject(bool complete)
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("Kind", complete);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }
    else if (complete)
    {
        return GetCompleteKindObject();
    }
    // else
    return GetMinimalKindObject();
}

const TypeObject* GetMinimalKindObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("Kind", false);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }

    TypeObject *type_object = new TypeObject();
    type_object->_d(EK_MINIMAL);
    type_object->minimal()._d(TK_ENUM);

    // No flags apply
    //type_object->minimal().enumerated_type().enum_flags().IS_FINAL(false);
    //type_object->minimal().enumerated_type().enum_flags().IS_APPENDABLE(false);
    //type_object->minimal().enumerated_type().enum_flags().IS_MUTABLE(false);
    //type_object->minimal().enumerated_type().enum_flags().IS_NESTED(false);
    //type_object->minimal().enumerated_type().enum_flags().IS_AUTOID_HASH(false);

    type_object->minimal().enumerated_type().header().common().bit_bound(32); // TODO fixed by IDL, isn't?

    uint32_t value = 0;
    MinimalEnumeratedLiteral mel_Info;
    mel_Info.common().flags().TRY_CONSTRUCT1(false); // Doesn't apply
    mel_Info.common().flags().TRY_CONSTRUCT2(false); // Doesn't apply
    mel_Info.common().flags().IS_EXTERNAL(false); // Doesn't apply
    mel_Info.common().flags().IS_OPTIONAL(false); // Doesn't apply
    mel_Info.common().flags().IS_MUST_UNDERSTAND(false); // Doesn't apply
    mel_Info.common().flags().IS_KEY(false); // Doesn't apply
    mel_Info.common().flags().IS_DEFAULT(false);
    mel_Info.common().value(value++);
    MD5 Info_hash("Info");
    for(int i = 0; i < 4; ++i)
    {
        mel_Info.detail().name_hash()[i] = Info_hash.digest[i];
    }
    type_object->minimal().enumerated_type().literal_seq().emplace_back(mel_Info);

    MinimalEnumeratedLiteral mel_Warning;
    mel_Warning.common().flags().TRY_CONSTRUCT1(false); // Doesn't apply
    mel_Warning.common().flags().TRY_CONSTRUCT2(false); // Doesn't apply
    mel_Warning.common().flags().IS_EXTERNAL(false); // Doesn't apply
    mel_Warning.common().flags().IS_OPTIONAL(false); // Doesn't apply
    mel_Warning.common().flags().IS_MUST_UNDERSTAND(false); // Doesn't apply
    mel_Warning.common().flags().IS_KEY(false); // Doesn't apply
    mel_Warning.common().flags().IS_DEFAULT(false);
    mel_Warning.common().value(value++);
    MD5 Warning_hash("Warning");
    for(int i = 0; i < 4; ++i)
    {
        mel_Warning.detail().name_hash()[i] = Warning_hash.digest[i];
    }
    type_object->minimal().enumerated_type().literal_seq().emplace_back(mel_Warning);

    MinimalEnumeratedLiteral mel_Error;
    mel_Error.common().flags().TRY_CONSTRUCT1(false); // Doesn't apply
    mel_Error.common().flags().TRY_CONSTRUCT2(false); // Doesn't apply
    mel_Error.common().flags().IS_EXTERNAL(false); // Doesn't apply
    mel_Error.common().flags().IS_OPTIONAL(false); // Doesn't apply
    mel_Error.common().flags().IS_MUST_UNDERSTAND(false); // Doesn't apply
    mel_Error.common().flags().IS_KEY(false); // Doesn't apply
    mel_Error.common().flags().IS_DEFAULT(false);
    mel_Error.common().value(value++);
    MD5 Error_hash("Error");
    for(int i = 0; i < 4; ++i)
    {
        mel_Error.detail().name_hash()[i] = Error_hash.digest[i];
    }
    type_object->minimal().enumerated_type().literal_seq().emplace_back(mel_Error);


    TypeIdentifier identifier;
    identifier._d(EK_MINIMAL);

    eprosima::fastcdr::CdrSizeCalculator calculator(eprosima::fastcdr::CdrVersion::XCDRv1);
    size_t current_alignment {0};
    SerializedPayload_t payload(static_cast<uint32_t>(
        calculator.calculate_serialized_size(type_object->minimal().enumerated_type(), current_alignment) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::CdrVersion::XCDRv1); // Object that serializes the data.
    payload.encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

    ser << *type_object;
#if FASTCDR_VERSION_MAJOR == 1
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
#else
    payload.length = (uint32_t)ser.get_serialized_data_length(); //Get the serialized length
#endif // FASTCDR_VERSION_MAJOR == 1
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("Kind", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("Kind", false);
}

const TypeObject* GetCompleteKindObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("Kind", true);
    if (c_type_object != nullptr && c_type_object->_d() == EK_COMPLETE)
    {
        return c_type_object;
    }

    TypeObject *type_object = new TypeObject();
    type_object->_d(EK_COMPLETE);
    type_object->complete()._d(TK_ENUM);

    // No flags apply
    //type_object->complete().enumerated_type().enum_flags().IS_FINAL(false);
    //type_object->complete().enumerated_type().enum_flags().IS_APPENDABLE(false);
    //type_object->complete().enumerated_type().enum_flags().IS_MUTABLE(false);
    //type_object->complete().enumerated_type().enum_flags().IS_NESTED(false);
    //type_object->complete().enumerated_type().enum_flags().IS_AUTOID_HASH(false);

    type_object->complete().enumerated_type().header().common().bit_bound(32); // TODO fixed by IDL, isn't?
    type_object->complete().enumerated_type().header().detail().type_name("Kind");

    uint32_t value = 0;
    CompleteEnumeratedLiteral cel_Info;
    cel_Info.common().flags().TRY_CONSTRUCT1(false); // Doesn't apply
    cel_Info.common().flags().TRY_CONSTRUCT2(false); // Doesn't apply
    cel_Info.common().flags().IS_EXTERNAL(false); // Doesn't apply
    cel_Info.common().flags().IS_OPTIONAL(false); // Doesn't apply
    cel_Info.common().flags().IS_MUST_UNDERSTAND(false); // Doesn't apply
    cel_Info.common().flags().IS_KEY(false); // Doesn't apply
    cel_Info.common().flags().IS_DEFAULT(false);
    cel_Info.common().value(value++);
    cel_Info.detail().name("Info");

    type_object->complete().enumerated_type().literal_seq().emplace_back(cel_Info);

    CompleteEnumeratedLiteral cel_Warning;
    cel_Warning.common().flags().TRY_CONSTRUCT1(false); // Doesn't apply
    cel_Warning.common().flags().TRY_CONSTRUCT2(false); // Doesn't apply
    cel_Warning.common().flags().IS_EXTERNAL(false); // Doesn't apply
    cel_Warning.common().flags().IS_OPTIONAL(false); // Doesn't apply
    cel_Warning.common().flags().IS_MUST_UNDERSTAND(false); // Doesn't apply
    cel_Warning.common().flags().IS_KEY(false); // Doesn't apply
    cel_Warning.common().flags().IS_DEFAULT(false);
    cel_Warning.common().value(value++);
    cel_Warning.detail().name("Warning");

    type_object->complete().enumerated_type().literal_seq().emplace_back(cel_Warning);

    CompleteEnumeratedLiteral cel_Error;
    cel_Error.common().flags().TRY_CONSTRUCT1(false); // Doesn't apply
    cel_Error.common().flags().TRY_CONSTRUCT2(false); // Doesn't apply
    cel_Error.common().flags().IS_EXTERNAL(false); // Doesn't apply
    cel_Error.common().flags().IS_OPTIONAL(false); // Doesn't apply
    cel_Error.common().flags().IS_MUST_UNDERSTAND(false); // Doesn't apply
    cel_Error.common().flags().IS_KEY(false); // Doesn't apply
    cel_Error.common().flags().IS_DEFAULT(false);
    cel_Error.common().value(value++);
    cel_Error.detail().name("Error");

    type_object->complete().enumerated_type().literal_seq().emplace_back(cel_Error);


    TypeIdentifier identifier;
    identifier._d(EK_COMPLETE);

    eprosima::fastcdr::CdrSizeCalculator calculator(eprosima::fastcdr::CdrVersion::XCDRv1);
    size_t current_alignment {0};
    SerializedPayload_t payload(static_cast<uint32_t>(
        calculator.calculate_serialized_size(type_object->complete().enumerated_type(), current_alignment) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::CdrVersion::XCDRv1); // Object that serializes the data.
    payload.encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

    ser << *type_object;
#if FASTCDR_VERSION_MAJOR == 1
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
#else
    payload.length = (uint32_t)ser.get_serialized_data_length(); //Get the serialized length
#endif // FASTCDR_VERSION_MAJOR == 1
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("Kind", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("Kind", true);
}



const TypeIdentifier* GetLogEntryIdentifier(bool complete)
{
    const TypeIdentifier * c_identifier = TypeObjectFactory::get_instance()->get_type_identifier("LogEntry", complete);
    if (c_identifier != nullptr && (!complete || c_identifier->_d() == EK_COMPLETE))
    {
        return c_identifier;
    }

    GetLogEntryObject(complete); // Generated inside
    return TypeObjectFactory::get_instance()->get_type_identifier("LogEntry", complete);
}

const TypeObject* GetLogEntryObject(bool complete)
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("LogEntry", complete);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }
    else if (complete)
    {
        return GetCompleteLogEntryObject();
    }
    //else
    return GetMinimalLogEntryObject();
}

const TypeObject* GetMinimalLogEntryObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("LogEntry", false);
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
    MinimalStructMember mst_event;
    mst_event.common().member_id(memberId++);
    mst_event.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_event.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_event.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_event.common().member_flags().IS_OPTIONAL(false);
    mst_event.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_event.common().member_flags().IS_KEY(true);
    mst_event.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_event.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("int32_t", false));


    MD5 event_hash("event");
    for(int i = 0; i < 4; ++i)
    {
        mst_event.detail().name_hash()[i] = event_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_event);

    MinimalStructMember mst_kind;
    mst_kind.common().member_id(memberId++);
    mst_kind.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_kind.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_kind.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_kind.common().member_flags().IS_OPTIONAL(false);
    mst_kind.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_kind.common().member_flags().IS_KEY(false);
    mst_kind.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_kind.common().member_type_id(*GetKindIdentifier(false));

    MD5 kind_hash("kind");
    for(int i = 0; i < 4; ++i)
    {
        mst_kind.detail().name_hash()[i] = kind_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_kind);

    MinimalStructMember mst_category;
    mst_category.common().member_id(memberId++);
    mst_category.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_category.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_category.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_category.common().member_flags().IS_OPTIONAL(false);
    mst_category.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_category.common().member_flags().IS_KEY(false);
    mst_category.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_category.common().member_type_id(*TypeObjectFactory::get_instance()->get_string_identifier(255, false));

    MD5 category_hash("category");
    for(int i = 0; i < 4; ++i)
    {
        mst_category.detail().name_hash()[i] = category_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_category);

    MinimalStructMember mst_message;
    mst_message.common().member_id(memberId++);
    mst_message.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_message.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_message.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_message.common().member_flags().IS_OPTIONAL(false);
    mst_message.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_message.common().member_flags().IS_KEY(false);
    mst_message.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_message.common().member_type_id(*TypeObjectFactory::get_instance()->get_string_identifier(255, false));

    MD5 message_hash("message");
    for(int i = 0; i < 4; ++i)
    {
        mst_message.detail().name_hash()[i] = message_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_message);

    MinimalStructMember mst_timestamp;
    mst_timestamp.common().member_id(memberId++);
    mst_timestamp.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_timestamp.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_timestamp.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_timestamp.common().member_flags().IS_OPTIONAL(false);
    mst_timestamp.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_timestamp.common().member_flags().IS_KEY(false);
    mst_timestamp.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_timestamp.common().member_type_id(*TypeObjectFactory::get_instance()->get_string_identifier(255, false));

    MD5 timestamp_hash("timestamp");
    for(int i = 0; i < 4; ++i)
    {
        mst_timestamp.detail().name_hash()[i] = timestamp_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_timestamp);


    // Header
    // TODO Inheritance
    //type_object->minimal().struct_type().header().base_type()._d(EK_MINIMAL);
    //type_object->minimal().struct_type().header().base_type().equivalence_hash()[0..13];
    TypeIdentifier identifier;
    identifier._d(EK_MINIMAL);

    eprosima::fastcdr::CdrSizeCalculator calculator(eprosima::fastcdr::CdrVersion::XCDRv1);
    size_t current_alignment {0};
    SerializedPayload_t payload(static_cast<uint32_t>(
        calculator.calculate_serialized_size(type_object->minimal().struct_type(), current_alignment) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::CdrVersion::XCDRv1); // Object that serializes the data.
    payload.encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

    ser << *type_object;
#if FASTCDR_VERSION_MAJOR == 1
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
#else
    payload.length = (uint32_t)ser.get_serialized_data_length(); //Get the serialized length
#endif // FASTCDR_VERSION_MAJOR == 1
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("LogEntry", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("LogEntry", false);
}

const TypeObject* GetCompleteLogEntryObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("LogEntry", true);
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
    CompleteStructMember cst_event;
    cst_event.common().member_id(memberId++);
    cst_event.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_event.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_event.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_event.common().member_flags().IS_OPTIONAL(false);
    cst_event.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_event.common().member_flags().IS_KEY(true);
    cst_event.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_event.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("int32_t", false));


    cst_event.detail().name("event");

    {
        AppliedAnnotation ann;
        //ann.annotation_typeid(GetkeyIdentifier(true));
        ann.annotation_typeid(*TypeObjectFactory::get_instance()->get_type_identifier_trying_complete("key"));
            {
                AppliedAnnotationParameter annParam;
                MD5 message_hash("value");
                for(int i = 0; i < 4; ++i)
                {
                    annParam.paramname_hash()[i] = message_hash.digest[i];
                }
                AnnotationParameterValue paramValue;
                paramValue._d(TK_BOOLEAN);
                paramValue.from_string("true");
                annParam.value(paramValue);
                ann.param_seq().push_back(annParam);
            }

        cst_event.detail().ann_custom().push_back(ann);
    }


    type_object->complete().struct_type().member_seq().emplace_back(cst_event);

    CompleteStructMember cst_kind;
    cst_kind.common().member_id(memberId++);
    cst_kind.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_kind.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_kind.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_kind.common().member_flags().IS_OPTIONAL(false);
    cst_kind.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_kind.common().member_flags().IS_KEY(false);
    cst_kind.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_kind.common().member_type_id(*GetKindIdentifier(true));

    cst_kind.detail().name("kind");

    type_object->complete().struct_type().member_seq().emplace_back(cst_kind);

    CompleteStructMember cst_category;
    cst_category.common().member_id(memberId++);
    cst_category.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_category.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_category.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_category.common().member_flags().IS_OPTIONAL(false);
    cst_category.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_category.common().member_flags().IS_KEY(false);
    cst_category.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_category.common().member_type_id(*TypeObjectFactory::get_instance()->get_string_identifier(255, false));

    cst_category.detail().name("category");

    type_object->complete().struct_type().member_seq().emplace_back(cst_category);

    CompleteStructMember cst_message;
    cst_message.common().member_id(memberId++);
    cst_message.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_message.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_message.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_message.common().member_flags().IS_OPTIONAL(false);
    cst_message.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_message.common().member_flags().IS_KEY(false);
    cst_message.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_message.common().member_type_id(*TypeObjectFactory::get_instance()->get_string_identifier(255, false));

    cst_message.detail().name("message");

    type_object->complete().struct_type().member_seq().emplace_back(cst_message);

    CompleteStructMember cst_timestamp;
    cst_timestamp.common().member_id(memberId++);
    cst_timestamp.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_timestamp.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_timestamp.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_timestamp.common().member_flags().IS_OPTIONAL(false);
    cst_timestamp.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_timestamp.common().member_flags().IS_KEY(false);
    cst_timestamp.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_timestamp.common().member_type_id(*TypeObjectFactory::get_instance()->get_string_identifier(255, false));

    cst_timestamp.detail().name("timestamp");

    type_object->complete().struct_type().member_seq().emplace_back(cst_timestamp);


    // Header
    type_object->complete().struct_type().header().detail().type_name("LogEntry");
    // TODO inheritance
    TypeIdentifier identifier;
    identifier._d(EK_COMPLETE);

    eprosima::fastcdr::CdrSizeCalculator calculator(eprosima::fastcdr::CdrVersion::XCDRv1);
    size_t current_alignment {0};
    SerializedPayload_t payload(static_cast<uint32_t>(
        calculator.calculate_serialized_size(type_object->complete().struct_type(), current_alignment) + 4));
    eprosima::fastcdr::FastBuffer fastbuffer((char*) payload.data, payload.max_size);
    // Fixed endian (Page 221, EquivalenceHash definition of Extensible and Dynamic Topic Types for DDS document)
    eprosima::fastcdr::Cdr ser(
        fastbuffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS,
        eprosima::fastcdr::CdrVersion::XCDRv1); // Object that serializes the data.
    payload.encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

    ser << *type_object;
#if FASTCDR_VERSION_MAJOR == 1
    payload.length = (uint32_t)ser.getSerializedDataLength(); //Get the serialized length
#else
    payload.length = (uint32_t)ser.get_serialized_data_length(); //Get the serialized length
#endif // FASTCDR_VERSION_MAJOR == 1
    MD5 objectHash;
    objectHash.update((char*)payload.data, payload.length);
    objectHash.finalize();
    for(int i = 0; i < 14; ++i)
    {
        identifier.equivalence_hash()[i] = objectHash.digest[i];
    }

    TypeObjectFactory::get_instance()->add_type_object("LogEntry", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("LogEntry", true);
}
