// Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <algorithm>

#include <cpp_utils/exception/UnsupportedException.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/communication/dds/DdsBridge.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

using namespace eprosima::ddspipe::core::types;

DdsBridge::DdsBridge(
        const utils::Heritable<DistributedTopic>& topic,
        const std::shared_ptr<ParticipantsDatabase>& participants_database,
        const std::shared_ptr<PayloadPool>& payload_pool,
        const std::shared_ptr<utils::SlotThreadPool>& thread_pool,
        const RoutesConfiguration& routes_config,
        const bool remove_unused_entities,
        const std::vector<core::types::ManualTopic>& manual_topics,
        const EndpointKind& endpoint_kind)
    : Bridge(participants_database, payload_pool, thread_pool)
    , topic_(topic)
    , manual_topics_(manual_topics)
{
    logDebug(DDSPIPE_DDSBRIDGE, "Creating DdsBridge " << *this << ".");

    const auto& participants_repeater_map = participants_database->get_participants_repeater_map();

    writers_in_route_ = routes_config.routes_of_readers(participants_repeater_map);
    readers_in_route_ = routes_config.routes_of_writers(participants_repeater_map);

    if (remove_unused_entities && topic->topic_discoverer() != DEFAULT_PARTICIPANT_ID)
    {
        create_endpoint(topic->topic_discoverer(), endpoint_kind);
    }
    else
    {
        for (const ParticipantId& id : participants_->get_participants_ids())
        {
            create_reader_and_its_track_nts_(id);
        }
    }

    logDebug(DDSPIPE_DDSBRIDGE, "DdsBridge " << *this << " created.");
}

DdsBridge::~DdsBridge()
{
    logDebug(DDSPIPE_DDSBRIDGE, "Destroying DdsBridge " << *this << ".");

    // Disable every Track before destruction
    disable();

    logDebug(DDSPIPE_DDSBRIDGE, "DdsBridge " << *this << " destroyed.");
}

void DdsBridge::enable() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!enabled_)
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DDSBRIDGE, "Enabling DdsBridge for topic " << topic_ << ".");

        for (auto& track_it : tracks_)
        {
            track_it.second->enable();
        }

        enabled_ = true;
    }
}

void DdsBridge::disable() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (enabled_)
    {
        EPROSIMA_LOG_INFO(DDSPIPE_DDSBRIDGE, "Disabling DdsBridge for topic " << topic_ << ".");

        for (auto& track_it : tracks_)
        {
            track_it.second->disable();
        }

        enabled_ = false;
    }
}

void DdsBridge::create_endpoint(
        const ParticipantId& participant_id,
        const EndpointKind& discovered_endpoint_kind)
{
    std::lock_guard<std::mutex> lock(mutex_);

    switch (discovered_endpoint_kind)
    {
        case EndpointKind::reader:
            create_writer_and_its_tracks_nts_(participant_id);
            break;
        case EndpointKind::writer:
            create_reader_and_its_track_nts_(participant_id);
            break;
        default:
            logError(DDSPIPE_DDSBRIDGE, "Invalid kind " << discovered_endpoint_kind << " to create an endpoint.");
            break;
    }
}

void DdsBridge::remove_endpoint(
        const ParticipantId& participant_id,
        const EndpointKind& removed_endpoint_kind)
{
    std::lock_guard<std::mutex> lock(mutex_);

    switch (removed_endpoint_kind)
    {
        case EndpointKind::reader:
            remove_writer_and_its_tracks_nts_(participant_id);
            break;
        case EndpointKind::writer:
            remove_reader_and_its_track_nts_(participant_id);
            break;
        default:
            logError(DDSPIPE_DDSBRIDGE, "Invalid kind " << removed_endpoint_kind << " to remove an endpoint.");
            break;
    }
}

void DdsBridge::create_writer_and_its_tracks_nts_(
        const ParticipantId& participant_id)
{
    assert(participant_id != DEFAULT_PARTICIPANT_ID);

    // Create the writer
    auto writer = create_writer_nts_(participant_id);

    // Save the writer
    writers_[participant_id] = writer;

    // Find or create the tracks in the writer's route
    for (const auto& id : readers_in_route_[participant_id])
    {
        if (tracks_.count(id))
        {
            // The track already exists. Add the writer.
            tracks_[id]->add_writer(participant_id, writer);
        }
        else
        {
            // The track doesn't exist. Create it.
            auto reader = create_reader_nts_(id);

            std::map<ParticipantId, std::shared_ptr<IWriter>> writers;
            writers[participant_id] = writer;

            create_track_nts_(id, reader, writers);
        }
    }
}

void DdsBridge::create_reader_and_its_track_nts_(
        const ParticipantId& participant_id)
{
    assert(participant_id != DEFAULT_PARTICIPANT_ID);

    // Create the reader
    auto reader = create_reader_nts_(participant_id);

    // Create the missing writers in the reader's route
    for (const auto& id : writers_in_route_[participant_id])
    {
        if (writers_.count(id) == 0)
        {
            // The writer doesn't exist. Create it.
            writers_[id] = create_writer_nts_(id);
        }
    }

    // Create the track
    create_track_nts_(participant_id, reader, writers_);
}

void DdsBridge::remove_writer_and_its_tracks_nts_(
        const ParticipantId& participant_id) noexcept
{
    assert(participant_id != DEFAULT_PARTICIPANT_ID);

    // Remove the writer from the tracks and remove the tracks without writers
    for (auto it = tracks_.cbegin(), next_it = it; it != tracks_.cend(); it = next_it)
    {
        ++next_it;
        const auto& track = it->second;

        track->remove_writer(participant_id);

        if (!track->has_writers())
        {
            tracks_.erase(it);
        }
    }

    // Remove the writer
    writers_.erase(participant_id);
}

void DdsBridge::remove_reader_and_its_track_nts_(
        const ParticipantId& participant_id) noexcept
{
    assert(participant_id != DEFAULT_PARTICIPANT_ID);

    // Remove the writers that don't belong to another track
    for (const auto& writer_id : writers_in_route_[participant_id])
    {
        const auto& different_track_doesnt_contain_writer = [&](const auto& track_it)
                {
                    return track_it.first == participant_id || !track_it.second->has_writer(writer_id);
                };

        if (std::all_of(tracks_.begin(), tracks_.end(), different_track_doesnt_contain_writer))
        {
            writers_.erase(writer_id);
        }
    }

    // Remove the track
    tracks_.erase(participant_id);
}

void DdsBridge::create_track_nts_(
        const ParticipantId& id,
        const std::shared_ptr<IReader>& reader,
        std::map<ParticipantId, std::shared_ptr<IWriter>>& writers)
{
    tracks_[id] = std::make_unique<Track>(
        topic_,
        id,
        std::move(reader),
        std::move(writers),
        payload_pool_,
        thread_pool_);

    if (enabled_)
    {
        tracks_[id]->enable();
    }
}

std::shared_ptr<core::IWriter> DdsBridge::create_writer_nts_(
        const ParticipantId& participant_id)
{
    // Find the participant and the topic
    std::shared_ptr<IParticipant> participant = participants_->get_participant(participant_id);
    const auto topic = create_topic_for_participant_nts_(participant);

    // Create the writer
    return participant->create_writer(*topic);
}

std::shared_ptr<core::IReader> DdsBridge::create_reader_nts_(
        const ParticipantId& participant_id)
{
    // Find the participant and the topic
    std::shared_ptr<IParticipant> participant = participants_->get_participant(participant_id);
    const auto topic = create_topic_for_participant_nts_(participant);

    // Create the reader
    return participant->create_reader(*topic);
}

utils::Heritable<DistributedTopic> DdsBridge::create_topic_for_participant_nts_(
        const std::shared_ptr<IParticipant>& participant) noexcept
{
    // Make a copy of the Topic to customize it according to the Participant's configured QoS.
    utils::Heritable<DistributedTopic> topic = topic_->copy();

    // Impose the Topic QoS that have been pre-configured for the Bridge's topic.
    // set_qos only overwrites the Topic QoS that have been set with a lower FuzzyLevel.
    // A Topic QoS set with fuzzy_level_hard (the highest FuzzyLevel) cannot be overwritten.
    // Thus, the order matters. In this case, manual_topics[0] > manual_topics[1] > participant.

    // 1. Manually Configured Topic QoS.
    for (const auto& manual_topic : manual_topics_)
    {
        const auto& participant_ids = manual_topic.second;

        if (participant_ids.empty() || participant_ids.count(participant->id()))
        {
            topic->topic_qos.set_qos(manual_topic.first->topic_qos, utils::FuzzyLevelValues::fuzzy_level_hard);
        }
    }

    // 2. Participant Topic QoS.
    topic->topic_qos.set_qos(participant->topic_qos(), utils::FuzzyLevelValues::fuzzy_level_hard);

    return topic;
}

std::ostream& operator <<(
        std::ostream& os,
        const DdsBridge& bridge)
{
    os << "DdsBridge{" << bridge.topic_ << "}";
    return os;
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
