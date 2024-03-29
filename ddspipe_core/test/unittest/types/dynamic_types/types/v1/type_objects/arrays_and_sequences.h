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
 * @file arrays_and_sequences.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifndef _FAST_DDS_GENERATED_ARRAYS_AND_SEQUENCES_H_
#define _FAST_DDS_GENERATED_ARRAYS_AND_SEQUENCES_H_


#include <fastrtps/utils/fixed_size_string.hpp>

#include <stdint.h>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define eProsima_user_DllExport
#endif  // _WIN32

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(arrays_and_sequences_SOURCE)
#define arrays_and_sequences_DllAPI __declspec( dllexport )
#else
#define arrays_and_sequences_DllAPI __declspec( dllimport )
#endif // arrays_and_sequences_SOURCE
#else
#define arrays_and_sequences_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define arrays_and_sequences_DllAPI
#endif // _WIN32

namespace eprosima {
namespace fastcdr {
class Cdr;
} // namespace fastcdr
} // namespace eprosima


/*!
 * @brief This class represents the structure AnInternalObject defined by the user in the IDL file.
 * @ingroup ARRAYS_AND_SEQUENCES
 */
class AnInternalObject
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport AnInternalObject();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~AnInternalObject();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object AnInternalObject that will be copied.
     */
    eProsima_user_DllExport AnInternalObject(
            const AnInternalObject& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object AnInternalObject that will be copied.
     */
    eProsima_user_DllExport AnInternalObject(
            AnInternalObject&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object AnInternalObject that will be copied.
     */
    eProsima_user_DllExport AnInternalObject& operator =(
            const AnInternalObject& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object AnInternalObject that will be copied.
     */
    eProsima_user_DllExport AnInternalObject& operator =(
            AnInternalObject&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x AnInternalObject object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const AnInternalObject& x) const;

    /*!
     * @brief Comparison operator.
     * @param x AnInternalObject object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const AnInternalObject& x) const;

    /*!
     * @brief This function sets a value in member x
     * @param _x New value for member x
     */
    eProsima_user_DllExport void x(
            float _x);

    /*!
     * @brief This function returns the value of member x
     * @return Value of member x
     */
    eProsima_user_DllExport float x() const;

    /*!
     * @brief This function returns a reference to member x
     * @return Reference to member x
     */
    eProsima_user_DllExport float& x();

    /*!
     * @brief This function sets a value in member positive
     * @param _positive New value for member positive
     */
    eProsima_user_DllExport void positive(
            bool _positive);

    /*!
     * @brief This function returns the value of member positive
     * @return Value of member positive
     */
    eProsima_user_DllExport bool positive() const;

    /*!
     * @brief This function returns a reference to member positive
     * @return Reference to member positive
     */
    eProsima_user_DllExport bool& positive();


    /*!
     * @brief This function returns the maximum serialized size of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
            size_t current_alignment = 0);

    /*!
     * @brief This function returns the serialized size of a data depending on the buffer alignment.
     * @param data Data which is calculated its serialized size.
     * @param current_alignment Buffer alignment.
     * @return Serialized size.
     */
    eProsima_user_DllExport static size_t getCdrSerializedSize(
            const AnInternalObject& data,
            size_t current_alignment = 0);


    /*!
     * @brief This function serializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serialize(
            eprosima::fastcdr::Cdr& cdr) const;

    /*!
     * @brief This function deserializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void deserialize(
            eprosima::fastcdr::Cdr& cdr);



    /*!
     * @brief This function returns the maximum serialized size of the Key of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
            size_t current_alignment = 0);

    /*!
     * @brief This function tells you if the Key has been defined for this type
     */
    eProsima_user_DllExport static bool isKeyDefined();

    /*!
     * @brief This function serializes the key members of an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serializeKey(
            eprosima::fastcdr::Cdr& cdr) const;

private:

    float m_x;
    bool m_positive;
};
/*!
 * @brief This class represents the structure arrays_and_sequences defined by the user in the IDL file.
 * @ingroup ARRAYS_AND_SEQUENCES
 */
class arrays_and_sequences
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport arrays_and_sequences();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~arrays_and_sequences();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object arrays_and_sequences that will be copied.
     */
    eProsima_user_DllExport arrays_and_sequences(
            const arrays_and_sequences& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object arrays_and_sequences that will be copied.
     */
    eProsima_user_DllExport arrays_and_sequences(
            arrays_and_sequences&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object arrays_and_sequences that will be copied.
     */
    eProsima_user_DllExport arrays_and_sequences& operator =(
            const arrays_and_sequences& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object arrays_and_sequences that will be copied.
     */
    eProsima_user_DllExport arrays_and_sequences& operator =(
            arrays_and_sequences&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x arrays_and_sequences object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const arrays_and_sequences& x) const;

    /*!
     * @brief Comparison operator.
     * @param x arrays_and_sequences object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const arrays_and_sequences& x) const;

    /*!
     * @brief This function copies the value in member unlimited_vector
     * @param _unlimited_vector New value to be copied in member unlimited_vector
     */
    eProsima_user_DllExport void unlimited_vector(
            const std::vector<AnInternalObject>& _unlimited_vector);

    /*!
     * @brief This function moves the value in member unlimited_vector
     * @param _unlimited_vector New value to be moved in member unlimited_vector
     */
    eProsima_user_DllExport void unlimited_vector(
            std::vector<AnInternalObject>&& _unlimited_vector);

    /*!
     * @brief This function returns a constant reference to member unlimited_vector
     * @return Constant reference to member unlimited_vector
     */
    eProsima_user_DllExport const std::vector<AnInternalObject>& unlimited_vector() const;

    /*!
     * @brief This function returns a reference to member unlimited_vector
     * @return Reference to member unlimited_vector
     */
    eProsima_user_DllExport std::vector<AnInternalObject>& unlimited_vector();
    /*!
     * @brief This function copies the value in member limited_vector
     * @param _limited_vector New value to be copied in member limited_vector
     */
    eProsima_user_DllExport void limited_vector(
            const std::vector<AnInternalObject>& _limited_vector);

    /*!
     * @brief This function moves the value in member limited_vector
     * @param _limited_vector New value to be moved in member limited_vector
     */
    eProsima_user_DllExport void limited_vector(
            std::vector<AnInternalObject>&& _limited_vector);

    /*!
     * @brief This function returns a constant reference to member limited_vector
     * @return Constant reference to member limited_vector
     */
    eProsima_user_DllExport const std::vector<AnInternalObject>& limited_vector() const;

    /*!
     * @brief This function returns a reference to member limited_vector
     * @return Reference to member limited_vector
     */
    eProsima_user_DllExport std::vector<AnInternalObject>& limited_vector();
    /*!
     * @brief This function copies the value in member limited_array
     * @param _limited_array New value to be copied in member limited_array
     */
    eProsima_user_DllExport void limited_array(
            const std::array<AnInternalObject, 10>& _limited_array);

    /*!
     * @brief This function moves the value in member limited_array
     * @param _limited_array New value to be moved in member limited_array
     */
    eProsima_user_DllExport void limited_array(
            std::array<AnInternalObject, 10>&& _limited_array);

    /*!
     * @brief This function returns a constant reference to member limited_array
     * @return Constant reference to member limited_array
     */
    eProsima_user_DllExport const std::array<AnInternalObject, 10>& limited_array() const;

    /*!
     * @brief This function returns a reference to member limited_array
     * @return Reference to member limited_array
     */
    eProsima_user_DllExport std::array<AnInternalObject, 10>& limited_array();

    /*!
     * @brief This function returns the maximum serialized size of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
            size_t current_alignment = 0);

    /*!
     * @brief This function returns the serialized size of a data depending on the buffer alignment.
     * @param data Data which is calculated its serialized size.
     * @param current_alignment Buffer alignment.
     * @return Serialized size.
     */
    eProsima_user_DllExport static size_t getCdrSerializedSize(
            const arrays_and_sequences& data,
            size_t current_alignment = 0);


    /*!
     * @brief This function serializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serialize(
            eprosima::fastcdr::Cdr& cdr) const;

    /*!
     * @brief This function deserializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void deserialize(
            eprosima::fastcdr::Cdr& cdr);



    /*!
     * @brief This function returns the maximum serialized size of the Key of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
            size_t current_alignment = 0);

    /*!
     * @brief This function tells you if the Key has been defined for this type
     */
    eProsima_user_DllExport static bool isKeyDefined();

    /*!
     * @brief This function serializes the key members of an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serializeKey(
            eprosima::fastcdr::Cdr& cdr) const;

private:

    std::vector<AnInternalObject> m_unlimited_vector;
    std::vector<AnInternalObject> m_limited_vector;
    std::array<AnInternalObject, 10> m_limited_array;
};

#endif // _FAST_DDS_GENERATED_ARRAYS_AND_SEQUENCES_H_
