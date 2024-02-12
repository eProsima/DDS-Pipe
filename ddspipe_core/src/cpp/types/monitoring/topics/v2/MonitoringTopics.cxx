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
 * @file MonitoringTopics.cpp
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

#include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopics.h>

#if FASTCDR_VERSION_MAJOR > 1

#include <fastcdr/Cdr.h>


#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>




DdsTopicData::DdsTopicData()
{
}

DdsTopicData::~DdsTopicData()
{
}

DdsTopicData::DdsTopicData(
        const DdsTopicData& x)
{
    m_participant_id = x.m_participant_id;
    m_msgs_lost = x.m_msgs_lost;
    m_msgs_received = x.m_msgs_received;
    m_frequency = x.m_frequency;
}

DdsTopicData::DdsTopicData(
        DdsTopicData&& x) noexcept
{
    m_participant_id = std::move(x.m_participant_id);
    m_msgs_lost = x.m_msgs_lost;
    m_msgs_received = x.m_msgs_received;
    m_frequency = x.m_frequency;
}

DdsTopicData& DdsTopicData::operator =(
        const DdsTopicData& x)
{

    m_participant_id = x.m_participant_id;
    m_msgs_lost = x.m_msgs_lost;
    m_msgs_received = x.m_msgs_received;
    m_frequency = x.m_frequency;
    return *this;
}

DdsTopicData& DdsTopicData::operator =(
        DdsTopicData&& x) noexcept
{

    m_participant_id = std::move(x.m_participant_id);
    m_msgs_lost = x.m_msgs_lost;
    m_msgs_received = x.m_msgs_received;
    m_frequency = x.m_frequency;
    return *this;
}

bool DdsTopicData::operator ==(
        const DdsTopicData& x) const
{
    return (m_participant_id == x.m_participant_id &&
           m_msgs_lost == x.m_msgs_lost &&
           m_msgs_received == x.m_msgs_received &&
           m_frequency == x.m_frequency);
}

bool DdsTopicData::operator !=(
        const DdsTopicData& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function copies the value in member participant_id
 * @param _participant_id New value to be copied in member participant_id
 */
void DdsTopicData::participant_id(
        const std::string& _participant_id)
{
    m_participant_id = _participant_id;
}

/*!
 * @brief This function moves the value in member participant_id
 * @param _participant_id New value to be moved in member participant_id
 */
void DdsTopicData::participant_id(
        std::string&& _participant_id)
{
    m_participant_id = std::move(_participant_id);
}

/*!
 * @brief This function returns a constant reference to member participant_id
 * @return Constant reference to member participant_id
 */
const std::string& DdsTopicData::participant_id() const
{
    return m_participant_id;
}

/*!
 * @brief This function returns a reference to member participant_id
 * @return Reference to member participant_id
 */
std::string& DdsTopicData::participant_id()
{
    return m_participant_id;
}


/*!
 * @brief This function sets a value in member msgs_lost
 * @param _msgs_lost New value for member msgs_lost
 */
void DdsTopicData::msgs_lost(
        uint32_t _msgs_lost)
{
    m_msgs_lost = _msgs_lost;
}

/*!
 * @brief This function returns the value of member msgs_lost
 * @return Value of member msgs_lost
 */
uint32_t DdsTopicData::msgs_lost() const
{
    return m_msgs_lost;
}

/*!
 * @brief This function returns a reference to member msgs_lost
 * @return Reference to member msgs_lost
 */
uint32_t& DdsTopicData::msgs_lost()
{
    return m_msgs_lost;
}


/*!
 * @brief This function sets a value in member msgs_received
 * @param _msgs_received New value for member msgs_received
 */
void DdsTopicData::msgs_received(
        uint32_t _msgs_received)
{
    m_msgs_received = _msgs_received;
}

/*!
 * @brief This function returns the value of member msgs_received
 * @return Value of member msgs_received
 */
uint32_t DdsTopicData::msgs_received() const
{
    return m_msgs_received;
}

/*!
 * @brief This function returns a reference to member msgs_received
 * @return Reference to member msgs_received
 */
uint32_t& DdsTopicData::msgs_received()
{
    return m_msgs_received;
}


/*!
 * @brief This function sets a value in member frequency
 * @param _frequency New value for member frequency
 */
void DdsTopicData::frequency(
        double _frequency)
{
    m_frequency = _frequency;
}

/*!
 * @brief This function returns the value of member frequency
 * @return Value of member frequency
 */
double DdsTopicData::frequency() const
{
    return m_frequency;
}

/*!
 * @brief This function returns a reference to member frequency
 * @return Reference to member frequency
 */
double& DdsTopicData::frequency()
{
    return m_frequency;
}




DdsTopic::DdsTopic()
{
}

DdsTopic::~DdsTopic()
{
}

DdsTopic::DdsTopic(
        const DdsTopic& x)
{
    m_name = x.m_name;
    m_type_name = x.m_type_name;
    m_type_discovered = x.m_type_discovered;
    m_type_mismatch = x.m_type_mismatch;
    m_data = x.m_data;
    m_qos_mismatch = x.m_qos_mismatch;
}

DdsTopic::DdsTopic(
        DdsTopic&& x) noexcept
{
    m_name = std::move(x.m_name);
    m_type_name = std::move(x.m_type_name);
    m_type_discovered = x.m_type_discovered;
    m_type_mismatch = x.m_type_mismatch;
    m_data = std::move(x.m_data);
    m_qos_mismatch = x.m_qos_mismatch;
}

DdsTopic& DdsTopic::operator =(
        const DdsTopic& x)
{

    m_name = x.m_name;
    m_type_name = x.m_type_name;
    m_type_discovered = x.m_type_discovered;
    m_type_mismatch = x.m_type_mismatch;
    m_data = x.m_data;
    m_qos_mismatch = x.m_qos_mismatch;
    return *this;
}

DdsTopic& DdsTopic::operator =(
        DdsTopic&& x) noexcept
{

    m_name = std::move(x.m_name);
    m_type_name = std::move(x.m_type_name);
    m_type_discovered = x.m_type_discovered;
    m_type_mismatch = x.m_type_mismatch;
    m_data = std::move(x.m_data);
    m_qos_mismatch = x.m_qos_mismatch;
    return *this;
}

bool DdsTopic::operator ==(
        const DdsTopic& x) const
{
    return (m_name == x.m_name &&
           m_type_name == x.m_type_name &&
           m_type_discovered == x.m_type_discovered &&
           m_type_mismatch == x.m_type_mismatch &&
           m_data == x.m_data &&
           m_qos_mismatch == x.m_qos_mismatch);
}

bool DdsTopic::operator !=(
        const DdsTopic& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function copies the value in member name
 * @param _name New value to be copied in member name
 */
void DdsTopic::name(
        const std::string& _name)
{
    m_name = _name;
}

/*!
 * @brief This function moves the value in member name
 * @param _name New value to be moved in member name
 */
void DdsTopic::name(
        std::string&& _name)
{
    m_name = std::move(_name);
}

/*!
 * @brief This function returns a constant reference to member name
 * @return Constant reference to member name
 */
const std::string& DdsTopic::name() const
{
    return m_name;
}

/*!
 * @brief This function returns a reference to member name
 * @return Reference to member name
 */
std::string& DdsTopic::name()
{
    return m_name;
}


/*!
 * @brief This function copies the value in member type_name
 * @param _type_name New value to be copied in member type_name
 */
void DdsTopic::type_name(
        const std::string& _type_name)
{
    m_type_name = _type_name;
}

/*!
 * @brief This function moves the value in member type_name
 * @param _type_name New value to be moved in member type_name
 */
void DdsTopic::type_name(
        std::string&& _type_name)
{
    m_type_name = std::move(_type_name);
}

/*!
 * @brief This function returns a constant reference to member type_name
 * @return Constant reference to member type_name
 */
const std::string& DdsTopic::type_name() const
{
    return m_type_name;
}

/*!
 * @brief This function returns a reference to member type_name
 * @return Reference to member type_name
 */
std::string& DdsTopic::type_name()
{
    return m_type_name;
}


/*!
 * @brief This function sets a value in member type_discovered
 * @param _type_discovered New value for member type_discovered
 */
void DdsTopic::type_discovered(
        bool _type_discovered)
{
    m_type_discovered = _type_discovered;
}

/*!
 * @brief This function returns the value of member type_discovered
 * @return Value of member type_discovered
 */
bool DdsTopic::type_discovered() const
{
    return m_type_discovered;
}

/*!
 * @brief This function returns a reference to member type_discovered
 * @return Reference to member type_discovered
 */
bool& DdsTopic::type_discovered()
{
    return m_type_discovered;
}


/*!
 * @brief This function sets a value in member type_mismatch
 * @param _type_mismatch New value for member type_mismatch
 */
void DdsTopic::type_mismatch(
        bool _type_mismatch)
{
    m_type_mismatch = _type_mismatch;
}

/*!
 * @brief This function returns the value of member type_mismatch
 * @return Value of member type_mismatch
 */
bool DdsTopic::type_mismatch() const
{
    return m_type_mismatch;
}

/*!
 * @brief This function returns a reference to member type_mismatch
 * @return Reference to member type_mismatch
 */
bool& DdsTopic::type_mismatch()
{
    return m_type_mismatch;
}


/*!
 * @brief This function copies the value in member data
 * @param _data New value to be copied in member data
 */
void DdsTopic::data(
        const std::vector<DdsTopicData>& _data)
{
    m_data = _data;
}

/*!
 * @brief This function moves the value in member data
 * @param _data New value to be moved in member data
 */
void DdsTopic::data(
        std::vector<DdsTopicData>&& _data)
{
    m_data = std::move(_data);
}

/*!
 * @brief This function returns a constant reference to member data
 * @return Constant reference to member data
 */
const std::vector<DdsTopicData>& DdsTopic::data() const
{
    return m_data;
}

/*!
 * @brief This function returns a reference to member data
 * @return Reference to member data
 */
std::vector<DdsTopicData>& DdsTopic::data()
{
    return m_data;
}


/*!
 * @brief This function sets a value in member qos_mismatch
 * @param _qos_mismatch New value for member qos_mismatch
 */
void DdsTopic::qos_mismatch(
        bool _qos_mismatch)
{
    m_qos_mismatch = _qos_mismatch;
}

/*!
 * @brief This function returns the value of member qos_mismatch
 * @return Value of member qos_mismatch
 */
bool DdsTopic::qos_mismatch() const
{
    return m_qos_mismatch;
}

/*!
 * @brief This function returns a reference to member qos_mismatch
 * @return Reference to member qos_mismatch
 */
bool& DdsTopic::qos_mismatch()
{
    return m_qos_mismatch;
}






MonitoringTopics::MonitoringTopics()
{
}

MonitoringTopics::~MonitoringTopics()
{
}

MonitoringTopics::MonitoringTopics(
        const MonitoringTopics& x)
{
    m_topics = x.m_topics;
}

MonitoringTopics::MonitoringTopics(
        MonitoringTopics&& x) noexcept
{
    m_topics = std::move(x.m_topics);
}

MonitoringTopics& MonitoringTopics::operator =(
        const MonitoringTopics& x)
{

    m_topics = x.m_topics;
    return *this;
}

MonitoringTopics& MonitoringTopics::operator =(
        MonitoringTopics&& x) noexcept
{

    m_topics = std::move(x.m_topics);
    return *this;
}

bool MonitoringTopics::operator ==(
        const MonitoringTopics& x) const
{
    return (m_topics == x.m_topics);
}

bool MonitoringTopics::operator !=(
        const MonitoringTopics& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function copies the value in member topics
 * @param _topics New value to be copied in member topics
 */
void MonitoringTopics::topics(
        const std::vector<DdsTopic>& _topics)
{
    m_topics = _topics;
}

/*!
 * @brief This function moves the value in member topics
 * @param _topics New value to be moved in member topics
 */
void MonitoringTopics::topics(
        std::vector<DdsTopic>&& _topics)
{
    m_topics = std::move(_topics);
}

/*!
 * @brief This function returns a constant reference to member topics
 * @return Constant reference to member topics
 */
const std::vector<DdsTopic>& MonitoringTopics::topics() const
{
    return m_topics;
}

/*!
 * @brief This function returns a reference to member topics
 * @return Reference to member topics
 */
std::vector<DdsTopic>& MonitoringTopics::topics()
{
    return m_topics;
}


// Include auxiliary functions like for serializing/deserializing.
#include <ddspipe_core/types/monitoring/topics/v2/MonitoringTopicsCdrAux.ipp>

#endif // FASTCDR_VERSION_MAJOR > 1
