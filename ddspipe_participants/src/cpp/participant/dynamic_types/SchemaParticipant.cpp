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

/**
 * @file SchemaParticipant.cpp
 */

#include <cpp_utils/Log.hpp>
#include <cpp_utils/types/cast.hpp>

#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>
#include <ddspipe_participants/writer/dynamic_types/SchemaWriter.hpp>
#include <ddspipe_participants/writer/dynamic_types/TypeObjectWriter.hpp>

#include <ddspipe_core/types/dynamic_types/types.hpp>

#include <ddspipe_participants/participant/dynamic_types/SchemaParticipant.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;

SchemaParticipant::SchemaParticipant(
        std::shared_ptr<ParticipantConfiguration> participant_configuration,
        std::shared_ptr<PayloadPool> payload_pool,
        std::shared_ptr<DiscoveryDatabase> discovery_database,
        std::shared_ptr<ISchemaHandler> schema_handler)
    : id_(participant_configuration->id)
    , payload_pool_(payload_pool)
    , discovery_database_(discovery_database)
    , schema_handler_(schema_handler)
{
    // Simulate that there is a reader of type object to force this track creation
    discovery_database_->add_endpoint(
        simulate_endpoint_(type_object_topic())
        );

    // Force for every topic found to create track by creating simulated readers
    // NOTE: this could change for: in DDS Pipe change that only readers create track
    discovery_database_->add_endpoint_discovered_callback(
        [this](Endpoint endpoint_discovered)
        {
            if (endpoint_discovered.is_writer() && endpoint_discovered.discoverer_participant_id != this->id())
            {
                discovery_database_->add_endpoint(
                    simulate_endpoint_(endpoint_discovered.topic)
                    );
            }
        }
        );
}

ParticipantId SchemaParticipant::id() const noexcept
{
    return id_;
}

bool SchemaParticipant::is_repeater() const noexcept
{
    return false;
}

bool SchemaParticipant::is_rtps_kind() const noexcept
{
    return false;
}

std::shared_ptr<IWriter> SchemaParticipant::create_writer(
        const ITopic& topic)
{
    if (is_type_object_topic(topic))
    {
        return std::make_shared<TypeObjectWriter>(id(), schema_handler_);
    }
    else
    {
        if (!utils::can_cast<DdsTopic>(topic))
        {
            logWarning(DDSPIPE_SCHEMA_PARTICIPANT, "Not creating Writer for topic " << topic.topic_name());
            return std::make_shared<BlankWriter>();
        }
        return std::make_shared<SchemaWriter>(id(), dynamic_cast<const DdsTopic&>(topic), payload_pool_,
                       schema_handler_);
    }
}

std::shared_ptr<IReader> SchemaParticipant::create_reader(
        const ITopic& /* topic */)
{
    return std::make_shared<BlankReader>();
}

Endpoint SchemaParticipant::simulate_endpoint_(
        const DdsTopic& topic) const
{
    Endpoint endpoint;
    endpoint.kind = EndpointKind::reader;
    endpoint.guid = Guid::new_unique_guid();
    endpoint.topic = topic;
    endpoint.discoverer_participant_id = this->id();

    return endpoint;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
