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
 * @file arrays_and_sequences.cpp
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

#include "arrays_and_sequences.h"

#if FASTCDR_VERSION_MAJOR > 1

#include "arrays_and_sequencesTypeObject.h"

#include <fastcdr/Cdr.h>


#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>




AnInternalObject::AnInternalObject()
{
    // Just to register all known types
    registerarrays_and_sequencesTypes();
}

AnInternalObject::~AnInternalObject()
{
}

AnInternalObject::AnInternalObject(
        const AnInternalObject& x)
{
    m_x = x.m_x;
    m_positive = x.m_positive;
}

AnInternalObject::AnInternalObject(
        AnInternalObject&& x) noexcept
{
    m_x = x.m_x;
    m_positive = x.m_positive;
}

AnInternalObject& AnInternalObject::operator =(
        const AnInternalObject& x)
{

    m_x = x.m_x;
    m_positive = x.m_positive;
    return *this;
}

AnInternalObject& AnInternalObject::operator =(
        AnInternalObject&& x) noexcept
{

    m_x = x.m_x;
    m_positive = x.m_positive;
    return *this;
}

bool AnInternalObject::operator ==(
        const AnInternalObject& x) const
{
    return (m_x == x.m_x &&
           m_positive == x.m_positive);
}

bool AnInternalObject::operator !=(
        const AnInternalObject& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function sets a value in member x
 * @param _x New value for member x
 */
void AnInternalObject::x(
        float _x)
{
    m_x = _x;
}

/*!
 * @brief This function returns the value of member x
 * @return Value of member x
 */
float AnInternalObject::x() const
{
    return m_x;
}

/*!
 * @brief This function returns a reference to member x
 * @return Reference to member x
 */
float& AnInternalObject::x()
{
    return m_x;
}


/*!
 * @brief This function sets a value in member positive
 * @param _positive New value for member positive
 */
void AnInternalObject::positive(
        bool _positive)
{
    m_positive = _positive;
}

/*!
 * @brief This function returns the value of member positive
 * @return Value of member positive
 */
bool AnInternalObject::positive() const
{
    return m_positive;
}

/*!
 * @brief This function returns a reference to member positive
 * @return Reference to member positive
 */
bool& AnInternalObject::positive()
{
    return m_positive;
}




arrays_and_sequences::arrays_and_sequences()
{
    // Just to register all known types
    registerarrays_and_sequencesTypes();
}

arrays_and_sequences::~arrays_and_sequences()
{
}

arrays_and_sequences::arrays_and_sequences(
        const arrays_and_sequences& x)
{
    m_unlimited_vector = x.m_unlimited_vector;
    m_limited_vector = x.m_limited_vector;
    m_limited_array = x.m_limited_array;
}

arrays_and_sequences::arrays_and_sequences(
        arrays_and_sequences&& x) noexcept
{
    m_unlimited_vector = std::move(x.m_unlimited_vector);
    m_limited_vector = std::move(x.m_limited_vector);
    m_limited_array = std::move(x.m_limited_array);
}

arrays_and_sequences& arrays_and_sequences::operator =(
        const arrays_and_sequences& x)
{

    m_unlimited_vector = x.m_unlimited_vector;
    m_limited_vector = x.m_limited_vector;
    m_limited_array = x.m_limited_array;
    return *this;
}

arrays_and_sequences& arrays_and_sequences::operator =(
        arrays_and_sequences&& x) noexcept
{

    m_unlimited_vector = std::move(x.m_unlimited_vector);
    m_limited_vector = std::move(x.m_limited_vector);
    m_limited_array = std::move(x.m_limited_array);
    return *this;
}

bool arrays_and_sequences::operator ==(
        const arrays_and_sequences& x) const
{
    return (m_unlimited_vector == x.m_unlimited_vector &&
           m_limited_vector == x.m_limited_vector &&
           m_limited_array == x.m_limited_array);
}

bool arrays_and_sequences::operator !=(
        const arrays_and_sequences& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function copies the value in member unlimited_vector
 * @param _unlimited_vector New value to be copied in member unlimited_vector
 */
void arrays_and_sequences::unlimited_vector(
        const std::vector<AnInternalObject>& _unlimited_vector)
{
    m_unlimited_vector = _unlimited_vector;
}

/*!
 * @brief This function moves the value in member unlimited_vector
 * @param _unlimited_vector New value to be moved in member unlimited_vector
 */
void arrays_and_sequences::unlimited_vector(
        std::vector<AnInternalObject>&& _unlimited_vector)
{
    m_unlimited_vector = std::move(_unlimited_vector);
}

/*!
 * @brief This function returns a constant reference to member unlimited_vector
 * @return Constant reference to member unlimited_vector
 */
const std::vector<AnInternalObject>& arrays_and_sequences::unlimited_vector() const
{
    return m_unlimited_vector;
}

/*!
 * @brief This function returns a reference to member unlimited_vector
 * @return Reference to member unlimited_vector
 */
std::vector<AnInternalObject>& arrays_and_sequences::unlimited_vector()
{
    return m_unlimited_vector;
}


/*!
 * @brief This function copies the value in member limited_vector
 * @param _limited_vector New value to be copied in member limited_vector
 */
void arrays_and_sequences::limited_vector(
        const std::vector<AnInternalObject>& _limited_vector)
{
    m_limited_vector = _limited_vector;
}

/*!
 * @brief This function moves the value in member limited_vector
 * @param _limited_vector New value to be moved in member limited_vector
 */
void arrays_and_sequences::limited_vector(
        std::vector<AnInternalObject>&& _limited_vector)
{
    m_limited_vector = std::move(_limited_vector);
}

/*!
 * @brief This function returns a constant reference to member limited_vector
 * @return Constant reference to member limited_vector
 */
const std::vector<AnInternalObject>& arrays_and_sequences::limited_vector() const
{
    return m_limited_vector;
}

/*!
 * @brief This function returns a reference to member limited_vector
 * @return Reference to member limited_vector
 */
std::vector<AnInternalObject>& arrays_and_sequences::limited_vector()
{
    return m_limited_vector;
}


/*!
 * @brief This function copies the value in member limited_array
 * @param _limited_array New value to be copied in member limited_array
 */
void arrays_and_sequences::limited_array(
        const std::array<AnInternalObject, 10>& _limited_array)
{
    m_limited_array = _limited_array;
}

/*!
 * @brief This function moves the value in member limited_array
 * @param _limited_array New value to be moved in member limited_array
 */
void arrays_and_sequences::limited_array(
        std::array<AnInternalObject, 10>&& _limited_array)
{
    m_limited_array = std::move(_limited_array);
}

/*!
 * @brief This function returns a constant reference to member limited_array
 * @return Constant reference to member limited_array
 */
const std::array<AnInternalObject, 10>& arrays_and_sequences::limited_array() const
{
    return m_limited_array;
}

/*!
 * @brief This function returns a reference to member limited_array
 * @return Reference to member limited_array
 */
std::array<AnInternalObject, 10>& arrays_and_sequences::limited_array()
{
    return m_limited_array;
}


// Include auxiliary functions like for serializing/deserializing.
#include "arrays_and_sequencesCdrAux.ipp"

#endif // FASTCDR_VERSION_MAJOR > 1