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
#include <ddspipe_core/types/dds/Endpoint.hpp>
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
     * @param routes_config: Configuration of the routes of the Bridge
     * @param remove_unused_entities: Whether the Bridge should remove unused entities
     * @param manual_topics: Topics that explicitally set a QoS attribute for this participant
     * @param endpoint_kind: Kind of the endpoint that discovered the topic
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
            const std::vector<core::types::ManualTopic>& manual_topics,
            const types::EndpointKind& endpoint_kind);

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
     * Create a new endpoint in the bridge.
     *
     * It will call either \c create_writer_and_its_tracks_nts_ or \c create_reader_and_its_track_nts_ depending on the
     * \c discovered_endpoint_kind.
     *
     * @param participant_id: The id of the participant who is creating the endpoint.
     * @param discovered_endpoint_kind: The kind of endpoint that has been discovered.
     *
     * Thread safe.
     */
    DDSPIPE_CORE_DllAPI
    void create_endpoint(
            const types::ParticipantId& participant_id,
            const types::EndpointKind& discovered_endpoint_kind);

    /**
     * Remove an endpoint from the bridge.
     *
     * It will call either \c remove_writer_and_its_tracks_nts_ or \c remove_reader_and_its_track_nts_ depending on the
     * \c removed_endpoint_kind.
     *
     * @param participant_id: The id of the participant who is removing the endpoint.
     * @param removed_endpoint_kind: The kind of endpoint that has been removed.
     *
     * Thread safe.
     */
    DDSPIPE_CORE_DllAPI
    void remove_endpoint(
            const types::ParticipantId& participant_id,
            const types::EndpointKind& removed_endpoint_kind);

protected:

    /**
     * Create an IWriter for the new participant.
     * Create the IReaders in the IWriter's route.
     * Create the Tracks of the IReaders with the IWriter.
     *
     * @param participant_id: The id of the participant who is creating the writer.
     *
     * @throw InitializationException in case \c IWriters or \c IReaders creation fails.
     */
    void create_writer_and_its_tracks_nts_(
            const types::ParticipantId& participant_id);

    /**
     * Create an IReader for the new participant.
     * Create the IWriters in the IReader's route.
     * Create the Track with the IReader and IWriters.
     *
     * @param participant_id: The id of the participant who is creating the reader.
     *
     * @throw InitializationException in case \c IWriters or \c IReaders creation fails.
     */
    void create_reader_and_its_track_nts_(
            const types::ParticipantId& participant_id);

    /**
     * Remove the IWriter from all the Tracks.
     * Remove the IReaders and Tracks without IWriters.
     *
     * @param participant_id: The id of the participant who is removing the writer.
     */
    void remove_writer_and_its_tracks_nts_(
            const types::ParticipantId& participant_id) noexcept;

    /**
     * Remove the IReader and its Track.
     * Remove the IWriters that no longer belong to a Track.
     *
     * @param participant_id: The id of the participant who is removing the reader.
     */
    void remove_reader_and_its_track_nts_(
            const types::ParticipantId& participant_id) noexcept;

    /**
     * @brief Create a Track for an IReader and its IWriters.
     *
     * @param id: The id of the participant who is creating the track.
     * @param reader: The IReader of the track.
     * @param writers: The IWriters of the track.
     */
    void create_track_nts_(
            const types::ParticipantId& id,
            const std::shared_ptr<IReader>& reader,
            std::map<types::ParticipantId, std::shared_ptr<IWriter>>& writers);

    /**
     * @brief Create an IWriter for a participant in the topic.
     *
     * A tailored Topic is created for the participant, depending on the QoS configured for it.
     *
     * @param participant_id: The id of the participant who is creating the writer.
     */
    std::shared_ptr<core::IWriter> create_writer_nts_(
            const types::ParticipantId& participant_id);

    /**
     * @brief Create an IReader for a participant in the topic.
     *
     * A tailored Topic is created for the participant, depending on the QoS configured for it.
     *
     * @param participant_id: The id of the participant who is creating the reader.
     */
    std::shared_ptr<core::IReader> create_reader_nts_(
            const types::ParticipantId& participant_id);

    /**
     * @brief Impose the Topic QoS that have been pre-configured for a participant.
     *
     * First, it imposes the Topic QoS configured at \c manual_topics and then the ones configured at \c participants.
     *
     * @param participant: The participant to impose the QoS on.
     */
    utils::Heritable<types::DistributedTopic> create_topic_for_participant_nts_(
            const std::shared_ptr<IParticipant>& participant) noexcept;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    //! Topic associated to the DdsBridge.
    utils::Heritable<types::DistributedTopic> topic_;

    //! Routes associated to the Topic.
    RoutesConfiguration::RoutesMap writers_in_route_;
    RoutesConfiguration::RoutesMap readers_in_route_;

    //! Topics that explicitally set a QoS attribute for this participant.
    std::vector<types::ManualTopic> manual_topics_;

    //! The tracks of the bridge indexed by the participant_id of their reader.
    std::map<types::ParticipantId, std::unique_ptr<Track>> tracks_;

    //! The writers of the bridge index by their participant_id.
    std::map<types::ParticipantId, std::shared_ptr<IWriter>> writers_;

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
