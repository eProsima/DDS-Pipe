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
 * @file union_struct.hpp
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool fastddsgen.
 */

#ifndef FAST_DDS_GENERATED__UNION_STRUCT_HPP
#define FAST_DDS_GENERATED__UNION_STRUCT_HPP

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <fastcdr/cdr/fixed_size_string.hpp>
#include <fastcdr/exceptions/BadParamException.h>


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
#if defined(UNION_STRUCT_SOURCE)
#define UNION_STRUCT_DllAPI __declspec( dllexport )
#else
#define UNION_STRUCT_DllAPI __declspec( dllimport )
#endif // UNION_STRUCT_SOURCE
#else
#define UNION_STRUCT_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define UNION_STRUCT_DllAPI
#endif // _WIN32

/*!
 * @brief This class represents the union MyUnion defined by the user in the IDL file.
 * @ingroup union_struct
 */
class MyUnion
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport MyUnion()
    {
    }

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~MyUnion()
    {
        if (member_destructor_)
        {
            member_destructor_();
        }
    }

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object MyUnion that will be copied.
     */
    eProsima_user_DllExport MyUnion(
            const MyUnion& x)
    {
        m__d = x.m__d;

        switch (x.selected_member_)
        {
                        case 0x00000001:
                            octet_value_() = x.m_octet_value;
                            break;

                        case 0x00000002:
                            long_value_() = x.m_long_value;
                            break;

                        case 0x00000003:
                            string_value_() = x.m_string_value;
                            break;

        }
    }

    /*!
     * @brief Move constructor.
     * @param x Reference to the object MyUnion that will be copied.
     */
    eProsima_user_DllExport MyUnion(
            MyUnion&& x) noexcept
    {
        m__d = x.m__d;

        switch (x.selected_member_)
        {
                        case 0x00000001:
                            octet_value_() = std::move(x.m_octet_value);
                            break;

                        case 0x00000002:
                            long_value_() = std::move(x.m_long_value);
                            break;

                        case 0x00000003:
                            string_value_() = std::move(x.m_string_value);
                            break;

        }
    }

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object MyUnion that will be copied.
     */
    eProsima_user_DllExport MyUnion& operator =(
            const MyUnion& x)
    {
        m__d = x.m__d;

        switch (x.selected_member_)
        {
                        case 0x00000001:
                            octet_value_() = x.m_octet_value;
                            break;

                        case 0x00000002:
                            long_value_() = x.m_long_value;
                            break;

                        case 0x00000003:
                            string_value_() = x.m_string_value;
                            break;

        }

        return *this;
    }

    /*!
     * @brief Move assignment.
     * @param x Reference to the object MyUnion that will be copied.
     */
    eProsima_user_DllExport MyUnion& operator =(
            MyUnion&& x) noexcept
    {
        m__d = x.m__d;

        switch (x.selected_member_)
        {
                        case 0x00000001:
                            octet_value_() = std::move(x.m_octet_value);
                            break;

                        case 0x00000002:
                            long_value_() = std::move(x.m_long_value);
                            break;

                        case 0x00000003:
                            string_value_() = std::move(x.m_string_value);
                            break;

        }

        return *this;
    }

    /*!
     * @brief Comparison operator.
     * @param x MyUnion object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const MyUnion& x) const
    {
        bool ret_value {false};

        if (m__d == x.m__d &&
                selected_member_ == x.selected_member_)
        {
            switch (selected_member_)
            {
                                case 0x00000001:
                                    ret_value = (m_octet_value == x.m_octet_value);
                                    break;

                                case 0x00000002:
                                    ret_value = (m_long_value == x.m_long_value);
                                    break;

                                case 0x00000003:
                                    ret_value = (m_string_value == x.m_string_value);
                                    break;

            }
        }

        return ret_value;
    }

    /*!
     * @brief Comparison operator.
     * @param x MyUnion object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const MyUnion& x) const
    {
        return !(*this == x);
    }

    /*!
     * @brief This function sets the discriminator value.
     * @param __d New value for the discriminator.
     * @exception eprosima::fastcdr::exception::BadParamException This exception is thrown if the new value doesn't correspond to the selected union member.
     */
    eProsima_user_DllExport void _d(
            int32_t __d)
    {
        bool valid_discriminator = false;

        switch (__d)
        {
                        case 1:
                            if (0x00000001 == selected_member_)
                            {
                                valid_discriminator = true;
                            }
                            break;

                        case 2:
                            if (0x00000002 == selected_member_)
                            {
                                valid_discriminator = true;
                            }
                            break;

                        case 3:
                            if (0x00000003 == selected_member_)
                            {
                                valid_discriminator = true;
                            }
                            break;

        }

        if (!valid_discriminator)
        {
            throw eprosima::fastcdr::exception::BadParamException("Discriminator doesn't correspond with the selected union member");
        }

        m__d = __d;
    }

    /*!
     * @brief This function returns the value of the discriminator.
     * @return Value of the discriminator
     */
    eProsima_user_DllExport int32_t _d() const
    {
        return m__d;
    }

    /*!
     * @brief This function sets a value in member octet_value
     * @param _octet_value New value for member octet_value
     */
    eProsima_user_DllExport void octet_value(
            uint8_t _octet_value)
    {
        octet_value_() = _octet_value;
        m__d = 1;
    }

    /*!
     * @brief This function returns the value of member octet_value
     * @return Value of member octet_value
     * @exception eprosima::fastcdr::exception::BadParamException This exception is thrown if the requested union member is not the current selection.
     */
    eProsima_user_DllExport uint8_t octet_value() const
    {
        if (0x00000001 != selected_member_)
        {
            throw eprosima::fastcdr::exception::BadParamException("This member has not been selected");
        }

        return m_octet_value;
    }

    /*!
     * @brief This function returns a reference to member octet_value
     * @return Reference to member octet_value
     * @exception eprosima::fastcdr::exception::BadParamException This exception is thrown if the requested union member is not the current selection.
     */
    eProsima_user_DllExport uint8_t& octet_value()
    {
        if (0x00000001 != selected_member_)
        {
            throw eprosima::fastcdr::exception::BadParamException("This member has not been selected");
        }

        return m_octet_value;
    }


    /*!
     * @brief This function sets a value in member long_value
     * @param _long_value New value for member long_value
     */
    eProsima_user_DllExport void long_value(
            int32_t _long_value)
    {
        long_value_() = _long_value;
        m__d = 2;
    }

    /*!
     * @brief This function returns the value of member long_value
     * @return Value of member long_value
     * @exception eprosima::fastcdr::exception::BadParamException This exception is thrown if the requested union member is not the current selection.
     */
    eProsima_user_DllExport int32_t long_value() const
    {
        if (0x00000002 != selected_member_)
        {
            throw eprosima::fastcdr::exception::BadParamException("This member has not been selected");
        }

        return m_long_value;
    }

    /*!
     * @brief This function returns a reference to member long_value
     * @return Reference to member long_value
     * @exception eprosima::fastcdr::exception::BadParamException This exception is thrown if the requested union member is not the current selection.
     */
    eProsima_user_DllExport int32_t& long_value()
    {
        if (0x00000002 != selected_member_)
        {
            throw eprosima::fastcdr::exception::BadParamException("This member has not been selected");
        }

        return m_long_value;
    }


    /*!
     * @brief This function copies the value in member string_value
     * @param _string_value New value to be copied in member string_value
     */
    eProsima_user_DllExport void string_value(
            const std::string& _string_value)
    {
        string_value_() = _string_value;
        m__d = 3;
    }

    /*!
     * @brief This function moves the value in member string_value
     * @param _string_value New value to be moved in member string_value
     */
    eProsima_user_DllExport void string_value(
            std::string&& _string_value)
    {
        string_value_() = _string_value;
        m__d = 3;
    }

    /*!
     * @brief This function returns a constant reference to member string_value
     * @return Constant reference to member string_value
     * @exception eprosima::fastcdr::exception::BadParamException This exception is thrown if the requested union member is not the current selection.
     */
    eProsima_user_DllExport const std::string& string_value() const
    {
        if (0x00000003 != selected_member_)
        {
            throw eprosima::fastcdr::exception::BadParamException("This member has not been selected");
        }

        return m_string_value;
    }

    /*!
     * @brief This function returns a reference to member string_value
     * @return Reference to member string_value
     * @exception eprosima::fastcdr::exception::BadParamException This exception is thrown if the requested union member is not the current selection.
     */
    eProsima_user_DllExport std::string& string_value()
    {
        if (0x00000003 != selected_member_)
        {
            throw eprosima::fastcdr::exception::BadParamException("This member has not been selected");
        }

        return m_string_value;
    }


    void _default()
    {
        if (member_destructor_)
        {
            member_destructor_();
        }

        selected_member_ = 0x0FFFFFFFu;
    }


private:

            uint8_t& octet_value_()
            {
                if (0x00000001 != selected_member_)
                {
                    if (member_destructor_)
                    {
                        member_destructor_();
                    }

                    selected_member_ = 0x00000001;
                    member_destructor_ = nullptr;
                    m_octet_value = {0};
    ;
                }

                return m_octet_value;
            }

            int32_t& long_value_()
            {
                if (0x00000002 != selected_member_)
                {
                    if (member_destructor_)
                    {
                        member_destructor_();
                    }

                    selected_member_ = 0x00000002;
                    member_destructor_ = nullptr;
                    m_long_value = {0};
    ;
                }

                return m_long_value;
            }

            std::string& string_value_()
            {
                if (0x00000003 != selected_member_)
                {
                    if (member_destructor_)
                    {
                        member_destructor_();
                    }

                    selected_member_ = 0x00000003;
                    member_destructor_ = [&]() {m_string_value.~basic_string();};
                    new(&m_string_value) std::string();
    ;
                }

                return m_string_value;
            }


    int32_t m__d {0};

    union
    {
        uint8_t m_octet_value;
        int32_t m_long_value;
        std::string m_string_value;
    };

    uint32_t selected_member_ {0x0FFFFFFFu};

    std::function<void()> member_destructor_;
};
/*!
 * @brief This class represents the structure union_struct defined by the user in the IDL file.
 * @ingroup union_struct
 */
class union_struct
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport union_struct()
    {
    }

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~union_struct()
    {
    }

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object union_struct that will be copied.
     */
    eProsima_user_DllExport union_struct(
            const union_struct& x)
    {
                    m_index = x.m_index;

                    m_union_value = x.m_union_value;

    }

    /*!
     * @brief Move constructor.
     * @param x Reference to the object union_struct that will be copied.
     */
    eProsima_user_DllExport union_struct(
            union_struct&& x) noexcept
    {
        m_index = x.m_index;
        m_union_value = std::move(x.m_union_value);
    }

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object union_struct that will be copied.
     */
    eProsima_user_DllExport union_struct& operator =(
            const union_struct& x)
    {

                    m_index = x.m_index;

                    m_union_value = x.m_union_value;

        return *this;
    }

    /*!
     * @brief Move assignment.
     * @param x Reference to the object union_struct that will be copied.
     */
    eProsima_user_DllExport union_struct& operator =(
            union_struct&& x) noexcept
    {

        m_index = x.m_index;
        m_union_value = std::move(x.m_union_value);
        return *this;
    }

    /*!
     * @brief Comparison operator.
     * @param x union_struct object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const union_struct& x) const
    {
        return (m_index == x.m_index &&
           m_union_value == x.m_union_value);
    }

    /*!
     * @brief Comparison operator.
     * @param x union_struct object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const union_struct& x) const
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
     * @brief This function copies the value in member union_value
     * @param _union_value New value to be copied in member union_value
     */
    eProsima_user_DllExport void union_value(
            const MyUnion& _union_value)
    {
        m_union_value = _union_value;
    }

    /*!
     * @brief This function moves the value in member union_value
     * @param _union_value New value to be moved in member union_value
     */
    eProsima_user_DllExport void union_value(
            MyUnion&& _union_value)
    {
        m_union_value = std::move(_union_value);
    }

    /*!
     * @brief This function returns a constant reference to member union_value
     * @return Constant reference to member union_value
     */
    eProsima_user_DllExport const MyUnion& union_value() const
    {
        return m_union_value;
    }

    /*!
     * @brief This function returns a reference to member union_value
     * @return Reference to member union_value
     */
    eProsima_user_DllExport MyUnion& union_value()
    {
        return m_union_value;
    }



private:

    uint32_t m_index{0};
    MyUnion m_union_value;

};

#endif // _FAST_DDS_GENERATED_UNION_STRUCT_HPP_


