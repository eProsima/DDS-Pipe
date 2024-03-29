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
 * @file float_bounded_sequence.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool fastddsgen.
 */

#include <fastcdr/config.h>
#include "float_bounded_sequencev1.h"

#if FASTCDR_VERSION_MAJOR > 1

#ifndef _FAST_DDS_GENERATED_FLOAT_BOUNDED_SEQUENCE_H_
#define _FAST_DDS_GENERATED_FLOAT_BOUNDED_SEQUENCE_H_

#include <array>
#include <bitset>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <fastcdr/cdr/fixed_size_string.hpp>
#include <fastcdr/xcdr/external.hpp>
#include <fastcdr/xcdr/optional.hpp>



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
#if defined(FLOAT_BOUNDED_SEQUENCE_SOURCE)
#define FLOAT_BOUNDED_SEQUENCE_DllAPI __declspec( dllexport )
#else
#define FLOAT_BOUNDED_SEQUENCE_DllAPI __declspec( dllimport )
#endif // FLOAT_BOUNDED_SEQUENCE_SOURCE
#else
#define FLOAT_BOUNDED_SEQUENCE_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define FLOAT_BOUNDED_SEQUENCE_DllAPI
#endif // _WIN32

namespace eprosima {
namespace fastcdr {
class Cdr;
class CdrSizeCalculator;
} // namespace fastcdr
} // namespace eprosima







/*!
 * @brief This class represents the structure float_bounded_sequence defined by the user in the IDL file.
 * @ingroup float_bounded_sequence
 */
class float_bounded_sequence
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport float_bounded_sequence();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~float_bounded_sequence();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object float_bounded_sequence that will be copied.
     */
    eProsima_user_DllExport float_bounded_sequence(
            const float_bounded_sequence& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object float_bounded_sequence that will be copied.
     */
    eProsima_user_DllExport float_bounded_sequence(
            float_bounded_sequence&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object float_bounded_sequence that will be copied.
     */
    eProsima_user_DllExport float_bounded_sequence& operator =(
            const float_bounded_sequence& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object float_bounded_sequence that will be copied.
     */
    eProsima_user_DllExport float_bounded_sequence& operator =(
            float_bounded_sequence&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x float_bounded_sequence object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const float_bounded_sequence& x) const;

    /*!
     * @brief Comparison operator.
     * @param x float_bounded_sequence object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const float_bounded_sequence& x) const;

    /*!
     * @brief This function copies the value in member numbers
     * @param _numbers New value to be copied in member numbers
     */
    eProsima_user_DllExport void numbers(
            const std::vector<float>& _numbers);

    /*!
     * @brief This function moves the value in member numbers
     * @param _numbers New value to be moved in member numbers
     */
    eProsima_user_DllExport void numbers(
            std::vector<float>&& _numbers);

    /*!
     * @brief This function returns a constant reference to member numbers
     * @return Constant reference to member numbers
     */
    eProsima_user_DllExport const std::vector<float>& numbers() const;

    /*!
     * @brief This function returns a reference to member numbers
     * @return Reference to member numbers
     */
    eProsima_user_DllExport std::vector<float>& numbers();

private:

    std::vector<float> m_numbers;

};

#endif // _FAST_DDS_GENERATED_FLOAT_BOUNDED_SEQUENCE_H_



#endif // FASTCDR_VERSION_MAJOR > 1
