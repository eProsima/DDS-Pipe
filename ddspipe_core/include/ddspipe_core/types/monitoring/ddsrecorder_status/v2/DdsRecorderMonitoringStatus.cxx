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
 * @file DdsRecorderMonitoringStatus.cpp
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

#include "DdsRecorderMonitoringStatus.h"

#if FASTCDR_VERSION_MAJOR > 1

#include <fastcdr/Cdr.h>


#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>




DdsRecorderMonitoringErrorStatus::DdsRecorderMonitoringErrorStatus()
{
}

DdsRecorderMonitoringErrorStatus::~DdsRecorderMonitoringErrorStatus()
{
}

DdsRecorderMonitoringErrorStatus::DdsRecorderMonitoringErrorStatus(
        const DdsRecorderMonitoringErrorStatus& x)
{
    m_mcap_file_creation_failure = x.m_mcap_file_creation_failure;
    m_disk_full = x.m_disk_full;
}

DdsRecorderMonitoringErrorStatus::DdsRecorderMonitoringErrorStatus(
        DdsRecorderMonitoringErrorStatus&& x) noexcept
{
    m_mcap_file_creation_failure = x.m_mcap_file_creation_failure;
    m_disk_full = x.m_disk_full;
}

DdsRecorderMonitoringErrorStatus& DdsRecorderMonitoringErrorStatus::operator =(
        const DdsRecorderMonitoringErrorStatus& x)
{

    m_mcap_file_creation_failure = x.m_mcap_file_creation_failure;
    m_disk_full = x.m_disk_full;
    return *this;
}

DdsRecorderMonitoringErrorStatus& DdsRecorderMonitoringErrorStatus::operator =(
        DdsRecorderMonitoringErrorStatus&& x) noexcept
{

    m_mcap_file_creation_failure = x.m_mcap_file_creation_failure;
    m_disk_full = x.m_disk_full;
    return *this;
}

bool DdsRecorderMonitoringErrorStatus::operator ==(
        const DdsRecorderMonitoringErrorStatus& x) const
{
    return (m_mcap_file_creation_failure == x.m_mcap_file_creation_failure &&
           m_disk_full == x.m_disk_full);
}

bool DdsRecorderMonitoringErrorStatus::operator !=(
        const DdsRecorderMonitoringErrorStatus& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function sets a value in member mcap_file_creation_failure
 * @param _mcap_file_creation_failure New value for member mcap_file_creation_failure
 */
void DdsRecorderMonitoringErrorStatus::mcap_file_creation_failure(
        bool _mcap_file_creation_failure)
{
    m_mcap_file_creation_failure = _mcap_file_creation_failure;
}

/*!
 * @brief This function returns the value of member mcap_file_creation_failure
 * @return Value of member mcap_file_creation_failure
 */
bool DdsRecorderMonitoringErrorStatus::mcap_file_creation_failure() const
{
    return m_mcap_file_creation_failure;
}

/*!
 * @brief This function returns a reference to member mcap_file_creation_failure
 * @return Reference to member mcap_file_creation_failure
 */
bool& DdsRecorderMonitoringErrorStatus::mcap_file_creation_failure()
{
    return m_mcap_file_creation_failure;
}


/*!
 * @brief This function sets a value in member disk_full
 * @param _disk_full New value for member disk_full
 */
void DdsRecorderMonitoringErrorStatus::disk_full(
        bool _disk_full)
{
    m_disk_full = _disk_full;
}

/*!
 * @brief This function returns the value of member disk_full
 * @return Value of member disk_full
 */
bool DdsRecorderMonitoringErrorStatus::disk_full() const
{
    return m_disk_full;
}

/*!
 * @brief This function returns a reference to member disk_full
 * @return Reference to member disk_full
 */
bool& DdsRecorderMonitoringErrorStatus::disk_full()
{
    return m_disk_full;
}




DdsRecorderMonitoringStatus::DdsRecorderMonitoringStatus()
    : MonitoringStatus()

{
}

DdsRecorderMonitoringStatus::~DdsRecorderMonitoringStatus()
{
}

DdsRecorderMonitoringStatus::DdsRecorderMonitoringStatus(
        const DdsRecorderMonitoringStatus& x)
    : MonitoringStatus(x)

{
    m_ddsrecorder_error_status = x.m_ddsrecorder_error_status;
}

DdsRecorderMonitoringStatus::DdsRecorderMonitoringStatus(
        DdsRecorderMonitoringStatus&& x) noexcept
    : MonitoringStatus(std::move(x))

{
    m_ddsrecorder_error_status = std::move(x.m_ddsrecorder_error_status);
}

DdsRecorderMonitoringStatus& DdsRecorderMonitoringStatus::operator =(
        const DdsRecorderMonitoringStatus& x)
{
    MonitoringStatus::operator =(x);

    m_ddsrecorder_error_status = x.m_ddsrecorder_error_status;
    return *this;
}

DdsRecorderMonitoringStatus& DdsRecorderMonitoringStatus::operator =(
        DdsRecorderMonitoringStatus&& x) noexcept
{
    MonitoringStatus::operator =(std::move(x));

    m_ddsrecorder_error_status = std::move(x.m_ddsrecorder_error_status);
    return *this;
}

bool DdsRecorderMonitoringStatus::operator ==(
        const DdsRecorderMonitoringStatus& x) const
{
    if (MonitoringStatus::operator !=(x))
        {
            return false;
        }
    return (m_ddsrecorder_error_status == x.m_ddsrecorder_error_status);
}

bool DdsRecorderMonitoringStatus::operator !=(
        const DdsRecorderMonitoringStatus& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function copies the value in member ddsrecorder_error_status
 * @param _ddsrecorder_error_status New value to be copied in member ddsrecorder_error_status
 */
void DdsRecorderMonitoringStatus::ddsrecorder_error_status(
        const DdsRecorderMonitoringErrorStatus& _ddsrecorder_error_status)
{
    m_ddsrecorder_error_status = _ddsrecorder_error_status;
}

/*!
 * @brief This function moves the value in member ddsrecorder_error_status
 * @param _ddsrecorder_error_status New value to be moved in member ddsrecorder_error_status
 */
void DdsRecorderMonitoringStatus::ddsrecorder_error_status(
        DdsRecorderMonitoringErrorStatus&& _ddsrecorder_error_status)
{
    m_ddsrecorder_error_status = std::move(_ddsrecorder_error_status);
}

/*!
 * @brief This function returns a constant reference to member ddsrecorder_error_status
 * @return Constant reference to member ddsrecorder_error_status
 */
const DdsRecorderMonitoringErrorStatus& DdsRecorderMonitoringStatus::ddsrecorder_error_status() const
{
    return m_ddsrecorder_error_status;
}

/*!
 * @brief This function returns a reference to member ddsrecorder_error_status
 * @return Reference to member ddsrecorder_error_status
 */
DdsRecorderMonitoringErrorStatus& DdsRecorderMonitoringStatus::ddsrecorder_error_status()
{
    return m_ddsrecorder_error_status;
}


// Include auxiliary functions like for serializing/deserializing.
#include "DdsRecorderMonitoringStatusCdrAux.ipp"

#endif // FASTCDR_VERSION_MAJOR > 1