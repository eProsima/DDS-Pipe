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
 * @file MonitoringStatusTypeObject.cpp
 * This source file contains the definition of the described types in the IDL file.
 *
 * This file was generated by the tool fastddsgen.
 */

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace { char dummy; }
#endif

#include <ddspipe_core/types/monitoring/status/v2/MonitoringStatus.h>
#include <ddspipe_core/types/monitoring/status/v2/MonitoringStatusTypeObject.h>
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

void registerMonitoringStatusTypes()
{
    static std::once_flag once_flag;
    std::call_once(once_flag, []()
            {
                TypeObjectFactory *factory = TypeObjectFactory::get_instance();
                factory->add_type_object("MonitoringErrorStatus", GetMonitoringErrorStatusIdentifier(true),
                GetMonitoringErrorStatusObject(true));
                factory->add_type_object("MonitoringErrorStatus", GetMonitoringErrorStatusIdentifier(false),
                GetMonitoringErrorStatusObject(false));


                factory->add_type_object("MonitoringStatus", GetMonitoringStatusIdentifier(true),
                GetMonitoringStatusObject(true));
                factory->add_type_object("MonitoringStatus", GetMonitoringStatusIdentifier(false),
                GetMonitoringStatusObject(false));

            });
}



const TypeIdentifier* GetMonitoringErrorStatusIdentifier(bool complete)
{
    const TypeIdentifier * c_identifier = TypeObjectFactory::get_instance()->get_type_identifier("MonitoringErrorStatus", complete);
    if (c_identifier != nullptr && (!complete || c_identifier->_d() == EK_COMPLETE))
    {
        return c_identifier;
    }

    GetMonitoringErrorStatusObject(complete); // Generated inside
    return TypeObjectFactory::get_instance()->get_type_identifier("MonitoringErrorStatus", complete);
}

const TypeObject* GetMonitoringErrorStatusObject(bool complete)
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("MonitoringErrorStatus", complete);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }
    else if (complete)
    {
        return GetCompleteMonitoringErrorStatusObject();
    }
    //else
    return GetMinimalMonitoringErrorStatusObject();
}

const TypeObject* GetMinimalMonitoringErrorStatusObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("MonitoringErrorStatus", false);
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
    MinimalStructMember mst_type_mismatch;
    mst_type_mismatch.common().member_id(memberId++);
    mst_type_mismatch.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_type_mismatch.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_type_mismatch.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_type_mismatch.common().member_flags().IS_OPTIONAL(false);
    mst_type_mismatch.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_type_mismatch.common().member_flags().IS_KEY(false);
    mst_type_mismatch.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_type_mismatch.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("bool", false));


    MD5 type_mismatch_hash("type_mismatch");
    for(int i = 0; i < 4; ++i)
    {
        mst_type_mismatch.detail().name_hash()[i] = type_mismatch_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_type_mismatch);

    MinimalStructMember mst_qos_mismatch;
    mst_qos_mismatch.common().member_id(memberId++);
    mst_qos_mismatch.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_qos_mismatch.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_qos_mismatch.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_qos_mismatch.common().member_flags().IS_OPTIONAL(false);
    mst_qos_mismatch.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_qos_mismatch.common().member_flags().IS_KEY(false);
    mst_qos_mismatch.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_qos_mismatch.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("bool", false));


    MD5 qos_mismatch_hash("qos_mismatch");
    for(int i = 0; i < 4; ++i)
    {
        mst_qos_mismatch.detail().name_hash()[i] = qos_mismatch_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_qos_mismatch);


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

    TypeObjectFactory::get_instance()->add_type_object("MonitoringErrorStatus", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("MonitoringErrorStatus", false);
}

const TypeObject* GetCompleteMonitoringErrorStatusObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("MonitoringErrorStatus", true);
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
    CompleteStructMember cst_type_mismatch;
    cst_type_mismatch.common().member_id(memberId++);
    cst_type_mismatch.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_type_mismatch.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_type_mismatch.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_type_mismatch.common().member_flags().IS_OPTIONAL(false);
    cst_type_mismatch.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_type_mismatch.common().member_flags().IS_KEY(false);
    cst_type_mismatch.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_type_mismatch.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("bool", false));


    cst_type_mismatch.detail().name("type_mismatch");

    type_object->complete().struct_type().member_seq().emplace_back(cst_type_mismatch);

    CompleteStructMember cst_qos_mismatch;
    cst_qos_mismatch.common().member_id(memberId++);
    cst_qos_mismatch.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_qos_mismatch.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_qos_mismatch.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_qos_mismatch.common().member_flags().IS_OPTIONAL(false);
    cst_qos_mismatch.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_qos_mismatch.common().member_flags().IS_KEY(false);
    cst_qos_mismatch.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_qos_mismatch.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("bool", false));


    cst_qos_mismatch.detail().name("qos_mismatch");

    type_object->complete().struct_type().member_seq().emplace_back(cst_qos_mismatch);


    // Header
    type_object->complete().struct_type().header().detail().type_name("MonitoringErrorStatus");
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

    TypeObjectFactory::get_instance()->add_type_object("MonitoringErrorStatus", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("MonitoringErrorStatus", true);
}



const TypeIdentifier* GetMonitoringStatusIdentifier(bool complete)
{
    const TypeIdentifier * c_identifier = TypeObjectFactory::get_instance()->get_type_identifier("MonitoringStatus", complete);
    if (c_identifier != nullptr && (!complete || c_identifier->_d() == EK_COMPLETE))
    {
        return c_identifier;
    }

    GetMonitoringStatusObject(complete); // Generated inside
    return TypeObjectFactory::get_instance()->get_type_identifier("MonitoringStatus", complete);
}

const TypeObject* GetMonitoringStatusObject(bool complete)
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("MonitoringStatus", complete);
    if (c_type_object != nullptr)
    {
        return c_type_object;
    }
    else if (complete)
    {
        return GetCompleteMonitoringStatusObject();
    }
    //else
    return GetMinimalMonitoringStatusObject();
}

const TypeObject* GetMinimalMonitoringStatusObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("MonitoringStatus", false);
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
    MinimalStructMember mst_error_status;
    mst_error_status.common().member_id(memberId++);
    mst_error_status.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_error_status.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_error_status.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_error_status.common().member_flags().IS_OPTIONAL(false);
    mst_error_status.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_error_status.common().member_flags().IS_KEY(false);
    mst_error_status.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_error_status.common().member_type_id(*GetMonitoringErrorStatusIdentifier(false));

    MD5 error_status_hash("error_status");
    for(int i = 0; i < 4; ++i)
    {
        mst_error_status.detail().name_hash()[i] = error_status_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_error_status);

    MinimalStructMember mst_has_errors;
    mst_has_errors.common().member_id(memberId++);
    mst_has_errors.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    mst_has_errors.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    mst_has_errors.common().member_flags().IS_EXTERNAL(false); // Unsupported
    mst_has_errors.common().member_flags().IS_OPTIONAL(false);
    mst_has_errors.common().member_flags().IS_MUST_UNDERSTAND(false);
    mst_has_errors.common().member_flags().IS_KEY(false);
    mst_has_errors.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    mst_has_errors.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("bool", false));


    MD5 has_errors_hash("has_errors");
    for(int i = 0; i < 4; ++i)
    {
        mst_has_errors.detail().name_hash()[i] = has_errors_hash.digest[i];
    }
    type_object->minimal().struct_type().member_seq().emplace_back(mst_has_errors);


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

    TypeObjectFactory::get_instance()->add_type_object("MonitoringStatus", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("MonitoringStatus", false);
}

const TypeObject* GetCompleteMonitoringStatusObject()
{
    const TypeObject* c_type_object = TypeObjectFactory::get_instance()->get_type_object("MonitoringStatus", true);
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
    CompleteStructMember cst_error_status;
    cst_error_status.common().member_id(memberId++);
    cst_error_status.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_error_status.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_error_status.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_error_status.common().member_flags().IS_OPTIONAL(false);
    cst_error_status.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_error_status.common().member_flags().IS_KEY(false);
    cst_error_status.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_error_status.common().member_type_id(*GetMonitoringErrorStatusIdentifier(true));

    cst_error_status.detail().name("error_status");

    type_object->complete().struct_type().member_seq().emplace_back(cst_error_status);

    CompleteStructMember cst_has_errors;
    cst_has_errors.common().member_id(memberId++);
    cst_has_errors.common().member_flags().TRY_CONSTRUCT1(false); // Unsupported
    cst_has_errors.common().member_flags().TRY_CONSTRUCT2(false); // Unsupported
    cst_has_errors.common().member_flags().IS_EXTERNAL(false); // Unsupported
    cst_has_errors.common().member_flags().IS_OPTIONAL(false);
    cst_has_errors.common().member_flags().IS_MUST_UNDERSTAND(false);
    cst_has_errors.common().member_flags().IS_KEY(false);
    cst_has_errors.common().member_flags().IS_DEFAULT(false); // Doesn't apply
    cst_has_errors.common().member_type_id(*TypeObjectFactory::get_instance()->get_type_identifier("bool", false));


    cst_has_errors.detail().name("has_errors");

    type_object->complete().struct_type().member_seq().emplace_back(cst_has_errors);


    // Header
    type_object->complete().struct_type().header().detail().type_name("MonitoringStatus");
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

    TypeObjectFactory::get_instance()->add_type_object("MonitoringStatus", &identifier, type_object);
    delete type_object;
    return TypeObjectFactory::get_instance()->get_type_object("MonitoringStatus", true);
}
