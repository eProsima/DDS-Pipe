// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

/**
 * @file DdsLogConsumer.cpp
 */

#include <map>
#include <regex>
#include <string>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include <cpp_utils/exception/InitializationException.hpp>

#include <ddspipe_core/logging/DdsLogConsumer.hpp>
#include <ddspipe_core/types/dds/DomainId.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

DdsLogConsumer::DdsLogConsumer(
        const DdsPipeLogConfiguration* configuration)
    : utils::BaseLogConsumer(configuration)
{
    // Create the participant
    fastdds::dds::DomainParticipantQos pqos;
    pqos.name("DdsLogConsumerParticipant_" + std::to_string(configuration->publish.domain));

    if (configuration->publish.publish_type)
    {
        // Send the type information
        pqos.wire_protocol().builtin.typelookup_config.use_client = false;
        pqos.wire_protocol().builtin.typelookup_config.use_server = true;
    }

    fastdds::dds::DomainParticipant* participant = fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(configuration->publish.domain, pqos);

    if (participant == nullptr)
    {
        throw utils::InitializationException(
                    utils::Formatter() << "Error creating Participant " <<
                        pqos.name() << ".");
    }

    // Register the type
    fastdds::dds::TypeSupport type(new LogEntryPubSubType());

    if (configuration->publish.publish_type)
    {
        // Publish the type
        registerLogEntryTypes();
        type->auto_fill_type_information(true);
    }

    type.register_type(participant);

    // Create the publisher
    fastdds::dds::Publisher* publisher = participant->create_publisher(fastdds::dds::PUBLISHER_QOS_DEFAULT, nullptr);

    if (publisher == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Publisher for Participant " <<
                      participant->get_qos().name() << ".");
    }

    // Create the topic
    fastdds::dds::TopicQos tqos = fastdds::dds::TOPIC_QOS_DEFAULT;
    tqos.reliability().kind = fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    tqos.durability().kind = fastdds::dds::VOLATILE_DURABILITY_QOS;

    fastdds::dds::Topic* topic = participant->create_topic(configuration->publish.topic_name, type.get_type_name(), tqos);

    if (topic == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Topic " << configuration->publish.topic_name <<
                      " for Participant " << participant->guid() << ".");
    }

    // Create the writer
    fastdds::dds::DataWriterQos wqos = fastdds::dds::DATAWRITER_QOS_DEFAULT;

    wqos.data_sharing().automatic();
    wqos.publish_mode().kind = fastdds::dds::SYNCHRONOUS_PUBLISH_MODE;
    wqos.reliability().kind = fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    wqos.durability().kind = fastdds::dds::VOLATILE_DURABILITY_QOS;

    writer_ = publisher->create_datawriter(topic, wqos);

    if (writer_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating DataWriter for Participant " <<
                      participant->guid() << " in topic " << topic << ".");
    }
}

void DdsLogConsumer::Consume(
        const utils::Log::Entry& entry)
{
    if (!accept_entry_(entry))
    {
        return;
    }

    // Extract event from message
    long event = UNDEFINED;

    // The content of log messages should be either
    // "Event | Message" or "Message"
    std::regex pattern(R"(^([^|]+)\s\|\s)");
    std::smatch match;

    if (std::regex_search(entry.message, match, pattern) && match.size() > 1)
    {
        const std::string& event_str = match.str(1);

        // For an event to be valid, it must be in the events_ map.
        // Derived classes should add their specific events to the map.
        if (events_.count(event_str))
        {
            event = events_[event_str];
        }
    }

    // Map Fast-DDS's Log kind to the LogEntry's kind
    static const std::map<utils::Log::Kind, Kind> kind_map
    {
        {utils::Log::Kind::Error, Kind::Error},
        {utils::Log::Kind::Warning, Kind::Warning},
        {utils::Log::Kind::Info, Kind::Info}
    };

    LogEntry log_entry;
    log_entry.event(event);
    log_entry.kind(kind_map.at(entry.kind));
    log_entry.category(entry.context.category);
    log_entry.message(entry.message);
    log_entry.timestamp(entry.timestamp);

    writer_->write(&log_entry);
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
