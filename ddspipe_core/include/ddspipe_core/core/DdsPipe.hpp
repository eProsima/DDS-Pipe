// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
// limitations under the License\.

#pragma once

#include <memory>

#include <cpp_utils/ReturnCode.hpp>
#include <cpp_utils/thread_pool/pool/SlotThreadPool.hpp>

#include <ddspipe_core/communication/dds/DdsBridge.hpp>
#include <ddspipe_core/communication/rpc/RpcBridge.hpp>
#include <ddspipe_core/configuration/DdsPipeConfiguration.hpp>
#include <ddspipe_core/dynamic/AllowedTopicList.hpp>
#include <ddspipe_core/dynamic/DiscoveryDatabase.hpp>
#include <ddspipe_core/dynamic/ParticipantsDatabase.hpp>
#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>

#include <ddspipe_core/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
class DdsPipe
{
public:

    /////////////////////////
    // CONSTRUCTORS
    /////////////////////////

    /**
     * @brief Construct a new DdsPipe object
     *
     * Initialize a whole DdsPipe:
     * - Create its associated AllowedTopicList
     * - Create Participants and add them to \c ParticipantsDatabase
     * - Create the Bridges for (allowed) builtin topics
     *
     * @param [in] configuration : Configuration for the new DDS Router
     *
     * @throw \c ConfigurationException in case the yaml inside allowlist is not well-formed
     * @throw \c InitializationException in case \c IParticipants , \c IWriters or \c IReaders creation fails.
     *
     * @todo change SlotThreadPool for a IThreadPool when exist.
     */
    DDSPIPE_CORE_DllAPI
    DdsPipe(
            const DdsPipeConfiguration& configuration,
            const std::shared_ptr<DiscoveryDatabase>& discovery_database,
            const std::shared_ptr<PayloadPool>& payload_pool,
            const std::shared_ptr<ParticipantsDatabase>& participants_database,
            const std::shared_ptr<utils::SlotThreadPool>& thread_pool);

    /**
     * @brief Destroy the DdsPipe object
     *
     * disable the DdsPipe
     * Destroy all Bridges
     * Destroy all Participants
     */
    DDSPIPE_CORE_DllAPI
    ~DdsPipe();

    /////////////////////////
    // INTERACTION METHODS
    /////////////////////////

    /**
     * @brief Reload the DdsPipe configuration.
     *
     * @param [in] new_configuration : new configuration.
     *
     * @return \c RETCODE_OK if the configuration has been updated correctly.
     * @return \c RETCODE_NO_DATA if the new configuration has not changed.
     * @return \c RETCODE_ERROR if any other error has occurred.
     *
     * @note This method checks that the new configuration file is valid and calls \c reload_allowed_topics_
     *
     * @throw \c ConfigurationException in case the new yaml is not well-formed.
     */
    DDSPIPE_CORE_DllAPI
    utils::ReturnCode reload_configuration(
            const DdsPipeConfiguration& new_configuration);

    /////////////////////////
    // ENABLING METHODS
    /////////////////////////

    /**
     * @brief enable communication in DDS Router
     *
     * Enable every topic Bridge.
     *
     * @note this method returns a ReturnCode for future possible errors
     *
     * @return \c RETCODE_OK always
     */
    DDSPIPE_CORE_DllAPI
    utils::ReturnCode enable() noexcept;

    /**
     * @brief disable communication in DDS Router
     *
     * Disable every topic Bridge.
     *
     * @note this method returns a ReturnCode for future possible errors
     *
     * @return \c RETCODE_OK always
     */
    DDSPIPE_CORE_DllAPI
    utils::ReturnCode disable() noexcept;

protected:

    /////////////////////////
    // INTERACTION METHODS
    /////////////////////////
    /**
     * @brief Load allowed topics from configuration
     *
     * @throw \c ConfigurationException in case the yaml inside allowlist is not well-formed
     */
    void init_allowed_topics_();

    /**
     * @brief Reload the allowed topics configuration.
     *
     * @param [in] allowed_topics : new allowed topics.
     *
     * @return \c RETCODE_OK if the allowed topics have been updated correctly.
     * @return \c RETCODE_NO_DATA if the new allowed topics have not changed.
     * @return \c RETCODE_ERROR if any other error has occurred.
     */
    utils::ReturnCode reload_allowed_topics_(
            const std::shared_ptr<AllowedTopicList>& allowed_topics);

    /////////////////////////
    // CALLBACK METHODS
    /////////////////////////

    /**
     * @brief Method called every time a new endpoint has been discovered
     *
     * This method calls \c discovered_endpoint_nts_ with a lock on the mutex to make it thread safe.
     *
     * @param [in] endpoint : endpoint discovered
     */
    void discovered_endpoint_(
            const types::Endpoint& endpoint) noexcept;

    /**
     * @brief Method called every time a new endpoint has been updated
     *
     * This method calls \c updated_endpoint_nts_ with a lock on the mutex to make it thread safe.
     *
     * @param [in] endpoint : endpoint updated
     */
    void updated_endpoint_(
            const types::Endpoint& endpoint) noexcept;

    /**
     * @brief Method called every time an endpoint has been removed/dropped
     *
     * This method calls \c removed_endpoint_nts_ with a lock on the mutex to make it thread safe.
     *
     * @param [in] endpoint : endpoint removed/dropped
     */
    void removed_endpoint_(
            const types::Endpoint& endpoint) noexcept;

    /**
     * @brief Method called every time a new endpoint has been discovered
     *
     * This method calls \c discovered_topic_ with the topic of \c endpoint as parameter.
     *
     * @param [in] endpoint : endpoint discovered
     */
    void discovered_endpoint_nts_(
            const types::Endpoint& endpoint) noexcept;

    /**
     * @brief Method called every time an endpoint has been removed/dropped
     *
     * @param [in] endpoint : endpoint removed/dropped
     */
    void removed_endpoint_nts_(
            const types::Endpoint& endpoint) noexcept;

    /**
     * @brief Method called every time a new endpoint has been updated
     *
     * This method calls \c discovered_endpoint_nts_ or \c removed_endpoint_nts_.
     *
     * @param [in] endpoint : endpoint updated
     */
    void updated_endpoint_nts_(
            const types::Endpoint& endpoint) noexcept;

    /**
     * @brief Check whether the kind of an endpoint matches the discovery trigger kind.
     *
     * Method called every time a new endpoint has been discovered, removed, or updated.
     *
     * @param [in] endpoint : endpoint discovered, removed, or updated.
     *
     * @return Whether the endpoint's kind matches the discovery trigger.
     */
    bool is_endpoint_kind_relevant_(
            const types::Endpoint& endpoint) noexcept;

    /**
     * @brief Check whether an endpoint is the first endpoint discovered or the last removed.
     *
     * Method called every time a new endpoint has been discovered, removed, or updated.
     * This method calls \c is_endpoint_kind_relevant_
     *
     * @param [in] endpoint : endpoint discovered, removed, or updated.
     *
     * @return Whether the DdsPipe's discovery callbacks need to process the endpoint.
     */
    bool is_endpoint_relevant_(
            const types::Endpoint& endpoint) noexcept;

    /////////////////////////
    // INTERNAL CTOR METHODS
    /////////////////////////

    /**
     * @brief  Create a disabled bridge for every real topic
     */
    void init_bridges_nts_(
            const std::set<utils::Heritable<types::DistributedTopic>>& builtin_topics);

    /////////////////////////
    // INTERNAL AUXILIARY METHODS
    /////////////////////////

    /**
     * @brief Method called every time a new endpoint has been discovered/updated
     *
     * This method is called with the topic of a new/updated \c Endpoint discovered.
     * If the DdsPipe is enabled, the new Bridge is created and enabled.
     *
     * @note This is the only method that adds topics to \c current_topics_
     *
     * @param [in] topic : topic discovered
     */
    void discovered_topic_nts_(
            const utils::Heritable<types::DistributedTopic>& topic) noexcept;

    /**
     * @brief Method called every time a new endpoint (corresponding to a server) has been discovered/updated
     *
     * This method is called with the topic of a new/updated \c Endpoint discovered.
     * If the DdsPipe is enabled and no bridge exists, the new RpcBridge is created (and enabled if allowed).
     *
     * @note This is the only method that adds topics to \c current_services_
     *
     * @param [in] topic : topic discovered
     * @param [in] server_participant_id : id of participant discovering server
     * @param [in] server_guid_prefix : GUID Prefix of discovered server
     */
    void discovered_service_nts_(
            const types::RpcTopic& topic,
            const types::ParticipantId& server_participant_id,
            const types::GuidPrefix& server_guid_prefix) noexcept;

    /**
     * @brief Method called every time a new endpoint (corresponding to a server) has been removed/dropped
     *
     * This method is called with the topic of a removed/dropped \c Endpoint.
     *
     * @param [in] topic : topic discovered
     * @param [in] server_participant_id : id of participant discovering server
     * @param [in] server_guid_prefix : GUID Prefix of discovered server
     */
    void removed_service_nts_(
            const types::RpcTopic& topic,
            const types::ParticipantId& server_participant_id,
            const types::GuidPrefix& server_guid_prefix) noexcept;

    /**
     * @brief Create a new \c DdsBridge object
     *
     * It is created enabled if the DdsPipe is enabled.
     *
     * @param [in] topic : new topic
     */
    void create_new_bridge_nts_(
            const utils::Heritable<types::DistributedTopic>& topic,
            bool enabled = false) noexcept;

    /**
     * @brief Create a new \c RpcBridge object
     *
     * It is always created disabled.
     *
     * @param [in] topic : new topic
     */
    void create_new_service_nts_(
            const types::RpcTopic& topic) noexcept;

    /**
     * @brief Enable a specific topic
     *
     * If the topic did not exist before, the Bridge is created.
     *
     * @param [in] topic : Topic to be enabled
     */
    void activate_topic_nts_(
            const utils::Heritable<types::DistributedTopic>& topic) noexcept;

    /**
     * @brief Disable a specific topic.
     *
     * If the Bridge of the topic does not exist, do nothing.
     *
     * @param [in] topic : Topic to be disabled
     */
    void deactivate_topic_nts_(
            const utils::Heritable<types::DistributedTopic>& topic) noexcept;

    /**
     * @brief Activate all Topics that are allowed by the allowed topics list
     */
    void activate_all_topics_nts_() noexcept;

    /**
     * @brief Disable all Bridges
     */
    void deactivate_all_topics_nts_() noexcept;

    //////////////////////////
    // CONFIGURATION VARIABLES
    //////////////////////////

    //! Configuration of the DDS Pipe
    DdsPipeConfiguration configuration_;

    /////////////////////////
    // SHARED DATA STORAGE
    /////////////////////////

    //! List of allowed and blocked topics
    std::shared_ptr<AllowedTopicList> allowed_topics_;

    /**
     * @brief Common discovery database
     *
     * This object is shared by every Participant.
     * Every time an endpoint is discovered by any Participant, it should be
     * added to the database.
     */
    std::shared_ptr<DiscoveryDatabase> discovery_database_;

    /**
     * @brief  Common payload pool where every payload will be stored
     *
     * This payload will be shared by every endpoint.
     * Every reader will store its data in the pool, the track will pass this
     * data to the writers, that will release it after used.
     */
    std::shared_ptr<PayloadPool> payload_pool_;

    /**
     * @brief Object that stores every Participant running in the DdsPipe
     */
    std::shared_ptr<ParticipantsDatabase> participants_database_;

    //! Thread Pool for tracks
    std::shared_ptr<utils::SlotThreadPool> thread_pool_;

    /////////////////////////
    // INTERNAL DATA STORAGE
    /////////////////////////

    //! Map of bridges indexed by their topic
    std::map<utils::Heritable<types::DistributedTopic>, std::unique_ptr<DdsBridge>> bridges_;

    //! Map of RPC bridges indexed by their topic
    std::map<types::RpcTopic, std::unique_ptr<RpcBridge>> rpc_bridges_;

    /**
     * @brief List of topics discovered
     *
     * Every topic discovered would be added to this map.
     * If the value is true, it means this topic is currently activated.
     */
    std::map<utils::Heritable<types::DistributedTopic>, bool> current_topics_;

    /**
     * @brief List of RPC topics discovered
     *
     * Every RPC topic discovered would is added to this map.
     * If the value is true, it means this service is allowed.
     */
    std::map<types::RpcTopic, bool> current_services_;

    /////////////////////
    // AUXILIAR VARIABLES
    /////////////////////

    //! Whether the DdsPipe is currently communicating data or not
    bool enabled_;

    /**
     * @brief Internal mutex for concurrent calls
     */
    mutable std::mutex mutex_;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
