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
 * @file complex_nested_arraysPubSubTypes.hpp
 * This header file contains the declaration of the serialization functions.
 *
 * This file was generated by the tool fastddsgen.
 */


#ifndef FAST_DDS_GENERATED__COMPLEX_NESTED_ARRAYS_PUBSUBTYPES_HPP
#define FAST_DDS_GENERATED__COMPLEX_NESTED_ARRAYS_PUBSUBTYPES_HPP

#include <fastdds/dds/core/policy/QosPolicies.hpp>
#include <fastdds/dds/topic/TopicDataType.hpp>
#include <fastdds/rtps/common/InstanceHandle.hpp>
#include <fastdds/rtps/common/SerializedPayload.hpp>
#include <fastdds/utils/md5.hpp>

#include "complex_nested_arrays.hpp"


#if !defined(FASTDDS_GEN_API_VER) || (FASTDDS_GEN_API_VER != 3)
#error \
    Generated complex_nested_arrays is not compatible with current installed Fast DDS. Please, regenerate it with fastddsgen.
#endif  // FASTDDS_GEN_API_VER


/*!
 * @brief This class represents the TopicDataType of the type ThirdLevelElement defined by the user in the IDL file.
 * @ingroup complex_nested_arrays
 */
class ThirdLevelElementPubSubType : public eprosima::fastdds::dds::TopicDataType
{
public:

    typedef ThirdLevelElement type;

    eProsima_user_DllExport ThirdLevelElementPubSubType();

    eProsima_user_DllExport ~ThirdLevelElementPubSubType() override;

    eProsima_user_DllExport bool serialize(
            const void* const data,
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) override;

    eProsima_user_DllExport bool deserialize(
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            void* data) override;

    eProsima_user_DllExport uint32_t calculate_serialized_size(
            const void* const data,
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) override;

    eProsima_user_DllExport bool compute_key(
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            eprosima::fastdds::rtps::InstanceHandle_t& ihandle,
            bool force_md5 = false) override;

    eProsima_user_DllExport bool compute_key(
            const void* const data,
            eprosima::fastdds::rtps::InstanceHandle_t& ihandle,
            bool force_md5 = false) override;

    eProsima_user_DllExport void* create_data() override;

    eProsima_user_DllExport void delete_data(
            void* data) override;

    //Register TypeObject representation in Fast DDS TypeObjectRegistry
    eProsima_user_DllExport void register_type_object_representation() override;

#ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
    eProsima_user_DllExport inline bool is_bounded() const override
    {
        return true;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED

#ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

    eProsima_user_DllExport inline bool is_plain(
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) const override
    {
        static_cast<void>(data_representation);
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

#ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
    eProsima_user_DllExport inline bool construct_sample(
            void* memory) const override
    {
        static_cast<void>(memory);
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE

private:

    eprosima::fastdds::MD5 md5_;
    unsigned char* key_buffer_;

};

/*!
 * @brief This class represents the TopicDataType of the type SecondLevelElement defined by the user in the IDL file.
 * @ingroup complex_nested_arrays
 */
class SecondLevelElementPubSubType : public eprosima::fastdds::dds::TopicDataType
{
public:

    typedef SecondLevelElement type;

    eProsima_user_DllExport SecondLevelElementPubSubType();

    eProsima_user_DllExport ~SecondLevelElementPubSubType() override;

    eProsima_user_DllExport bool serialize(
            const void* const data,
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) override;

    eProsima_user_DllExport bool deserialize(
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            void* data) override;

    eProsima_user_DllExport uint32_t calculate_serialized_size(
            const void* const data,
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) override;

    eProsima_user_DllExport bool compute_key(
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            eprosima::fastdds::rtps::InstanceHandle_t& ihandle,
            bool force_md5 = false) override;

    eProsima_user_DllExport bool compute_key(
            const void* const data,
            eprosima::fastdds::rtps::InstanceHandle_t& ihandle,
            bool force_md5 = false) override;

    eProsima_user_DllExport void* create_data() override;

    eProsima_user_DllExport void delete_data(
            void* data) override;

    //Register TypeObject representation in Fast DDS TypeObjectRegistry
    eProsima_user_DllExport void register_type_object_representation() override;

#ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
    eProsima_user_DllExport inline bool is_bounded() const override
    {
        return true;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED

#ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

    eProsima_user_DllExport inline bool is_plain(
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) const override
    {
        static_cast<void>(data_representation);
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

#ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
    eProsima_user_DllExport inline bool construct_sample(
            void* memory) const override
    {
        static_cast<void>(memory);
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE

private:

    eprosima::fastdds::MD5 md5_;
    unsigned char* key_buffer_;

};

/*!
 * @brief This class represents the TopicDataType of the type FirstLevelElement defined by the user in the IDL file.
 * @ingroup complex_nested_arrays
 */
class FirstLevelElementPubSubType : public eprosima::fastdds::dds::TopicDataType
{
public:

    typedef FirstLevelElement type;

    eProsima_user_DllExport FirstLevelElementPubSubType();

    eProsima_user_DllExport ~FirstLevelElementPubSubType() override;

    eProsima_user_DllExport bool serialize(
            const void* const data,
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) override;

    eProsima_user_DllExport bool deserialize(
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            void* data) override;

    eProsima_user_DllExport uint32_t calculate_serialized_size(
            const void* const data,
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) override;

    eProsima_user_DllExport bool compute_key(
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            eprosima::fastdds::rtps::InstanceHandle_t& ihandle,
            bool force_md5 = false) override;

    eProsima_user_DllExport bool compute_key(
            const void* const data,
            eprosima::fastdds::rtps::InstanceHandle_t& ihandle,
            bool force_md5 = false) override;

    eProsima_user_DllExport void* create_data() override;

    eProsima_user_DllExport void delete_data(
            void* data) override;

    //Register TypeObject representation in Fast DDS TypeObjectRegistry
    eProsima_user_DllExport void register_type_object_representation() override;

#ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
    eProsima_user_DllExport inline bool is_bounded() const override
    {
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED

#ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

    eProsima_user_DllExport inline bool is_plain(
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) const override
    {
        static_cast<void>(data_representation);
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

#ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
    eProsima_user_DllExport inline bool construct_sample(
            void* memory) const override
    {
        static_cast<void>(memory);
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE

private:

    eprosima::fastdds::MD5 md5_;
    unsigned char* key_buffer_;

};

/*!
 * @brief This class represents the TopicDataType of the type complex_nested_arrays defined by the user in the IDL file.
 * @ingroup complex_nested_arrays
 */
class complex_nested_arraysPubSubType : public eprosima::fastdds::dds::TopicDataType
{
public:

    typedef complex_nested_arrays type;

    eProsima_user_DllExport complex_nested_arraysPubSubType();

    eProsima_user_DllExport ~complex_nested_arraysPubSubType() override;

    eProsima_user_DllExport bool serialize(
            const void* const data,
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) override;

    eProsima_user_DllExport bool deserialize(
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            void* data) override;

    eProsima_user_DllExport uint32_t calculate_serialized_size(
            const void* const data,
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) override;

    eProsima_user_DllExport bool compute_key(
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            eprosima::fastdds::rtps::InstanceHandle_t& ihandle,
            bool force_md5 = false) override;

    eProsima_user_DllExport bool compute_key(
            const void* const data,
            eprosima::fastdds::rtps::InstanceHandle_t& ihandle,
            bool force_md5 = false) override;

    eProsima_user_DllExport void* create_data() override;

    eProsima_user_DllExport void delete_data(
            void* data) override;

    //Register TypeObject representation in Fast DDS TypeObjectRegistry
    eProsima_user_DllExport void register_type_object_representation() override;

#ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
    eProsima_user_DllExport inline bool is_bounded() const override
    {
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED

#ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

    eProsima_user_DllExport inline bool is_plain(
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) const override
    {
        static_cast<void>(data_representation);
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

#ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
    eProsima_user_DllExport inline bool construct_sample(
            void* memory) const override
    {
        static_cast<void>(memory);
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE

private:

    eprosima::fastdds::MD5 md5_;
    unsigned char* key_buffer_;

};

#endif // FAST_DDS_GENERATED__COMPLEX_NESTED_ARRAYS_PUBSUBTYPES_HPP
