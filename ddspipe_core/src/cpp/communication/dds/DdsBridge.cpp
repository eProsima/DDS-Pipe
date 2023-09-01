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
        const ParticipantId& discoverer_participant_id)
    : Bridge(participants_database, payload_pool, thread_pool)
    , topic_(topic)
{
    logDebug(DDSPIPE_DDSBRIDGE, "Creating DdsBridge " << *this << ".");

    routes_ = routes_config();

    if (discoverer_participant_id == "builtin-participant")
    {
        for (const ParticipantId& id : participants_->get_participants_ids())
        {
            add_to_tracks(id);
        }
    }
    else
    {
        add_to_tracks(discoverer_participant_id);
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
        logInfo(DDSPIPE_DDSBRIDGE, "Enabling DdsBridge for topic " << topic_ << ".");

        // ATTENTION: reference needed or it would copy Track
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
        logInfo(DDSPIPE_DDSBRIDGE, "Disabling DdsBridge for topic " << topic_ << ".");

        // ATTENTION: reference needed or it would copy Track
        for (auto& track_it : tracks_)
        {
            track_it.second->disable();
        }

        enabled_ = false;
    }
}

void DdsBridge::add_to_tracks(
        const ParticipantId& discoverer_participant_id)
{
    // A new subscriber has been discovered.
    // Check if the writer for this participant already exists.
    for (const auto& id_to_track : tracks_)
    {
        const auto& id = id_to_track.first;
        const auto& track = id_to_track.second;

        if (track->has_writer(discoverer_participant_id))
        {
            // The writer already exists. There is nothing to do. Exit.
            return;
        }
    }

    // Create the writer.
    std::shared_ptr<IParticipant> participant = participants_->get_participant(discoverer_participant_id);

    std::map<ParticipantId, std::shared_ptr<IWriter>> id_to_writer;
    id_to_writer[discoverer_participant_id] = participant->create_writer(*topic_);

    // Create the necessary readers and tracks.
    for (const ParticipantId& id : participants_->get_participants_ids())
    {
        const auto& it = routes_.find(id);

        if (it != routes_.end() && it->second.find(discoverer_participant_id) == it->second.end())
        {
            // The Participant has a route and the discoverer is not in it.
            // There can be no changes to the tracks.
            continue;
        }

        if (id == discoverer_participant_id && !participants_->get_participant(id)->is_repeater())
        {
            // Don't connect a participant's reader and writer if the participant is not a repeater.
            continue;
        }

        // Create a copy of the writer
        std::map<ParticipantId, std::shared_ptr<IWriter>> id_to_dst_writer;
        id_to_dst_writer[discoverer_participant_id] = id_to_writer[discoverer_participant_id];

        if (tracks_.count(id))
        {
            // The track already exists. Add the writer to it.
            tracks_[id]->add_writer(discoverer_participant_id, id_to_dst_writer[discoverer_participant_id]);
        }
        else
        {
            // The track doesn't exist. Create it.
            std::shared_ptr<IParticipant> participant = participants_->get_participant(id);
            auto reader = participant->create_reader(*topic_);

            tracks_[id] = std::make_unique<Track>(
                topic_,
                id,
                std::move(reader),
                std::move(id_to_dst_writer),
                payload_pool_,
                thread_pool_);
        }

        tracks_[id]->enable();
    }
}

void DdsBridge::remove_from_tracks(
        const ParticipantId& discoverer_participant_id) noexcept
{
    for (const auto& id_to_track : tracks_)
    {
        const auto& id = id_to_track.first;
        const auto& track = id_to_track.second;

        // If the writer is in the track, remove it.
        track->remove_writer(discoverer_participant_id);

        if (track->count_writers() <= 0)
        {
            // The track doesn't have any writers. Remove it.
            tracks_.erase(id);
        }
    }
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
