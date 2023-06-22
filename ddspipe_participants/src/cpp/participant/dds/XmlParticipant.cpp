// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <memory>

#include <cpp_utils/exception/InitializationException.hpp>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <cpp_utils/Log.hpp>
#include <cpp_utils/exception/ConfigurationException.hpp>

#include <ddspipe_participants/participant/dds/XmlParticipant.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>
#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

XmlParticipant::XmlParticipant(
        const std::shared_ptr<XmlParticipantConfiguration>& participant_configuration,
        const std::shared_ptr<core::PayloadPool>& payload_pool,
        const std::shared_ptr<core::DiscoveryDatabase>& discovery_database)
    : CommonParticipant(participant_configuration, payload_pool, discovery_database)
    , xml_specific_configuration_(*reinterpret_cast<XmlParticipantConfiguration*>(configuration_.get()))
{
    // Do nothing
}

std::shared_ptr<core::IWriter> XmlParticipant::create_writer(
        const core::ITopic& topic)
{
    try
    {
        return CommonParticipant::create_writer(topic);
    }
    catch (const utils::InitializationException& e)
    {
        logWarning(
            DDSPIPE_XMLPARTICIPANT,
            e.what()
                << ". Execution continue but this topic will not be published in Participant " << id() << ".");
        return std::make_shared<BlankWriter>();
    }
}

std::shared_ptr<core::IReader> XmlParticipant::create_reader(
        const core::ITopic& topic)
{
    try
    {
        return CommonParticipant::create_reader(topic);
    }
    catch (const utils::InitializationException& e)
    {
        logWarning(
            DDSPIPE_XMLPARTICIPANT,
            e.what()
                << ". Execution continue but this topic will not be subscribed in Participant " << id() << ".");
        return std::make_shared<BlankReader>();
    }
}

fastdds::dds::DomainParticipantQos XmlParticipant::reckon_participant_qos_() const
{
    // NOTE: Due to the creation of the participant using overriden create_dds_participant_
    // this method is never called. However we keep it for the possible future.
    fastdds::dds::DomainParticipantQos qos = CommonParticipant::reckon_participant_qos_();

    // If participant profile have been set, use it
    if (xml_specific_configuration_.participant_profile.is_set())
    {
        auto res = fastdds::dds::DomainParticipantFactory::get_instance()->get_participant_qos_from_profile(
            xml_specific_configuration_.participant_profile.get_value(),
            qos
            );

        if (res != fastrtps::types::ReturnCode_t::ReturnCodeValue::RETCODE_OK)
        {
            throw utils::ConfigurationException(STR_ENTRY
                          << "Participant profile <" << xml_specific_configuration_.participant_profile.get_value()
                          << "> does not exist.");
        }
    }

    return qos;
}

fastdds::dds::DomainParticipant* XmlParticipant::create_dds_participant_()
{
    // Set listener mask so reader read its own messages
    fastdds::dds::StatusMask mask;
    mask << fastdds::dds::StatusMask::publication_matched();
    mask << fastdds::dds::StatusMask::subscription_matched();

    if (xml_specific_configuration_.participant_profile.is_set())
    {
        return eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant_with_profile(
            xml_specific_configuration_.participant_profile.get_value(),
            this,
            mask);
    }
    else
    {
        return CommonParticipant::create_dds_participant_();
    }
}

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
