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
 * @file enum_struct.cpp
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

#include "enum_struct.h"

#if FASTCDR_VERSION_MAJOR > 1

#include "enum_structTypeObject.h"

#include <fastcdr/Cdr.h>


#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>




enum_struct::enum_struct()
{
    // Just to register all known types
    registerenum_structTypes();
}

enum_struct::~enum_struct()
{
}

enum_struct::enum_struct(
        const enum_struct& x)
{
    m_index = x.m_index;
    m_enum_value = x.m_enum_value;
}

enum_struct::enum_struct(
        enum_struct&& x) noexcept
{
    m_index = x.m_index;
    m_enum_value = x.m_enum_value;
}

enum_struct& enum_struct::operator =(
        const enum_struct& x)
{

    m_index = x.m_index;
    m_enum_value = x.m_enum_value;
    return *this;
}

enum_struct& enum_struct::operator =(
        enum_struct&& x) noexcept
{

    m_index = x.m_index;
    m_enum_value = x.m_enum_value;
    return *this;
}

bool enum_struct::operator ==(
        const enum_struct& x) const
{
    return (m_index == x.m_index &&
           m_enum_value == x.m_enum_value);
}

bool enum_struct::operator !=(
        const enum_struct& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function sets a value in member index
 * @param _index New value for member index
 */
void enum_struct::index(
        uint32_t _index)
{
    m_index = _index;
}

/*!
 * @brief This function returns the value of member index
 * @return Value of member index
 */
uint32_t enum_struct::index() const
{
    return m_index;
}

/*!
 * @brief This function returns a reference to member index
 * @return Reference to member index
 */
uint32_t& enum_struct::index()
{
    return m_index;
}


/*!
 * @brief This function sets a value in member enum_value
 * @param _enum_value New value for member enum_value
 */
void enum_struct::enum_value(
        ColorEnum _enum_value)
{
    m_enum_value = _enum_value;
}

/*!
 * @brief This function returns the value of member enum_value
 * @return Value of member enum_value
 */
ColorEnum enum_struct::enum_value() const
{
    return m_enum_value;
}

/*!
 * @brief This function returns a reference to member enum_value
 * @return Reference to member enum_value
 */
ColorEnum& enum_struct::enum_value()
{
    return m_enum_value;
}


// Include auxiliary functions like for serializing/deserializing.
#include "enum_structCdrAux.ipp"

#endif // FASTCDR_VERSION_MAJOR > 1
