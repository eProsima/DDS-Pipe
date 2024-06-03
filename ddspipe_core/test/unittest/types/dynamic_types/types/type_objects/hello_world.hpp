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
 * @file hello_world.hpp
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool fastddsgen.
 */

#ifndef _FAST_DDS_GENERATED_HELLO_WORLD_HPP_
#define _FAST_DDS_GENERATED_HELLO_WORLD_HPP_

#include <cstdint>
#include <string>
#include <utility>
#include <fastcdr/cdr/fixed_size_string.hpp>

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
#if defined(HELLO_WORLD_SOURCE)
#define HELLO_WORLD_DllAPI __declspec( dllexport )
#else
#define HELLO_WORLD_DllAPI __declspec( dllimport )
#endif // HELLO_WORLD_SOURCE
#else
#define HELLO_WORLD_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define HELLO_WORLD_DllAPI
#endif // _WIN32

/*!
 * @brief This class represents the structure hello_world defined by the user in the IDL file.
 * @ingroup hello_world
 */
class hello_world
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport hello_world()
    {
    }

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~hello_world()
    {
    }

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object hello_world that will be copied.
     */
    eProsima_user_DllExport hello_world(
            const hello_world& x)
    {
                    m_index = x.m_index;

                    m_message = x.m_message;

    }

    /*!
     * @brief Move constructor.
     * @param x Reference to the object hello_world that will be copied.
     */
    eProsima_user_DllExport hello_world(
            hello_world&& x) noexcept
    {
        m_index = x.m_index;
        m_message = std::move(x.m_message);
    }

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object hello_world that will be copied.
     */
    eProsima_user_DllExport hello_world& operator =(
            const hello_world& x)
    {

                    m_index = x.m_index;

                    m_message = x.m_message;

        return *this;
    }

    /*!
     * @brief Move assignment.
     * @param x Reference to the object hello_world that will be copied.
     */
    eProsima_user_DllExport hello_world& operator =(
            hello_world&& x) noexcept
    {

        m_index = x.m_index;
        m_message = std::move(x.m_message);
        return *this;
    }

    /*!
     * @brief Comparison operator.
     * @param x hello_world object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const hello_world& x) const
    {
        return (m_index == x.m_index &&
           m_message == x.m_message);
    }

    /*!
     * @brief Comparison operator.
     * @param x hello_world object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const hello_world& x) const
    {
        return !(*this == x);
    }

    /*!
     * @brief This function sets a value in member index
     * @param _index New value for member index
     */
    eProsima_user_DllExport void index(
            uint32_t _index)
    {
        m_index = _index;
    }

    /*!
     * @brief This function returns the value of member index
     * @return Value of member index
     */
    eProsima_user_DllExport uint32_t index() const
    {
        return m_index;
    }

    /*!
     * @brief This function returns a reference to member index
     * @return Reference to member index
     */
    eProsima_user_DllExport uint32_t& index()
    {
        return m_index;
    }


    /*!
     * @brief This function copies the value in member message
     * @param _message New value to be copied in member message
     */
    eProsima_user_DllExport void message(
            const std::string& _message)
    {
        m_message = _message;
    }

    /*!
     * @brief This function moves the value in member message
     * @param _message New value to be moved in member message
     */
    eProsima_user_DllExport void message(
            std::string&& _message)
    {
        m_message = std::move(_message);
    }

    /*!
     * @brief This function returns a constant reference to member message
     * @return Constant reference to member message
     */
    eProsima_user_DllExport const std::string& message() const
    {
        return m_message;
    }

    /*!
     * @brief This function returns a reference to member message
     * @return Reference to member message
     */
    eProsima_user_DllExport std::string& message()
    {
        return m_message;
    }



private:

    uint32_t m_index{0};
    std::string m_message;

};

#endif // _FAST_DDS_GENERATED_HELLO_WORLD_HPP_

