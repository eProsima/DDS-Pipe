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

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>
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

    participant_ = fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(
        configuration->publish.domain, pqos);

    if (participant_ == nullptr)
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

    type.register_type(participant_);

    // Create the publisher
    publisher_ = participant_->create_publisher(fastdds::dds::PUBLISHER_QOS_DEFAULT, nullptr);

    if (publisher_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Publisher for Participant " <<
                      participant_->get_qos().name() << ".");
    }

    // Create the topic
    topic_ = participant_->create_topic(configuration->publish.topic_name,
                    type.get_type_name(), fastdds::dds::TOPIC_QOS_DEFAULT);

    if (topic_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating Topic " << configuration->publish.topic_name <<
                      " for Participant " << participant_->guid() << ".");
    }

    // Create the writer
    writer_ = publisher_->create_datawriter(topic_, fastdds::dds::DATAWRITER_QOS_DEFAULT);

    if (writer_ == nullptr)
    {
        throw utils::InitializationException(
                  utils::Formatter() << "Error creating DataWriter for Participant " <<
                      participant_->guid() << " in topic " << topic_ << ".");
    }
}

DdsLogConsumer::~DdsLogConsumer()
{
    if (writer_ != nullptr)
    {
        publisher_->delete_datawriter(writer_);
    }

    if (topic_ != nullptr)
    {
        participant_->delete_topic(topic_);
    }

    if (publisher_ != nullptr)
    {
        participant_->delete_publisher(publisher_);
    }

    if (participant_ != nullptr)
    {
        fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(participant_);
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

    LogEntry log_entry;
    log_entry.event(event);
    log_entry.kind(get_log_entry_kind_(entry.kind));
    log_entry.category(entry.context.category);
    log_entry.message(entry.message);
    log_entry.timestamp(entry.timestamp);

    writer_->write(&log_entry);
}

constexpr Kind DdsLogConsumer::get_log_entry_kind_(
        const utils::Log::Kind kind) const noexcept
{
    switch (kind)
    {
        case utils::Log::Kind::Error:
            return Kind::Error;

        case utils::Log::Kind::Warning:
            return Kind::Warning;

        case utils::Log::Kind::Info:
        default:
            return Kind::Info;
    }
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
