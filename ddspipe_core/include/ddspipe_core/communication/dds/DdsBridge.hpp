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

#pragma once

#include <mutex>

#include <ddspipe_core/communication/Bridge.hpp>
#include <ddspipe_core/communication/dds/Track.hpp>
#include <ddspipe_core/configuration/RoutesConfiguration.hpp>
#include <ddspipe_core/types/topic/dds/DistributedTopic.hpp>
#include <ddspipe_core/types/topic/filter/ManualTopic.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * Bridge object manages the communication of a \c DistributedTopic.
 * It could be seen as a channel of communication as a DDS Topic, whit several Participants that
 * could publish or subscribe in this specific Topic.
 *
 * It contains N \c Tracks that will manage each direction of the communication,
 * being N the number of Participants of this communication channel.
 */
class DdsBridge : public Bridge
{
public:

    /**
     * Bridge constructor by required values
     *
     * In Bridge construction, the inside \c Tracks are created.
     * In Bridge construction, a Writer and a Reader are created for each Participant.
     *
     * @param topic: Topic of which this Bridge manages communication
     * @param participant_database: Collection of Participants to manage communication
     * @param payload_pool: Payload Pool that handles the reservation/release of payloads throughout the DDS Router
     * @param thread_pool: Shared pool of threads in charge of data transmission.
     * @param enable: Whether the Bridge should be initialized as enabled
     *
     * @throw InitializationException in case \c IWriters or \c IReaders creation fails.
     */
    DDSPIPE_CORE_DllAPI
    DdsBridge(
            const utils::Heritable<types::DistributedTopic>& topic,
            const std::shared_ptr<ParticipantsDatabase>& participants_database,
            const std::shared_ptr<PayloadPool>& payload_pool,
            const std::shared_ptr<utils::SlotThreadPool>& thread_pool,
            const RoutesConfiguration& routes_config,
            const bool remove_unused_entities,
            const std::vector<core::types::ManualTopic>& manual_topics);

    DDSPIPE_CORE_DllAPI
    ~DdsBridge();

    /**
     * Enable bridge in case it is not enabled
     * Does nothing if it is already enabled
     *
     * Thread safe
     */
    DDSPIPE_CORE_DllAPI
    void enable() noexcept override;

    /**
     * Disable bridge in case it is not enabled
     * Does nothing if it is disabled
     *
     * Thread safe
     */
    DDSPIPE_CORE_DllAPI
    void disable() noexcept override;

    /**
     * Build the IReaders and IWriters inside the bridge for the new participant,
     * and add them to the Tracks.
     *
     * Thread safe
     *
     * @param participant_id: The id of the participant who is creating the writer.
     *
     * @throw InitializationException in case \c IWriters or \c IReaders creation fails.
     */
    DDSPIPE_CORE_DllAPI
    void create_writer(
            const types::ParticipantId& participant_id);

    /**
     * Remove the IWriter from all the Tracks in the bridge.
     * Remove the IReaders and Tracks that don't have any IWriters.
     *
     * Thread safe
     *
     * @param participant_id: The id of the participant who is removing the writer.
     */
    DDSPIPE_CORE_DllAPI
    void remove_writer(
            const types::ParticipantId& participant_id) noexcept;

protected:

    /**
     * Create the readers, writers, and tracks that are required by the routes.
     *
     * Thread safe
     *
     * @throw InitializationException in case \c IWriters or \c IReaders creation fails.
     */
    DDSPIPE_CORE_DllAPI
    void create_all_tracks_();

    /**
     * Add each Participant's IWriters to its Track.
     * If the Participant's IReader doesn't exist, create it.
     * If the Participant's Track doesn't exist, create it.
     *
     * @param writers: The map of ids to writers that are required for the tracks.
     *
     * @throw InitializationException in case \c IReaders creation fails.
     */
    DDSPIPE_CORE_DllAPI
    void add_writer_to_tracks_nts_(
            const types::ParticipantId& participant_id,
            std::shared_ptr<IWriter>& writer);

    /**
     * Add each Participant's IWriters to its Track.
     * If the Participant's IReader doesn't exist, create it.
     * If the Participant's Track doesn't exist, create it.
     *
     * @param writers: The map of ids to writers that are required for the tracks.
     *
     * @throw InitializationException in case \c IReaders creation fails.
     */
    DDSPIPE_CORE_DllAPI
    void add_writers_to_tracks_nts_(
            std::map<types::ParticipantId, std::shared_ptr<IWriter>>& writers);

    /**
     * @brief Impose the Topic QoS that have been pre-configured for a participant.
     *
     * First, it imposes the Topic QoS configured at \c manual_topics and then the ones configured at \c participants.
     */
    DDSPIPE_CORE_DllAPI
    utils::Heritable<types::DistributedTopic> create_topic_for_participant_nts_(
            const std::shared_ptr<IParticipant>& participant) noexcept;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    //! Topic associated to the DdsBridge.
    utils::Heritable<types::DistributedTopic> topic_;

    //! Routes associated to the Topic.
    RoutesConfiguration::RoutesMap routes_;

    //! Topics that explicitally set a QoS attribute for this participant.
    std::vector<types::ManualTopic> manual_topics_;

    /**
     * Inside \c Tracks
     * They are indexed by the Id of the participant that is source
     */
    std::map<types::ParticipantId, std::unique_ptr<Track>> tracks_;

    //! Mutex to prevent simultaneous calls to enable and/or disable
    std::mutex mutex_;

    // Allow operator << to use private variables
    DDSPIPE_CORE_DllAPI
    friend std::ostream& operator <<(
            std::ostream&,
            const DdsBridge&);
};

/**
 * @brief \c DdsBridge to stream serialization
 *
 * This method is merely a to_string of a Bridge definition.
 * It serialize the topic
 */
DDSPIPE_CORE_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const DdsBridge& bridge);

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
