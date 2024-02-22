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
 * @file basic_struct.cpp
 * This source file contains the implementation of the described types in the IDL file.
 *
 * This file was generated by the tool fastddsgen.
 */

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace {
char dummy;
}  // namespace
#endif  // _WIN32

#include "basic_struct.h"

#if FASTCDR_VERSION_MAJOR > 1

#include "basic_structTypeObject.h"

#include <fastcdr/Cdr.h>


#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>




TheOtherObject::TheOtherObject()
{
    // Just to register all known types
    registerbasic_structTypes();
}

TheOtherObject::~TheOtherObject()
{
}

TheOtherObject::TheOtherObject(
        const TheOtherObject& x)
{
    m_some_num = x.m_some_num;
}

TheOtherObject::TheOtherObject(
        TheOtherObject&& x) noexcept
{
    m_some_num = x.m_some_num;
}

TheOtherObject& TheOtherObject::operator =(
        const TheOtherObject& x)
{

    m_some_num = x.m_some_num;
    return *this;
}

TheOtherObject& TheOtherObject::operator =(
        TheOtherObject&& x) noexcept
{

    m_some_num = x.m_some_num;
    return *this;
}

bool TheOtherObject::operator ==(
        const TheOtherObject& x) const
{
    return (m_some_num == x.m_some_num);
}

bool TheOtherObject::operator !=(
        const TheOtherObject& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function sets a value in member some_num
 * @param _some_num New value for member some_num
 */
void TheOtherObject::some_num(
        int32_t _some_num)
{
    m_some_num = _some_num;
}

/*!
 * @brief This function returns the value of member some_num
 * @return Value of member some_num
 */
int32_t TheOtherObject::some_num() const
{
    return m_some_num;
}

/*!
 * @brief This function returns a reference to member some_num
 * @return Reference to member some_num
 */
int32_t& TheOtherObject::some_num()
{
    return m_some_num;
}




basic_struct::basic_struct()
{
    // Just to register all known types
    registerbasic_structTypes();
}

basic_struct::~basic_struct()
{
}

basic_struct::basic_struct(
        const basic_struct& x)
{
    m_sub_struct = x.m_sub_struct;
}

basic_struct::basic_struct(
        basic_struct&& x) noexcept
{
    m_sub_struct = std::move(x.m_sub_struct);
}

basic_struct& basic_struct::operator =(
        const basic_struct& x)
{

    m_sub_struct = x.m_sub_struct;
    return *this;
}

basic_struct& basic_struct::operator =(
        basic_struct&& x) noexcept
{

    m_sub_struct = std::move(x.m_sub_struct);
    return *this;
}

bool basic_struct::operator ==(
        const basic_struct& x) const
{
    return (m_sub_struct == x.m_sub_struct);
}

bool basic_struct::operator !=(
        const basic_struct& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function copies the value in member sub_struct
 * @param _sub_struct New value to be copied in member sub_struct
 */
void basic_struct::sub_struct(
        const TheOtherObject& _sub_struct)
{
    m_sub_struct = _sub_struct;
}

/*!
 * @brief This function moves the value in member sub_struct
 * @param _sub_struct New value to be moved in member sub_struct
 */
void basic_struct::sub_struct(
        TheOtherObject&& _sub_struct)
{
    m_sub_struct = std::move(_sub_struct);
}

/*!
 * @brief This function returns a constant reference to member sub_struct
 * @return Constant reference to member sub_struct
 */
const TheOtherObject& basic_struct::sub_struct() const
{
    return m_sub_struct;
}

/*!
 * @brief This function returns a reference to member sub_struct
 * @return Reference to member sub_struct
 */
TheOtherObject& basic_struct::sub_struct()
{
    return m_sub_struct;
}


// Include auxiliary functions like for serializing/deserializing.
#include "basic_structCdrAux.ipp"

#endif // FASTCDR_VERSION_MAJOR > 1