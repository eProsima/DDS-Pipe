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
 * @file MonitoringTopicsPubSubTypes.cpp
 * This header file contains the implementation of the serialization functions.
 *
 * This file was generated by the tool fastddsgen.
 */


#include <fastdds/rtps/common/CdrSerialization.hpp>

#include "MonitoringTopicsPubSubTypes.h"
#include "MonitoringTopicsCdrAux.hpp"

using SerializedPayload_t = eprosima::fastrtps::rtps::SerializedPayload_t;
using InstanceHandle_t = eprosima::fastrtps::rtps::InstanceHandle_t;
using DataRepresentationId_t = eprosima::fastdds::dds::DataRepresentationId_t;



DdsTopicDataPubSubType::DdsTopicDataPubSubType()
{
    setName("DdsTopicData");
    uint32_t type_size =
#if FASTCDR_VERSION_MAJOR == 1
        static_cast<uint32_t>(DdsTopicData::getMaxCdrSerializedSize());
#else
        DdsTopicData_max_cdr_typesize;
#endif
    type_size += static_cast<uint32_t>(eprosima::fastcdr::Cdr::alignment(type_size, 4)); /* possible submessage alignment */
    m_typeSize = type_size + 4; /*encapsulation*/
    m_isGetKeyDefined = false;
    uint32_t keyLength = DdsTopicData_max_key_cdr_typesize > 16 ? DdsTopicData_max_key_cdr_typesize : 16;
    m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
    memset(m_keyBuffer, 0, keyLength);
}

DdsTopicDataPubSubType::~DdsTopicDataPubSubType()
{
    if (m_keyBuffer != nullptr)
    {
        free(m_keyBuffer);
    }
}

bool DdsTopicDataPubSubType::serialize(
        void* data,
        SerializedPayload_t* payload,
        DataRepresentationId_t data_representation)
{
    DdsTopicData* p_type = static_cast<DdsTopicData*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
            data_representation == DataRepresentationId_t::XCDR_DATA_REPRESENTATION ?
            eprosima::fastcdr::CdrVersion::XCDRv1 : eprosima::fastcdr::CdrVersion::XCDRv2);
    payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
#if FASTCDR_VERSION_MAJOR > 1
    ser.set_encoding_flag(
        data_representation == DataRepresentationId_t::XCDR_DATA_REPRESENTATION ?
        eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR  :
        eprosima::fastcdr::EncodingAlgorithmFlag::DELIMIT_CDR2);
#endif // FASTCDR_VERSION_MAJOR > 1

    try
    {
        // Serialize encapsulation
        ser.serialize_encapsulation();
        // Serialize the object.
        ser << *p_type;
    }
    catch (eprosima::fastcdr::exception::Exception& /*exception*/)
    {
        return false;
    }

    // Get the serialized length
#if FASTCDR_VERSION_MAJOR == 1
    payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
#else
    payload->length = static_cast<uint32_t>(ser.get_serialized_data_length());
#endif // FASTCDR_VERSION_MAJOR == 1
    return true;
}

bool DdsTopicDataPubSubType::deserialize(
        SerializedPayload_t* payload,
        void* data)
{
    try
    {
        // Convert DATA to pointer of your type
        DdsTopicData* p_type = static_cast<DdsTopicData*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

        // Object that deserializes the data.
        eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN
#if FASTCDR_VERSION_MAJOR == 1
                , eprosima::fastcdr::Cdr::CdrType::DDS_CDR
#endif // FASTCDR_VERSION_MAJOR == 1
                );

        // Deserialize encapsulation.
        deser.read_encapsulation();
        payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

        // Deserialize the object.
        deser >> *p_type;
    }
    catch (eprosima::fastcdr::exception::Exception& /*exception*/)
    {
        return false;
    }

    return true;
}

std::function<uint32_t()> DdsTopicDataPubSubType::getSerializedSizeProvider(
        void* data,
        DataRepresentationId_t data_representation)
{
    return [data, data_representation]() -> uint32_t
           {
#if FASTCDR_VERSION_MAJOR == 1
               static_cast<void>(data_representation);
               return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<DdsTopicData*>(data))) +
                      4u /*encapsulation*/;
#else
               try
               {
                   eprosima::fastcdr::CdrSizeCalculator calculator(
                       data_representation == DataRepresentationId_t::XCDR_DATA_REPRESENTATION ?
                       eprosima::fastcdr::CdrVersion::XCDRv1 :eprosima::fastcdr::CdrVersion::XCDRv2);
                   size_t current_alignment {0};
                   return static_cast<uint32_t>(calculator.calculate_serialized_size(
                               *static_cast<DdsTopicData*>(data), current_alignment)) +
                           4u /*encapsulation*/;
               }
               catch (eprosima::fastcdr::exception::Exception& /*exception*/)
               {
                   return 0;
               }
#endif // FASTCDR_VERSION_MAJOR == 1
           };
}

void* DdsTopicDataPubSubType::createData()
{
    return reinterpret_cast<void*>(new DdsTopicData());
}

void DdsTopicDataPubSubType::deleteData(
        void* data)
{
    delete(reinterpret_cast<DdsTopicData*>(data));
}

bool DdsTopicDataPubSubType::getKey(
        void* data,
        InstanceHandle_t* handle,
        bool force_md5)
{
    if (!m_isGetKeyDefined)
    {
        return false;
    }

    DdsTopicData* p_type = static_cast<DdsTopicData*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
            DdsTopicData_max_key_cdr_typesize);

    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS, eprosima::fastcdr::CdrVersion::XCDRv1);
#if FASTCDR_VERSION_MAJOR == 1
    p_type->serializeKey(ser);
#else
    eprosima::fastcdr::serialize_key(ser, *p_type);
#endif // FASTCDR_VERSION_MAJOR == 1
    if (force_md5 || DdsTopicData_max_key_cdr_typesize > 16)
    {
        m_md5.init();
#if FASTCDR_VERSION_MAJOR == 1
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
#else
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.get_serialized_data_length()));
#endif // FASTCDR_VERSION_MAJOR == 1
        m_md5.finalize();
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_md5.digest[i];
        }
    }
    else
    {
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_keyBuffer[i];
        }
    }
    return true;
}



DdsTopicPubSubType::DdsTopicPubSubType()
{
    setName("DdsTopic");
    uint32_t type_size =
#if FASTCDR_VERSION_MAJOR == 1
        static_cast<uint32_t>(DdsTopic::getMaxCdrSerializedSize());
#else
        DdsTopic_max_cdr_typesize;
#endif
    type_size += static_cast<uint32_t>(eprosima::fastcdr::Cdr::alignment(type_size, 4)); /* possible submessage alignment */
    m_typeSize = type_size + 4; /*encapsulation*/
    m_isGetKeyDefined = false;
    uint32_t keyLength = DdsTopic_max_key_cdr_typesize > 16 ? DdsTopic_max_key_cdr_typesize : 16;
    m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
    memset(m_keyBuffer, 0, keyLength);
}

DdsTopicPubSubType::~DdsTopicPubSubType()
{
    if (m_keyBuffer != nullptr)
    {
        free(m_keyBuffer);
    }
}

bool DdsTopicPubSubType::serialize(
        void* data,
        SerializedPayload_t* payload,
        DataRepresentationId_t data_representation)
{
    DdsTopic* p_type = static_cast<DdsTopic*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
            data_representation == DataRepresentationId_t::XCDR_DATA_REPRESENTATION ?
            eprosima::fastcdr::CdrVersion::XCDRv1 : eprosima::fastcdr::CdrVersion::XCDRv2);
    payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
#if FASTCDR_VERSION_MAJOR > 1
    ser.set_encoding_flag(
        data_representation == DataRepresentationId_t::XCDR_DATA_REPRESENTATION ?
        eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR  :
        eprosima::fastcdr::EncodingAlgorithmFlag::DELIMIT_CDR2);
#endif // FASTCDR_VERSION_MAJOR > 1

    try
    {
        // Serialize encapsulation
        ser.serialize_encapsulation();
        // Serialize the object.
        ser << *p_type;
    }
    catch (eprosima::fastcdr::exception::Exception& /*exception*/)
    {
        return false;
    }

    // Get the serialized length
#if FASTCDR_VERSION_MAJOR == 1
    payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
#else
    payload->length = static_cast<uint32_t>(ser.get_serialized_data_length());
#endif // FASTCDR_VERSION_MAJOR == 1
    return true;
}

bool DdsTopicPubSubType::deserialize(
        SerializedPayload_t* payload,
        void* data)
{
    try
    {
        // Convert DATA to pointer of your type
        DdsTopic* p_type = static_cast<DdsTopic*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

        // Object that deserializes the data.
        eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN
#if FASTCDR_VERSION_MAJOR == 1
                , eprosima::fastcdr::Cdr::CdrType::DDS_CDR
#endif // FASTCDR_VERSION_MAJOR == 1
                );

        // Deserialize encapsulation.
        deser.read_encapsulation();
        payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

        // Deserialize the object.
        deser >> *p_type;
    }
    catch (eprosima::fastcdr::exception::Exception& /*exception*/)
    {
        return false;
    }

    return true;
}

std::function<uint32_t()> DdsTopicPubSubType::getSerializedSizeProvider(
        void* data,
        DataRepresentationId_t data_representation)
{
    return [data, data_representation]() -> uint32_t
           {
#if FASTCDR_VERSION_MAJOR == 1
               static_cast<void>(data_representation);
               return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<DdsTopic*>(data))) +
                      4u /*encapsulation*/;
#else
               try
               {
                   eprosima::fastcdr::CdrSizeCalculator calculator(
                       data_representation == DataRepresentationId_t::XCDR_DATA_REPRESENTATION ?
                       eprosima::fastcdr::CdrVersion::XCDRv1 :eprosima::fastcdr::CdrVersion::XCDRv2);
                   size_t current_alignment {0};
                   return static_cast<uint32_t>(calculator.calculate_serialized_size(
                               *static_cast<DdsTopic*>(data), current_alignment)) +
                           4u /*encapsulation*/;
               }
               catch (eprosima::fastcdr::exception::Exception& /*exception*/)
               {
                   return 0;
               }
#endif // FASTCDR_VERSION_MAJOR == 1
           };
}

void* DdsTopicPubSubType::createData()
{
    return reinterpret_cast<void*>(new DdsTopic());
}

void DdsTopicPubSubType::deleteData(
        void* data)
{
    delete(reinterpret_cast<DdsTopic*>(data));
}

bool DdsTopicPubSubType::getKey(
        void* data,
        InstanceHandle_t* handle,
        bool force_md5)
{
    if (!m_isGetKeyDefined)
    {
        return false;
    }

    DdsTopic* p_type = static_cast<DdsTopic*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
            DdsTopic_max_key_cdr_typesize);

    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS, eprosima::fastcdr::CdrVersion::XCDRv1);
#if FASTCDR_VERSION_MAJOR == 1
    p_type->serializeKey(ser);
#else
    eprosima::fastcdr::serialize_key(ser, *p_type);
#endif // FASTCDR_VERSION_MAJOR == 1
    if (force_md5 || DdsTopic_max_key_cdr_typesize > 16)
    {
        m_md5.init();
#if FASTCDR_VERSION_MAJOR == 1
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
#else
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.get_serialized_data_length()));
#endif // FASTCDR_VERSION_MAJOR == 1
        m_md5.finalize();
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_md5.digest[i];
        }
    }
    else
    {
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_keyBuffer[i];
        }
    }
    return true;
}





MonitoringTopicsPubSubType::MonitoringTopicsPubSubType()
{
    setName("MonitoringTopics");
    uint32_t type_size =
#if FASTCDR_VERSION_MAJOR == 1
        static_cast<uint32_t>(MonitoringTopics::getMaxCdrSerializedSize());
#else
        MonitoringTopics_max_cdr_typesize;
#endif
    type_size += static_cast<uint32_t>(eprosima::fastcdr::Cdr::alignment(type_size, 4)); /* possible submessage alignment */
    m_typeSize = type_size + 4; /*encapsulation*/
    m_isGetKeyDefined = false;
    uint32_t keyLength = MonitoringTopics_max_key_cdr_typesize > 16 ? MonitoringTopics_max_key_cdr_typesize : 16;
    m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
    memset(m_keyBuffer, 0, keyLength);
}

MonitoringTopicsPubSubType::~MonitoringTopicsPubSubType()
{
    if (m_keyBuffer != nullptr)
    {
        free(m_keyBuffer);
    }
}

bool MonitoringTopicsPubSubType::serialize(
        void* data,
        SerializedPayload_t* payload,
        DataRepresentationId_t data_representation)
{
    MonitoringTopics* p_type = static_cast<MonitoringTopics*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
            data_representation == DataRepresentationId_t::XCDR_DATA_REPRESENTATION ?
            eprosima::fastcdr::CdrVersion::XCDRv1 : eprosima::fastcdr::CdrVersion::XCDRv2);
    payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
#if FASTCDR_VERSION_MAJOR > 1
    ser.set_encoding_flag(
        data_representation == DataRepresentationId_t::XCDR_DATA_REPRESENTATION ?
        eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR  :
        eprosima::fastcdr::EncodingAlgorithmFlag::DELIMIT_CDR2);
#endif // FASTCDR_VERSION_MAJOR > 1

    try
    {
        // Serialize encapsulation
        ser.serialize_encapsulation();
        // Serialize the object.
        ser << *p_type;
    }
    catch (eprosima::fastcdr::exception::Exception& /*exception*/)
    {
        return false;
    }

    // Get the serialized length
#if FASTCDR_VERSION_MAJOR == 1
    payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
#else
    payload->length = static_cast<uint32_t>(ser.get_serialized_data_length());
#endif // FASTCDR_VERSION_MAJOR == 1
    return true;
}

bool MonitoringTopicsPubSubType::deserialize(
        SerializedPayload_t* payload,
        void* data)
{
    try
    {
        // Convert DATA to pointer of your type
        MonitoringTopics* p_type = static_cast<MonitoringTopics*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

        // Object that deserializes the data.
        eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN
#if FASTCDR_VERSION_MAJOR == 1
                , eprosima::fastcdr::Cdr::CdrType::DDS_CDR
#endif // FASTCDR_VERSION_MAJOR == 1
                );

        // Deserialize encapsulation.
        deser.read_encapsulation();
        payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

        // Deserialize the object.
        deser >> *p_type;
    }
    catch (eprosima::fastcdr::exception::Exception& /*exception*/)
    {
        return false;
    }

    return true;
}

std::function<uint32_t()> MonitoringTopicsPubSubType::getSerializedSizeProvider(
        void* data,
        DataRepresentationId_t data_representation)
{
    return [data, data_representation]() -> uint32_t
           {
#if FASTCDR_VERSION_MAJOR == 1
               static_cast<void>(data_representation);
               return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<MonitoringTopics*>(data))) +
                      4u /*encapsulation*/;
#else
               try
               {
                   eprosima::fastcdr::CdrSizeCalculator calculator(
                       data_representation == DataRepresentationId_t::XCDR_DATA_REPRESENTATION ?
                       eprosima::fastcdr::CdrVersion::XCDRv1 :eprosima::fastcdr::CdrVersion::XCDRv2);
                   size_t current_alignment {0};
                   return static_cast<uint32_t>(calculator.calculate_serialized_size(
                               *static_cast<MonitoringTopics*>(data), current_alignment)) +
                           4u /*encapsulation*/;
               }
               catch (eprosima::fastcdr::exception::Exception& /*exception*/)
               {
                   return 0;
               }
#endif // FASTCDR_VERSION_MAJOR == 1
           };
}

void* MonitoringTopicsPubSubType::createData()
{
    return reinterpret_cast<void*>(new MonitoringTopics());
}

void MonitoringTopicsPubSubType::deleteData(
        void* data)
{
    delete(reinterpret_cast<MonitoringTopics*>(data));
}

bool MonitoringTopicsPubSubType::getKey(
        void* data,
        InstanceHandle_t* handle,
        bool force_md5)
{
    if (!m_isGetKeyDefined)
    {
        return false;
    }

    MonitoringTopics* p_type = static_cast<MonitoringTopics*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
            MonitoringTopics_max_key_cdr_typesize);

    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS, eprosima::fastcdr::CdrVersion::XCDRv1);
#if FASTCDR_VERSION_MAJOR == 1
    p_type->serializeKey(ser);
#else
    eprosima::fastcdr::serialize_key(ser, *p_type);
#endif // FASTCDR_VERSION_MAJOR == 1
    if (force_md5 || MonitoringTopics_max_key_cdr_typesize > 16)
    {
        m_md5.init();
#if FASTCDR_VERSION_MAJOR == 1
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
#else
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.get_serialized_data_length()));
#endif // FASTCDR_VERSION_MAJOR == 1
        m_md5.finalize();
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_md5.digest[i];
        }
    }
    else
    {
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_keyBuffer[i];
        }
    }
    return true;
}

