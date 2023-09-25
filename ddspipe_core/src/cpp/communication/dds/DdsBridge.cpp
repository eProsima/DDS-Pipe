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
        const RoutesConfiguration& routes_config)
    : Bridge(participants_database, payload_pool, thread_pool)
    , topic_(topic)
{
    logDebug(DDSPIPE_DDSBRIDGE, "Creating DdsBridge " << *this << ".");

    std::set<ParticipantId> ids = participants_->get_participants_ids();
    auto routes = routes_config();

    // Determine which endpoints need to be created
    std::set<types::ParticipantId> writers_to_create;
    std::set<types::ParticipantId> readers_to_create;
    for (const ParticipantId& id: ids)
    {
        const auto& it = routes.find(id);
        if (it != routes.end())
        {
            const auto& src_id = it->first;
            const auto& dst_ids = it->second;
            if (dst_ids.size() != 0) // Only create reader if there are any destination writers
            {
                readers_to_create.insert(src_id);
                writers_to_create.insert(dst_ids.begin(), dst_ids.end());
            }
        }
        else
        {
            // When no route is defined, forward to all other participants (+ itself if repeater)
            auto dst_ids = ids;
            if (!participants_->get_participant(id)->is_repeater())
            {
                // Do not add writer for this participant because it is not repeater
                dst_ids.erase(id);
            }
            writers_to_create.insert(dst_ids.begin(), dst_ids.end());
            readers_to_create.insert(id);
        }
    }

    // Generate writers for each participant
    std::map<types::ParticipantId, std::shared_ptr<IWriter>> writers;
    for (const auto& id: writers_to_create)
    {
        std::shared_ptr<IParticipant> participant = participants_database->get_participant(id);
        writers[id] = participant->create_writer(*topic);
    }

    // Generate readers for each participant
    std::map<types::ParticipantId, std::shared_ptr<IReader>> readers;
    for (const auto& id: readers_to_create)
    {
        std::shared_ptr<IParticipant> participant = participants_database->get_participant(id);
        readers[id] = participant->create_reader(*topic);
    }

    // Generate tracks
    for (const ParticipantId& id: ids)
    {
        std::map<ParticipantId, std::shared_ptr<IWriter>> dst_writers;
        auto it = routes.find(id);
        if (it != routes.end())
        {
            // Custom route available for this participant
            for (const auto& writer_id : it->second)
            {
                dst_writers[writer_id] = writers[writer_id];
            }

            // Do not create track if no destination writers
            if (dst_writers.size() == 0)
            {
                continue;
            }
        }
        else
        {
            // Use default forwarding route (receiver participant to all others)
            dst_writers = writers;

            // Remove this Track source participant if not repeater
            if (!participants_->get_participant(id)->is_repeater())
            {
                dst_writers.erase(id);
            }
        }

        // This insert is required as there is no copy method for Track
        // Tracks are always created disabled and then enabled with Bridge enable() method
        tracks_[id] =
                std::make_unique<Track>(
            topic,
            id,
            std::move(readers[id]),
            std::move(dst_writers),
            payload_pool,
            thread_pool);
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
