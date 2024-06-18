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
 * @file union_struct.cpp
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

#include "union_struct.h"

#if FASTCDR_VERSION_MAJOR == 1

#include "union_structTypeObject.h"

#include <fastcdr/Cdr.h>


#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>

namespace helper { namespace internal {

enum class Size {
    UInt8,
    UInt16,
    UInt32,
    UInt64,
};

constexpr Size get_size(int s) {
    return (s <= 8 ) ? Size::UInt8:
           (s <= 16) ? Size::UInt16:
           (s <= 32) ? Size::UInt32: Size::UInt64;
}

template<Size s>
struct FindTypeH;

template<>
struct FindTypeH<Size::UInt8> {
    using type = std::uint8_t;
};

template<>
struct FindTypeH<Size::UInt16> {
    using type = std::uint16_t;
};

template<>
struct FindTypeH<Size::UInt32> {
    using type = std::uint32_t;
};

template<>
struct FindTypeH<Size::UInt64> {
    using type = std::uint64_t;
};
}

template<int S>
struct FindType {
    using type = typename internal::FindTypeH<internal::get_size(S)>::type;
};
}

#define union_struct_max_cdr_typesize 276ULL;





MyUnion::MyUnion()
{
    m__d = 1;
    // octet m_octet_value
    m_octet_value = 0;
    // long m_long_value
    m_long_value = 0;
    // /type_d() m_string_value

}

MyUnion::~MyUnion()
{
}

MyUnion::MyUnion(
        const MyUnion& x)
{
    m__d = x.m__d;

    switch(m__d)
    {
        case 1:
        m_octet_value = x.m_octet_value;
        break;


        case 2:
        m_long_value = x.m_long_value;
        break;


        case 3:
        m_string_value = x.m_string_value;
        break;

        default:
        break;
    }
}

MyUnion::MyUnion(
        MyUnion&& x) noexcept
{
    m__d = x.m__d;

    switch(m__d)
    {
        case 1:
        m_octet_value = x.m_octet_value;
        break;


        case 2:
        m_long_value = x.m_long_value;
        break;


        case 3:
        m_string_value = std::move(x.m_string_value);

        break;

        default:
        break;
    }
}

MyUnion& MyUnion::operator =(
        const MyUnion& x)
{
    m__d = x.m__d;

    switch(m__d)
    {
        case 1:
        m_octet_value = x.m_octet_value;
        break;


        case 2:
        m_long_value = x.m_long_value;
        break;


        case 3:
        m_string_value = x.m_string_value;
        break;

        default:
        break;
    }

    return *this;
}

MyUnion& MyUnion::operator =(
        MyUnion&& x) noexcept
{
    m__d = x.m__d;

    switch(m__d)
    {
        case 1:
        m_octet_value = x.m_octet_value;
        break;


        case 2:
        m_long_value = x.m_long_value;
        break;


        case 3:
        m_string_value = std::move(x.m_string_value);

        break;

        default:
        break;
    }

    return *this;
}

bool MyUnion::operator ==(
        const MyUnion& x) const
{
    if (m__d != x.m__d)
    {
        return false;
    }

    switch(m__d)
    {
        case 1:
            return (m_octet_value == x.m_octet_value);
            break;


        case 2:
            return (m_long_value == x.m_long_value);
            break;


        case 3:
            return (m_string_value == x.m_string_value);
            break;

        default:
        break;
    }
    return false;
}

bool MyUnion::operator !=(
        const MyUnion& x) const
{
    return !(*this == x);
}

void MyUnion::_d(
        int32_t __d)
{
    bool b = false;

    switch(m__d)
    {
        case 1:
        switch(__d)
        {
            case 1:
            b = true;
            break;
            default:
            break;
        }
        break;


        case 2:
        switch(__d)
        {
            case 2:
            b = true;
            break;
            default:
            break;
        }
        break;


        case 3:
        switch(__d)
        {
            case 3:
            b = true;
            break;
            default:
            break;
        }
        break;

    }

    if(!b)
    {
        throw BadParamException("Discriminator doesn't correspond with the selected union member");
    }

    m__d = __d;
}

int32_t MyUnion::_d() const
{
    return m__d;
}

int32_t& MyUnion::_d()
{
    return m__d;
}

void MyUnion::octet_value(
        uint8_t _octet_value)
{
    m_octet_value = _octet_value;
    m__d = 1;

}

uint8_t MyUnion::octet_value() const
{
    bool b = false;

    switch(m__d)
    {
        case 1:
        b = true;
        break;
        default:
        break;
    }

    if(!b)
    {
        throw BadParamException("This member has not been selected");
    }

    return m_octet_value;
}

uint8_t& MyUnion::octet_value()
{
    bool b = false;

    switch(m__d)
    {
        case 1:
        b = true;
        break;
        default:
        break;
    }

    if(!b)
    {
        throw BadParamException("This member has not been selected");
    }

    return m_octet_value;
}


void MyUnion::long_value(
        int32_t _long_value)
{
    m_long_value = _long_value;
    m__d = 2;

}

int32_t MyUnion::long_value() const
{
    bool b = false;

    switch(m__d)
    {
        case 2:
        b = true;
        break;
        default:
        break;
    }

    if(!b)
    {
        throw BadParamException("This member has not been selected");
    }

    return m_long_value;
}

int32_t& MyUnion::long_value()
{
    bool b = false;

    switch(m__d)
    {
        case 2:
        b = true;
        break;
        default:
        break;
    }

    if(!b)
    {
        throw BadParamException("This member has not been selected");
    }

    return m_long_value;
}


void MyUnion::string_value(
        const std::string& _string_value)
{
    m_string_value = _string_value;
    m__d = 3;

}

void MyUnion::string_value(
        std::string&& _string_value)
{
    m_string_value = std::move(_string_value);
    m__d = 3;

}

const std::string& MyUnion::string_value() const
{
    bool b = false;

    switch(m__d)
    {
        case 3:
        b = true;
        break;
        default:
        break;
    }

    if(!b)
    {
        throw BadParamException("This member has not been selected");
    }

    return m_string_value;
}

std::string& MyUnion::string_value()
{
    bool b = false;

    switch(m__d)
    {
        case 3:
        b = true;
        break;
        default:
        break;
    }

    if(!b)
    {
        throw BadParamException("This member has not been selected");
    }

    return m_string_value;
}


// TODO(Ricardo) Review
size_t MyUnion::getCdrSerializedSize(
        const MyUnion& data,
        size_t current_alignment)
{
    (void)data;
    size_t initial_alignment = current_alignment;

    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);



    switch(data.m__d)
    {
        case 1:
        current_alignment += 1 + eprosima::fastcdr::Cdr::alignment(current_alignment, 1);

        break;


        case 2:
        current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);

        break;


        case 3:
        current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4) + data.string_value().size() + 1;

        break;

        default:
        break;
    }

    return current_alignment - initial_alignment;
}


void MyUnion::serialize(
        eprosima::fastcdr::Cdr& scdr) const
{
    scdr << m__d;

    switch(m__d)
    {
        case 1:
        scdr << m_octet_value;

        break;


        case 2:
        scdr << m_long_value;

        break;


        case 3:
        scdr << m_string_value.c_str();

        break;

        default:
        break;
    }
}

void MyUnion::deserialize(
        eprosima::fastcdr::Cdr& dcdr)
{
    dcdr >> m__d;


    switch(m__d)
    {
        case 1:
        dcdr >> m_octet_value;


        break;


        case 2:
        dcdr >> m_long_value;


        break;


        case 3:
        dcdr >> m_string_value;


        break;

        default:
        break;
    }
}





union_struct::union_struct()
{
    // unsigned long m_index
    m_index = 0;
    // MyUnion m_union_value


    // Just to register all known types
    registerunion_structTypes();
}

union_struct::~union_struct()
{
}

union_struct::union_struct(
        const union_struct& x)
{
    m_index = x.m_index;


    m_union_value = x.m_union_value;

}

union_struct::union_struct(
        union_struct&& x) noexcept
{
    m_index = x.m_index;


    m_union_value = std::move(x.m_union_value);

}

union_struct& union_struct::operator =(
        const union_struct& x)
{
    m_index = x.m_index;


    m_union_value = x.m_union_value;

    return *this;
}

union_struct& union_struct::operator =(
        union_struct&& x) noexcept
{
    m_index = x.m_index;


    m_union_value = std::move(x.m_union_value);

    return *this;
}

bool union_struct::operator ==(
        const union_struct& x) const
{
    return (m_index == x.m_index &&
           m_union_value == x.m_union_value);
}

bool union_struct::operator !=(
        const union_struct& x) const
{
    return !(*this == x);
}

size_t union_struct::getMaxCdrSerializedSize(
        size_t current_alignment)
{
    static_cast<void>(current_alignment);
    return union_struct_max_cdr_typesize;
}

size_t union_struct::getCdrSerializedSize(
        const union_struct& data,
        size_t current_alignment)
{
    (void)data;
    size_t initial_alignment = current_alignment;

    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);


    current_alignment += MyUnion::getCdrSerializedSize(data.union_value(), current_alignment);


    return current_alignment - initial_alignment;
}


void union_struct::serialize(
        eprosima::fastcdr::Cdr& scdr) const
{
    scdr << m_index;

    scdr << m_union_value;

}

void union_struct::deserialize(
        eprosima::fastcdr::Cdr& dcdr)
{
    dcdr >> m_index;



    dcdr >> m_union_value;


}


bool union_struct::isKeyDefined()
{
    return false;
}

void union_struct::serializeKey(
        eprosima::fastcdr::Cdr& scdr) const
{
    (void) scdr;
}

/*!
 * @brief This function sets a value in member index
 * @param _index New value for member index
 */
void union_struct::index(
        uint32_t _index)
{
    m_index = _index;
}

/*!
 * @brief This function returns the value of member index
 * @return Value of member index
 */
uint32_t union_struct::index() const
{
    return m_index;
}

/*!
 * @brief This function returns a reference to member index
 * @return Reference to member index
 */
uint32_t& union_struct::index()
{
    return m_index;
}


/*!
 * @brief This function copies the value in member union_value
 * @param _union_value New value to be copied in member union_value
 */
void union_struct::union_value(
        const MyUnion& _union_value)
{
    m_union_value = _union_value;
}

/*!
 * @brief This function moves the value in member union_value
 * @param _union_value New value to be moved in member union_value
 */
void union_struct::union_value(
        MyUnion&& _union_value)
{
    m_union_value = std::move(_union_value);
}

/*!
 * @brief This function returns a constant reference to member union_value
 * @return Constant reference to member union_value
 */
const MyUnion& union_struct::union_value() const
{
    return m_union_value;
}

/*!
 * @brief This function returns a reference to member union_value
 * @return Reference to member union_value
 */
MyUnion& union_struct::union_value()
{
    return m_union_value;
}




#endif // FASTCDR_VERSION_MAJOR == 1