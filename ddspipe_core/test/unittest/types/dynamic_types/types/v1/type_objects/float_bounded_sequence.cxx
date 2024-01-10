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
 * @file float_bounded_sequence.cpp
 * This source file contains the definition of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace {
char dummy;
}  // namespace
#endif  // _WIN32

#include "float_bounded_sequence.h"
#include "float_bounded_sequenceTypeObject.h"
#include <fastcdr/Cdr.h>

#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>

float_bounded_sequence::float_bounded_sequence()
{
    // m_numbers com.eprosima.idl.parser.typecode.SequenceTypeCode@3fd7a715


    // Just to register all known types
    registerfloat_bounded_sequenceTypes();
}

float_bounded_sequence::~float_bounded_sequence()
{
}

float_bounded_sequence::float_bounded_sequence(
        const float_bounded_sequence& x)
{
    m_numbers = x.m_numbers;
}

float_bounded_sequence::float_bounded_sequence(
        float_bounded_sequence&& x) noexcept
{
    m_numbers = std::move(x.m_numbers);
}

float_bounded_sequence& float_bounded_sequence::operator =(
        const float_bounded_sequence& x)
{

    m_numbers = x.m_numbers;

    return *this;
}

float_bounded_sequence& float_bounded_sequence::operator =(
        float_bounded_sequence&& x) noexcept
{

    m_numbers = std::move(x.m_numbers);

    return *this;
}

bool float_bounded_sequence::operator ==(
        const float_bounded_sequence& x) const
{

    return (m_numbers == x.m_numbers);
}

bool float_bounded_sequence::operator !=(
        const float_bounded_sequence& x) const
{
    return !(*this == x);
}

size_t float_bounded_sequence::getMaxCdrSerializedSize(
        size_t current_alignment)
{
    size_t initial_alignment = current_alignment;


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);

    current_alignment += (13 * 4) + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);



    return current_alignment - initial_alignment;
}

size_t float_bounded_sequence::getCdrSerializedSize(
        const float_bounded_sequence& data,
        size_t current_alignment)
{
    (void)data;
    size_t initial_alignment = current_alignment;


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);

    if (data.numbers().size() > 0)
    {
        current_alignment += (data.numbers().size() * 4) + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);
    }



    return current_alignment - initial_alignment;
}

void float_bounded_sequence::serialize(
        eprosima::fastcdr::Cdr& scdr) const
{

    scdr << m_numbers;
}

void float_bounded_sequence::deserialize(
        eprosima::fastcdr::Cdr& dcdr)
{

    dcdr >> m_numbers;}

/*!
 * @brief This function copies the value in member numbers
 * @param _numbers New value to be copied in member numbers
 */
void float_bounded_sequence::numbers(
        const std::vector<float>& _numbers)
{
    m_numbers = _numbers;
}

/*!
 * @brief This function moves the value in member numbers
 * @param _numbers New value to be moved in member numbers
 */
void float_bounded_sequence::numbers(
        std::vector<float>&& _numbers)
{
    m_numbers = std::move(_numbers);
}

/*!
 * @brief This function returns a constant reference to member numbers
 * @return Constant reference to member numbers
 */
const std::vector<float>& float_bounded_sequence::numbers() const
{
    return m_numbers;
}

/*!
 * @brief This function returns a reference to member numbers
 * @return Reference to member numbers
 */
std::vector<float>& float_bounded_sequence::numbers()
{
    return m_numbers;
}

size_t float_bounded_sequence::getKeyMaxCdrSerializedSize(
        size_t current_alignment)
{
    size_t current_align = current_alignment;



    return current_align;
}

bool float_bounded_sequence::isKeyDefined()
{
    return false;
}

void float_bounded_sequence::serializeKey(
        eprosima::fastcdr::Cdr& scdr) const
{
    (void) scdr;

}
