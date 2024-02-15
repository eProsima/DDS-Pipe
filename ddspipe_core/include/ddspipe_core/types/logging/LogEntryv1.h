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
 * @file LogEntry.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool fastddsgen.
 */

#include <fastcdr/config.h>

#if FASTCDR_VERSION_MAJOR == 1

#ifndef _FAST_DDS_GENERATED_LOGENTRY_H_
#define _FAST_DDS_GENERATED_LOGENTRY_H_


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
#if defined(LOGENTRY_SOURCE)
#define LOGENTRY_DllAPI __declspec( dllexport )
#else
#define LOGENTRY_DllAPI __declspec( dllimport )
#endif // LOGENTRY_SOURCE
#else
#define LOGENTRY_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define LOGENTRY_DllAPI
#endif // _WIN32

namespace eprosima {
namespace fastcdr {
class Cdr;
} // namespace fastcdr
} // namespace eprosima



const int32_t UNDEFINED = 0x10000000;

const int32_t SAMPLE_LOST = 0x10000001;

const int32_t TOPIC_MISMATCH_TYPE = 0x10000002;

const int32_t TOPIC_MISMATCH_QOS = 0x10000003;

/*!
 * @brief This class represents the enumeration Kind defined by the user in the IDL file.
 * @ingroup LogEntry
 */
enum Kind : uint32_t
{
    Info,
    Warning,
    Error
};


/*!
 * @brief This class represents the structure LogEntry defined by the user in the IDL file.
 * @ingroup LogEntry
 */
class LogEntry
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport LogEntry();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~LogEntry();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object LogEntry that will be copied.
     */
    eProsima_user_DllExport LogEntry(
            const LogEntry& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object LogEntry that will be copied.
     */
    eProsima_user_DllExport LogEntry(
            LogEntry&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object LogEntry that will be copied.
     */
    eProsima_user_DllExport LogEntry& operator =(
            const LogEntry& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object LogEntry that will be copied.
     */
    eProsima_user_DllExport LogEntry& operator =(
            LogEntry&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x LogEntry object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const LogEntry& x) const;

    /*!
     * @brief Comparison operator.
     * @param x LogEntry object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const LogEntry& x) const;

    /*!
     * @brief This function sets a value in member event
     * @param _event New value for member event
     */
    eProsima_user_DllExport void event(
            int32_t _event);

    /*!
     * @brief This function returns the value of member event
     * @return Value of member event
     */
    eProsima_user_DllExport int32_t event() const;

    /*!
     * @brief This function returns a reference to member event
     * @return Reference to member event
     */
    eProsima_user_DllExport int32_t& event();


    /*!
     * @brief This function sets a value in member kind
     * @param _kind New value for member kind
     */
    eProsima_user_DllExport void kind(
            Kind _kind);

    /*!
     * @brief This function returns the value of member kind
     * @return Value of member kind
     */
    eProsima_user_DllExport Kind kind() const;

    /*!
     * @brief This function returns a reference to member kind
     * @return Reference to member kind
     */
    eProsima_user_DllExport Kind& kind();


    /*!
     * @brief This function copies the value in member category
     * @param _category New value to be copied in member category
     */
    eProsima_user_DllExport void category(
            const std::string& _category);

    /*!
     * @brief This function moves the value in member category
     * @param _category New value to be moved in member category
     */
    eProsima_user_DllExport void category(
            std::string&& _category);

    /*!
     * @brief This function returns a constant reference to member category
     * @return Constant reference to member category
     */
    eProsima_user_DllExport const std::string& category() const;

    /*!
     * @brief This function returns a reference to member category
     * @return Reference to member category
     */
    eProsima_user_DllExport std::string& category();


    /*!
     * @brief This function copies the value in member message
     * @param _message New value to be copied in member message
     */
    eProsima_user_DllExport void message(
            const std::string& _message);

    /*!
     * @brief This function moves the value in member message
     * @param _message New value to be moved in member message
     */
    eProsima_user_DllExport void message(
            std::string&& _message);

    /*!
     * @brief This function returns a constant reference to member message
     * @return Constant reference to member message
     */
    eProsima_user_DllExport const std::string& message() const;

    /*!
     * @brief This function returns a reference to member message
     * @return Reference to member message
     */
    eProsima_user_DllExport std::string& message();


    /*!
     * @brief This function copies the value in member timestamp
     * @param _timestamp New value to be copied in member timestamp
     */
    eProsima_user_DllExport void timestamp(
            const std::string& _timestamp);

    /*!
     * @brief This function moves the value in member timestamp
     * @param _timestamp New value to be moved in member timestamp
     */
    eProsima_user_DllExport void timestamp(
            std::string&& _timestamp);

    /*!
     * @brief This function returns a constant reference to member timestamp
     * @return Constant reference to member timestamp
     */
    eProsima_user_DllExport const std::string& timestamp() const;

    /*!
     * @brief This function returns a reference to member timestamp
     * @return Reference to member timestamp
     */
    eProsima_user_DllExport std::string& timestamp();


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
            const LogEntry& data,
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

    int32_t m_event;
    Kind m_kind;
    std::string m_category;
    std::string m_message;
    std::string m_timestamp;

};


#endif // _FAST_DDS_GENERATED_LOGENTRY_H_



#endif // FASTCDR_VERSION_MAJOR == 1
