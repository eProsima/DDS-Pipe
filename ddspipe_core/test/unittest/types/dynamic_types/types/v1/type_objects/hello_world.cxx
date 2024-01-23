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
 * @file hello_world.cpp
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

#include "hello_world.h"
#include "hello_worldTypeObject.h"
#include <fastcdr/Cdr.h>

#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>

hello_world::hello_world()
{
    // m_index com.eprosima.idl.parser.typecode.PrimitiveTypeCode@1dfe2924
    m_index = 0;
    // m_message com.eprosima.idl.parser.typecode.StringTypeCode@6ebc05a6
    m_message ="";

    // Just to register all known types
    registerhello_worldTypes();
}

hello_world::~hello_world()
{


}

hello_world::hello_world(
        const hello_world& x)
{
    m_index = x.m_index;
    m_message = x.m_message;
}

hello_world::hello_world(
        hello_world&& x) noexcept
{
    m_index = x.m_index;
    m_message = std::move(x.m_message);
}

hello_world& hello_world::operator =(
        const hello_world& x)
{

    m_index = x.m_index;
    m_message = x.m_message;

    return *this;
}

hello_world& hello_world::operator =(
        hello_world&& x) noexcept
{

    m_index = x.m_index;
    m_message = std::move(x.m_message);

    return *this;
}

bool hello_world::operator ==(
        const hello_world& x) const
{

    return (m_index == x.m_index && m_message == x.m_message);
}

bool hello_world::operator !=(
        const hello_world& x) const
{
    return !(*this == x);
}

size_t hello_world::getMaxCdrSerializedSize(
        size_t current_alignment)
{
    size_t initial_alignment = current_alignment;


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4) + 255 + 1;


    return current_alignment - initial_alignment;
}

size_t hello_world::getCdrSerializedSize(
        const hello_world& data,
        size_t current_alignment)
{
    (void)data;
    size_t initial_alignment = current_alignment;


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);


    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4) + data.message().size() + 1;


    return current_alignment - initial_alignment;
}

void hello_world::serialize(
        eprosima::fastcdr::Cdr& scdr) const
{

    scdr << m_index;
    scdr << m_message.c_str();

}

void hello_world::deserialize(
        eprosima::fastcdr::Cdr& dcdr)
{

    dcdr >> m_index;
    dcdr >> m_message;
}

/*!
 * @brief This function sets a value in member index
 * @param _index New value for member index
 */
void hello_world::index(
        uint32_t _index)
{
    m_index = _index;
}

/*!
 * @brief This function returns the value of member index
 * @return Value of member index
 */
uint32_t hello_world::index() const
{
    return m_index;
}

/*!
 * @brief This function returns a reference to member index
 * @return Reference to member index
 */
uint32_t& hello_world::index()
{
    return m_index;
}

/*!
 * @brief This function copies the value in member message
 * @param _message New value to be copied in member message
 */
void hello_world::message(
        const std::string& _message)
{
    m_message = _message;
}

/*!
 * @brief This function moves the value in member message
 * @param _message New value to be moved in member message
 */
void hello_world::message(
        std::string&& _message)
{
    m_message = std::move(_message);
}

/*!
 * @brief This function returns a constant reference to member message
 * @return Constant reference to member message
 */
const std::string& hello_world::message() const
{
    return m_message;
}

/*!
 * @brief This function returns a reference to member message
 * @return Reference to member message
 */
std::string& hello_world::message()
{
    return m_message;
}

size_t hello_world::getKeyMaxCdrSerializedSize(
        size_t current_alignment)
{
    size_t current_align = current_alignment;





    return current_align;
}

bool hello_world::isKeyDefined()
{
    return false;
}

void hello_world::serializeKey(
        eprosima::fastcdr::Cdr& scdr) const
{
    (void) scdr;

}
