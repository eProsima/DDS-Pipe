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
// limitations under the License.

#include <set>

#include <cpp_utils/exception/ConfigurationException.hpp>
#include <cpp_utils/exception/InconsistencyException.hpp>
#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/exception/UnsupportedException.hpp>
#include <cpp_utils/Log.hpp>
#include <cpp_utils/ReturnCode.hpp>
#include <cpp_utils/utils.hpp>

#include <ddspipe_core/core/DdsPipe.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

using namespace eprosima::ddspipe::core::types;

DdsPipe::DdsPipe(
        const DdsPipeConfiguration& configuration,
        const std::shared_ptr<DiscoveryDatabase>& discovery_database,
        const std::shared_ptr<PayloadPool>& payload_pool,
        const std::shared_ptr<ParticipantsDatabase>& participants_database,
        const std::shared_ptr<utils::SlotThreadPool>& thread_pool)
    : configuration_(configuration)
    , discovery_database_(discovery_database)
    , payload_pool_(payload_pool)
    , participants_database_(participants_database)
    , thread_pool_(thread_pool)
    , enabled_(false)
{
    logDebug(DDSPIPE, "Creating DDS Pipe.");

    // Check that the configuration is correct
    utils::Formatter error_msg;
    if (!configuration_.is_valid(error_msg, participants_database_->get_participants_repeater_map()))
    {
        throw utils::ConfigurationException(
                  utils::Formatter() <<
                      "Configuration for DDS Pipe is invalid: " << error_msg);
    }

    // Initialize the allowed topics
    init_allowed_topics_();

    // Add callback to be called by the discovery database when an Endpoint is discovered
    discovery_database_->add_endpoint_discovered_callback(std::bind(&DdsPipe::discovered_endpoint_, this,
            std::placeholders::_1));

    // Add callback to be called by the discovery database when an Endpoint is updated
    discovery_database_->add_endpoint_updated_callback(std::bind(&DdsPipe::updated_endpoint_, this,
            std::placeholders::_1));

    // Add callback to be called by the discovery database when an Endpoint is removed/dropped
    discovery_database_->add_endpoint_erased_callback(std::bind(&DdsPipe::removed_endpoint_, this,
            std::placeholders::_1));

    // Create Bridges for builtin topics
    init_bridges_nts_(configuration_.builtin_topics);

    // Enable thread pool
    thread_pool_->enable();

    // Enable if set
    if (configuration_.init_enabled)
    {
        enable();
    }

    // Init discovery database
    discovery_database_->start();

    logDebug(DDSPIPE, "DDS Pipe created.");
}

DdsPipe::~DdsPipe()
{
    logDebug(DDSPIPE, "Destroying DDS Pipe.");

    // Stop Discovery Database
    discovery_database_->stop();

    // Stop all communications
    disable();

    // Disable thread pool
    thread_pool_->disable();

    // Destroy Bridges, so Writers and Readers are destroyed before the Databases
    bridges_.clear();

    // Destroy RpcBridges, so Writers and Readers are destroyed before the Databases
    rpc_bridges_.clear();

    // There is no need to destroy shared pointers.
    // They self-destruct when they have 0 references.

    logDebug(DDSPIPE, "DDS Pipe destroyed.");
}

utils::ReturnCode DdsPipe::reload_configuration(
        const DdsPipeConfiguration& new_configuration)
{
    // Check that the configuration is correct
    utils::Formatter error_msg;
    if (!new_configuration.is_valid(error_msg, participants_database_->get_participants_repeater_map()))
    {
        throw utils::ConfigurationException(
                  utils::Formatter() <<
                      "Configuration for Reload DDS Pipe is invalid: " << error_msg);
    }

    auto allowed_topics = std::make_shared<ddspipe::core::AllowedTopicList>(
        new_configuration.allowlist,
        new_configuration.blocklist);

    return reload_allowed_topics_(allowed_topics);
}

void DdsPipe::reload_filter_partition(
        const std::set<std::string> filter_partition_set)
{
    update_filter(filter_partition_set);

    std::vector<DdsBridge*> targets;
    {
        std::lock_guard<std::mutex> lock(bridges_mutex_);

        for (const auto& bridge : bridges_)
        {
            if (bridge.second)
            {
                targets.push_back(bridge.second.get());
            }
        }
    }

    for (auto* target : targets)
    {
        target->update_readers_track(filter_partition_set);
    }
}

utils::ReturnCode DdsPipe::enable() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!enabled_)
    {
        enabled_ = true;

        EPROSIMA_LOG_INFO(DDSPIPE, "Enabling DDS Pipe.");

        activate_all_topics_nts_();

        // Enable services discovered while pipe disabled
        for (auto it : current_services_)
        {
            // Enable only allowed services
            if (it.second)
            {
                rpc_bridges_[it.first]->enable();
            }
        }

        return utils::ReturnCode::RETCODE_OK;
    }
    else
    {
        EPROSIMA_LOG_INFO(DDSPIPE, "Trying to enable an already enabled DDS Pipe.");
        return utils::ReturnCode::RETCODE_PRECONDITION_NOT_MET;
    }
}

utils::ReturnCode DdsPipe::disable() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (enabled_)
    {
        enabled_ = false;

        EPROSIMA_LOG_INFO(DDSPIPE, "Disabling DDS Pipe.");

        deactivate_all_topics_nts_();

        return utils::ReturnCode::RETCODE_OK;
    }
    else
    {
        EPROSIMA_LOG_INFO(DDSPIPE, "Trying to disable a disabled DDS Pipe.");
        return utils::ReturnCode::RETCODE_PRECONDITION_NOT_MET;
    }
}

void DdsPipe::init_allowed_topics_()
{
    allowed_topics_ = std::make_shared<ddspipe::core::AllowedTopicList>(
        configuration_.allowlist,
        configuration_.blocklist);

    EPROSIMA_LOG_INFO(DDSROUTER, "DDS Router configured with allowed topics: " << *allowed_topics_);
}

utils::ReturnCode DdsPipe::reload_allowed_topics_(
        const std::shared_ptr<AllowedTopicList>& allowed_topics)
{
    std::lock_guard<std::mutex> lock(mutex_);

    logDebug(DDSPIPE, "Reloading DDS Pipe configuration...");

    // Check if it should change or is the same configuration
    if (*allowed_topics == *allowed_topics_)
    {
        logDebug(DDSPIPE, "Same configuration, do nothing in reload.");
        return utils::ReturnCode::RETCODE_NO_DATA;
    }

    // Set new Allowed list
    allowed_topics_ = allowed_topics;

    logDebug(DDSPIPE, "New DDS Pipe allowed topics configuration: " << allowed_topics_);

    if (!enabled_)
    {
        return utils::ReturnCode::RETCODE_OK;
    }

    // It must change the configuration. Check every topic discovered and activate/deactivate it if needed.
    for (auto& topic_it : current_topics_)
    {
        // If topic is active and it is blocked, deactivate it
        if (topic_it.second)
        {
            if (!allowed_topics_->is_topic_allowed(*topic_it.first))
            {
                deactivate_topic_nts_(topic_it.first);
            }
        }
        else
        {
            // If topic is not active and it is allowed, activate it
            if (allowed_topics_->is_topic_allowed(*topic_it.first))
            {
                activate_topic_nts_(topic_it.first);
            }
        }
    }

    // Check every service discovered and activate/deactivate it if needed.
    for (auto& service_it : current_services_)
    {
        if (allowed_topics_->is_service_allowed(service_it.first))
        {
            service_it.second = true;
            rpc_bridges_[service_it.first]->enable();
        }
        else
        {
            service_it.second = false;
            rpc_bridges_[service_it.first]->disable();
        }
    }

    return utils::ReturnCode::RETCODE_OK;
}

void DdsPipe::discovered_endpoint_(
        const Endpoint& endpoint) noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);
    discovered_endpoint_nts_(endpoint);
}

void DdsPipe::updated_endpoint_(
        const Endpoint& endpoint) noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);
    updated_endpoint_nts_(endpoint);
}

void DdsPipe::removed_endpoint_(
        const Endpoint& endpoint) noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);
    removed_endpoint_nts_(endpoint);
}

void DdsPipe::discovered_endpoint_nts_(
        const Endpoint& endpoint) noexcept
{
    logDebug(DDSPIPE, "Endpoint discovered in DDS Pipe core: " << endpoint << ".");

    if (RpcTopic::is_service_topic(endpoint.topic))
    {
        if (is_endpoint_kind_relevant_(endpoint) && endpoint.is_server_endpoint())
        {
            // Service server discovered
            discovered_service_nts_(RpcTopic(
                        endpoint.topic), endpoint.discoverer_participant_id, endpoint.guid.guid_prefix());
        }
    }
    else if (is_endpoint_relevant_(endpoint))
    {
        DdsTopic topic_with_partitions = endpoint.topic;
        topic_with_partitions.partition_name = endpoint.specific_partitions;

        discovered_topic_nts_(utils::Heritable<DdsTopic>::make_heritable(topic_with_partitions));
    }
    else
    {
        // there is a filter.
        // update the track of the topic to
        // add the partition in the reader if it is in the filter

        // update partitions under bridges_mutex_
        {
            std::lock_guard<std::mutex> lock(bridges_mutex_);

            const auto bridge_it = bridges_.find(utils::Heritable<DdsTopic>::make_heritable(endpoint.topic));
            // add the specific partition of the endpoint in the bridges topic.
            if (bridge_it != bridges_.end())
            {
                std::ostringstream guid_ss;
                guid_ss << endpoint.guid;

                const auto part_it = endpoint.specific_partitions.find(guid_ss.str());
                if (part_it != endpoint.specific_partitions.end())
                {
                    bridge_it->second->add_partition_to_topic(guid_ss.str(), part_it->second);
                }
            }
        }
        // update readers outside the lock
        if (!filter_partition_.empty())
        {
            update_readers_track(endpoint.topic.m_topic_name, filter_partition_);
        }
    }
}

void DdsPipe::removed_endpoint_nts_(
        const Endpoint& endpoint) noexcept
{
    logDebug(DDSPIPE, "Endpoint removed/dropped: " << endpoint << ".");

    if (RpcTopic::is_service_topic(endpoint.topic))
    {
        if (endpoint.is_server_endpoint())
        {
            // Service server removed/dropped
            removed_service_nts_(RpcTopic(endpoint.topic), endpoint.discoverer_participant_id,
                    endpoint.guid.guid_prefix());
        }

    }
    else if (configuration_.remove_unused_entities && is_endpoint_relevant_(endpoint))
    {
        std::lock_guard<std::mutex> lock(bridges_mutex_);

        const auto& topic = utils::Heritable<DdsTopic>::make_heritable(endpoint.topic);

        // Remove the subscriber from the topic.
        auto it_bridge = bridges_.find(topic);

        if (it_bridge != bridges_.end() && endpoint.discoverer_participant_id != DEFAULT_PARTICIPANT_ID)
        {
            it_bridge->second->remove_writer(endpoint.discoverer_participant_id);
        }
    }
}

void DdsPipe::updated_endpoint_nts_(
        const Endpoint& endpoint) noexcept
{
    logDebug(DDSPIPE, "Endpoint updated in DDS Pipe core: " << endpoint << ".");

    // Don't send updated information to service topics.
    if (RpcTopic::is_service_topic(endpoint.topic))
    {
        return;
    }

    if (endpoint.active)
    {
        discovered_endpoint_nts_(endpoint);
    }
    else
    {
        removed_endpoint_nts_(endpoint);
    }
}

bool DdsPipe::is_endpoint_kind_relevant_(
        const Endpoint& endpoint) noexcept
{
    switch (configuration_.discovery_trigger)
    {
        case DiscoveryTrigger::READER:
            return endpoint.is_reader();

        case DiscoveryTrigger::WRITER:
            return endpoint.is_writer();

        case DiscoveryTrigger::ANY:
            return true;

        case DiscoveryTrigger::NONE:
            return false;

        default:
            utils::tsnh(utils::Formatter() << "Invalid Discovery Trigger.");
            return false;
    }
}

bool DdsPipe::is_endpoint_relevant_(
        const Endpoint& endpoint) noexcept
{
    if (!is_endpoint_kind_relevant_(endpoint))
    {
        return false;
    }

    auto is_endpoint_relevant = [&](const Endpoint& entity)
            {
                return entity.active &&
                       is_endpoint_kind_relevant_(entity) &&
                       entity.topic == endpoint.topic &&
                       entity.discoverer_participant_id == endpoint.discoverer_participant_id;
            };

    const auto& relevant_endpoints = discovery_database_->get_endpoints(is_endpoint_relevant);

    if (endpoint.active)
    {
        // An active reader is relevant when it is the only active reader in a topic
        // with a discoverer participant id.
        return relevant_endpoints.size() == 1 && relevant_endpoints.count(endpoint.guid);
    }
    else
    {
        // An inactive reader is relevant when there aren't any active readers in a topic
        // with a discoverer participant id.
        return relevant_endpoints.size() == 0;
    }
}

void DdsPipe::init_bridges_nts_(
        const std::set<utils::Heritable<DistributedTopic>>& builtin_topics)
{
    for (const auto& topic : builtin_topics)
    {
        discovered_topic_nts_(topic);
        create_new_bridge_nts_(topic, false);
    }
}

void DdsPipe::discovered_topic_nts_(
        const utils::Heritable<DistributedTopic>& topic) noexcept
{
    EPROSIMA_LOG_INFO(DDSPIPE, "Discovered topic: " << topic << " by: " << topic->topic_discoverer() << ".");

    bool need_activate = false;
    DdsBridge* to_create_writer = nullptr;
    ParticipantId writer_pid{};

    {
        std::lock_guard<std::mutex> lock(bridges_mutex_);

        // Check if the bridge (and the topic) already exist.
        auto it_bridge = bridges_.find(topic);
        if (it_bridge == bridges_.end())
        {
            // Add topic to current_topics as not activated
            current_topics_.emplace(topic, false);

            // decide activation under lock, perform it after unlocking
            need_activate = enabled_ && allowed_topics_->is_topic_allowed(*topic);
        }
        else if (configuration_.remove_unused_entities && topic->topic_discoverer() != DEFAULT_PARTICIPANT_ID)
        {
            // The bridge already exists.
            // Create a writer in the participant who discovered it.
            // (out of the lock)
            to_create_writer = it_bridge->second.get();
            writer_pid = topic->topic_discoverer();
        }
    }

    // call out of lock
    if (to_create_writer)
    {
        to_create_writer->create_writer(writer_pid);
    }
    // if Pipe is enabled and topic allowed, activate it
    if (need_activate)
    {
        activate_topic_nts_(topic);
    }
}

void DdsPipe::discovered_service_nts_(
        const RpcTopic& topic,
        const ParticipantId& server_participant_id,
        const GuidPrefix& server_guid_prefix) noexcept
{
    EPROSIMA_LOG_INFO(DDSPIPE, "Discovered service: " << topic << ".");

    RpcBridge* bridge = nullptr;
    bool should_enable = false;
    {
        std::lock_guard<std::mutex> lock(bridges_mutex_);

        auto it_bridge = rpc_bridges_.find(topic);

        if (it_bridge == rpc_bridges_.end())
        {
            // Create RpcBridge even if topic not allowed, as we need to store server in database
            create_new_service_nts_(topic);
            current_services_[topic] = allowed_topics_->is_service_allowed(topic);

            it_bridge = rpc_bridges_.find(topic);
        }
        bridge = it_bridge->second.get();
        should_enable = enabled_ && current_services_[topic];
    }

    // Calls outside the lock
    bridge->discovered_service(server_participant_id, server_guid_prefix);
    if (should_enable)
    {
        bridge->enable();
    }
}

void DdsPipe::removed_service_nts_(
        const RpcTopic& topic,
        const ParticipantId& server_participant_id,
        const GuidPrefix& server_guid_prefix) noexcept
{
    EPROSIMA_LOG_INFO(DDSPIPE, "Removed service: " << topic << ".");

    RpcBridge* bridge = nullptr;
    {
        std::lock_guard<std::mutex> lock(bridges_mutex_);

        auto it_bridge = rpc_bridges_.find(topic);

        if (it_bridge != rpc_bridges_.end())
        {
            bridge = it_bridge->second.get();
        }
    }

    if (bridge)
    {
        bridge->removed_service(server_participant_id, server_guid_prefix);
    }
}

void DdsPipe::create_new_bridge_nts_(
        const utils::Heritable<DistributedTopic>& topic,
        bool enabled /*= false*/) noexcept
{
    EPROSIMA_LOG_INFO(DDSPIPE, "Creating Bridge for topic: " << topic << ".");

    try
    {
        auto routes_config = configuration_.get_routes_config(topic);
        auto manual_topics = configuration_.get_manual_topics(dynamic_cast<const core::ITopic&>(*topic));

        std::unique_ptr<DdsBridge> new_bridge;

        bool filter_partition_empty;
        {
            // Avoid possible datarace with partitions filter
            std::lock_guard<std::mutex> lock(bridges_mutex_);
            filter_partition_empty = filter_partition_.empty();
        }

        // check if there is a filter partition list
        if (filter_partition_empty)
        {
            // Create bridge instance
            new_bridge = std::make_unique<DdsBridge>(topic,
                            participants_database_,
                            payload_pool_,
                            thread_pool_,
                            routes_config,
                            configuration_.remove_unused_entities,
                            manual_topics);
        }
        else
        {
            // enters if there is a partitions filter
            // and the topic was not in the track of bridges

            // Create bridge instance
            new_bridge = std::make_unique<DdsBridge>(topic,
                            participants_database_,
                            payload_pool_,
                            thread_pool_,
                            routes_config,
                            configuration_.remove_unused_entities,
                            manual_topics,
                            filter_partition_);
        }

        DdsBridge* to_enable = nullptr;
        {
            std::lock_guard<std::mutex> lock(bridges_mutex_);
            auto& slot = bridges_[topic];
            slot = std::move(new_bridge);
            if (enabled)
            {
                to_enable = slot.get();
            }
        }

        // enable without the lock
        if (to_enable)
        {
            to_enable->enable();
        }
    }
    catch (const utils::InitializationException& e)
    {
        EPROSIMA_LOG_ERROR(DDSPIPE,
                "Error creating Bridge for topic " << topic <<
                ". Error code:" << e.what() << ".");
    }
}

void DdsPipe::create_new_service_nts_(
        const RpcTopic& topic) noexcept
{
    EPROSIMA_LOG_INFO(DDSPIPE, "Creating Service: " << topic << ".");

    // Endpoints not created until enabled for the first time, so no exception can be thrown
    rpc_bridges_[topic] = std::make_unique<RpcBridge>(topic, participants_database_, payload_pool_, thread_pool_);
}

void DdsPipe::activate_topic_nts_(
        const utils::Heritable<DistributedTopic>& topic) noexcept
{
    EPROSIMA_LOG_INFO(DDSPIPE, "Activating topic: " << topic << ".");

    // Modify current_topics_ and set this topic as active
    {
        std::lock_guard<std::mutex> lock(bridges_mutex_);
        current_topics_[topic] = true;
    }

    // Enable bridge. In case it is already enabled nothing should happen
    DdsBridge* to_enable = nullptr;
    {
        std::lock_guard<std::mutex> lock(bridges_mutex_);
        auto it = bridges_.find(topic);
        if (it != bridges_.end())
        {
            to_enable = it->second.get();
        }
    }
    if (!to_enable)
    {
        // Will enable after inserting (outside the lock) inside create_new_bridge_nts_
        create_new_bridge_nts_(topic, true);
    }
    else
    {
        to_enable->enable();
    }
}

void DdsPipe::deactivate_topic_nts_(
        const utils::Heritable<DistributedTopic>& topic) noexcept
{
    EPROSIMA_LOG_INFO(DDSPIPE, "Deactivating topic: " << topic << ".");

    DdsBridge* to_disable = nullptr;
    {
        std::lock_guard<std::mutex> lock(bridges_mutex_);

        // Modify current_topics_ and set this topic as not active
        current_topics_[topic] = false;

        // Disable bridge. In case it is already disabled nothing should happen
        auto it_bridge = bridges_.find(topic);
        if (it_bridge != bridges_.end())
        {
            to_disable = it_bridge->second.get();
        }
    }

    if (to_disable)
    {
        to_disable->disable();
    }
    // If the Bridge does not exist, there is no need to create it
}

void DdsPipe::activate_all_topics_nts_() noexcept
{
    for (auto it : current_topics_)
    {
        // Activate all topics allowed
        if (allowed_topics_->is_topic_allowed(*it.first))
        {
            activate_topic_nts_(it.first);
        }
    }
}

void DdsPipe::deactivate_all_topics_nts_() noexcept
{
    for (auto it : current_topics_)
    {
        // Deactivate all topics
        deactivate_topic_nts_(it.first);
    }
}

void DdsPipe::update_readers_track(
        const std::string topic_name,
        const std::set<std::string> filter_partition_set)
{
    // get the targets (with lock)
    std::vector<DdsBridge*> targets;
    {
        std::lock_guard<std::mutex> lock(bridges_mutex_);

        for (const auto& pair : bridges_)
        {
            if (pair.first->m_topic_name == topic_name && pair.second)
            {
                targets.push_back(pair.second.get());
            }
        }
    }

    // update readers outside the lock
    for (auto* target : targets)
    {
        target->update_readers_track(filter_partition_set);
    }
}

void DdsPipe::update_filter(
        const std::set<std::string> filter_partition_set)
{
    // Avoid possible datarace with partitions filter
    std::lock_guard<std::mutex> lock(bridges_mutex_);

    filter_partition_ = filter_partition_set;
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
