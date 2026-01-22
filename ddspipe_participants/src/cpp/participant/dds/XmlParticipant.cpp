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

#include <fastdds/dds/core/ReturnCode.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantExtendedQos.hpp>

#include <cpp_utils/Log.hpp>
#include <cpp_utils/exception/ConfigurationException.hpp>
#include <cpp_utils/exception/InitializationException.hpp>

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
        const std::shared_ptr<core::DiscoveryDatabase>& discovery_database,
        bool has_filter)
    : CommonParticipant(participant_configuration, payload_pool, discovery_database, has_filter) // TODO. danip
    , xml_specific_configuration_(*reinterpret_cast<XmlParticipantConfiguration*>(configuration_.get()))
    , has_filter_(has_filter)
{
    fastdds::dds::DomainParticipantExtendedQos extended_qos;
    if (xml_specific_configuration_.participant_profile.is_set() &&
            fastdds::dds::RETCODE_OK ==
            fastdds::dds::DomainParticipantFactory::get_instance()->get_participant_extended_qos_from_profile(
                xml_specific_configuration_.participant_profile.get_value(),
                extended_qos))
    {
        configuration_->domain = extended_qos.domainId();
    }
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
        EPROSIMA_LOG_WARNING(
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
        std::shared_ptr<core::IReader> ret =
            CommonParticipant::create_reader(topic);

        // if (has_filter_)
        // {
        //     // Check if the topic has a 'content_topicfilter' filter active
        //     auto content_topicfilter_it = topic_filter_dict_.find(topic.topic_name());
        //     std::string expression = "";
        //     if(content_topicfilter_it != topic_filter_dict_.end())
        //     {
        //         expression = content_topicfilter_it->second;
        //     }
        //     // Update content_topicfilter expression
        //     //  in this function, the reader is enabled
        //     //  (to ensure safety updates of filters)
        //     ret->update_content_topic_filter(expression);

        //     // Update the partitions
        //     ret->update_partitions(partition_filter_set_);
        // }

        return ret;
    }
    catch (const utils::InitializationException& e)
    {
        EPROSIMA_LOG_WARNING(
            DDSPIPE_XMLPARTICIPANT,
            e.what()
                << ". Execution continue but this topic will not be subscribed in Participant " << id() << ".");
        return std::make_shared<BlankReader>();
    }
}

fastdds::dds::DomainParticipantQos XmlParticipant::reckon_participant_qos_() const
{
    fastdds::dds::DomainParticipantQos qos = CommonParticipant::reckon_participant_qos_();

    // Use the participant's profile if it has been set
    if (xml_specific_configuration_.participant_profile.is_set())
    {
        auto res = fastdds::dds::DomainParticipantFactory::get_instance()->get_participant_qos_from_profile(
            xml_specific_configuration_.participant_profile.get_value(),
            qos
            );

        if (res != fastdds::dds::RETCODE_OK)
        {
            throw utils::ConfigurationException(STR_ENTRY
                          << "Participant profile <" << xml_specific_configuration_.participant_profile.get_value()
                          << "> does not exist.");
        }
    }

    return qos;
}

// // TODO. danip
// void XmlParticipant::update_filters(
//         const int flag,
//         std::set<std::string> partitions,
//         const std::string& topic_name,
//         const std::string& expression)
// {
//     // nothing

//     // if (flag == 0)
//     // {
//     //     // partitions
//     //     partition_filter_set_ = partitions;
//     // }
//     // else
//     // {
//     //     // content_topicfilter
//     //     topic_filter_dict_[topic_name] = expression;
//     // }
// }

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
