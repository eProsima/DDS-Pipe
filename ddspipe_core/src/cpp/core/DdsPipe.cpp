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

#include <cpp_utils/exception/UnsupportedException.hpp>
#include <cpp_utils/exception/ConfigurationException.hpp>
#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/exception/InconsistencyException.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/core/DdsPipe.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

using namespace eprosima::ddspipe::core::types;

DdsPipe::DdsPipe(
        const std::shared_ptr<AllowedTopicList>& allowed_topics,
        const std::shared_ptr<DiscoveryDatabase>& discovery_database,
        const std::shared_ptr<PayloadPool>& payload_pool,
        const std::shared_ptr<ParticipantsDatabase>& participants_database,
        const std::shared_ptr<utils::SlotThreadPool>& thread_pool,
        const std::set<utils::Heritable<DistributedTopic>>& builtin_topics, /* = {} */
        bool start_enable, /* = false */
        const RoutesConfiguration& routes_config, /* = {} */
        const TopicRoutesConfiguration& topic_routes_config /* = {} */)
    : allowed_topics_(allowed_topics)
    , discovery_database_(discovery_database)
    , payload_pool_(payload_pool)
    , participants_database_(participants_database)
    , thread_pool_(thread_pool)
    , enabled_(false)
    , routes_config_(routes_config)
    , topic_routes_config_(topic_routes_config)
{
    logDebug(DDSPIPE, "Creating DDS Pipe.");

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
    init_bridges_nts_(builtin_topics);

    // Enable thread pool
    thread_pool_->enable();

    // Enable if set
    if (start_enable)
    {
        enable();
    }

    // Init discovery database
    // The entities should not be added to the Discovery Database until the builtin topics have been created.
    // This is due to the fact that the Participants endpoints start discovering topics with different configuration
    // than the one specified in the yaml configuration file.
    discovery_database_->start();

    logDebug(DDSPIPE, "DDS Pipe created.");
}

DdsPipe::~DdsPipe()
{
    logDebug(DDSPIPE, "Destroying DDS Pipe.");

    // Stop Discovery Database
    discovery_database_->stop();

    // Disable thread pool
    thread_pool_->disable();

    // Stop all communications
    disable();

    // Destroy Bridges, so Writers and Readers are destroyed before the Databases
    bridges_.clear();

    // Destroy RpcBridges, so Writers and Readers are destroyed before the Databases
    rpc_bridges_.clear();

    // There is no need to destroy shared ptrs as they will delete itslefs with 0 references

    logDebug(DDSPIPE, "DDS Pipe destroyed.");
}

utils::ReturnCode DdsPipe::reload_allowed_topics(
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

utils::ReturnCode DdsPipe::enable() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!enabled_)
    {
        enabled_ = true;

        logInfo(DDSPIPE, "Enabling DDS Pipe.");

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
        logInfo(DDSPIPE, "Trying to enable an already enabled DDS Pipe.");
        return utils::ReturnCode::RETCODE_PRECONDITION_NOT_MET;
    }
}

utils::ReturnCode DdsPipe::disable() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (enabled_)
    {
        enabled_ = false;

        logInfo(DDSPIPE, "Disabling DDS Pipe.");

        deactivate_all_topics_nts_();

        return utils::ReturnCode::RETCODE_OK;
    }
    else
    {
        logInfo(DDSPIPE, "Trying to disable a disabled DDS Pipe.");
        return utils::ReturnCode::RETCODE_PRECONDITION_NOT_MET;
    }
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

    logDebug(DDSPIPE, "Endpoint updated in DDS Pipe core: " << endpoint << ".");

    // Set as discovered only if the endpoint is a Reader
    // If non Readers in topics, it is considered as non discovered
    if (endpoint.is_reader() && !RpcTopic::is_service_topic(endpoint.topic))
    {
        for (const auto& guid_to_entity : discovery_database_->get_endpoints())
        {
            const auto& guid = guid_to_entity.first;
            const auto& entity = guid_to_entity.second;

            if (guid != endpoint.guid &&
                    entity.active &&
                    entity.is_reader() &&
                    entity.topic == endpoint.topic &&
                    entity.discoverer_participant_id == endpoint.discoverer_participant_id)
            {
                // There is an active reader other than us.
                // If we have been reactivated, there is nothing to do.
                // If we have been disactivated, there is nothing to do.
                return;
            }
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

    // Set as discovered only if the endpoint is a Reader
    // If non Readers in topics, it is considered as non discovered
    if (endpoint.is_reader())
    {
        if (!RpcTopic::is_service_topic(endpoint.topic))
        {
            discovered_topic_nts_(utils::Heritable<DdsTopic>::make_heritable(
                        endpoint.topic), endpoint.discoverer_participant_id);
        }
        else if (endpoint.is_server_endpoint())
        {
            // Service server discovered
            discovered_service_nts_(RpcTopic(
                        endpoint.topic), endpoint.discoverer_participant_id, endpoint.guid.guid_prefix());
        }
    }
}

void DdsPipe::removed_endpoint_nts_(
        const Endpoint& endpoint) noexcept
{
    logDebug(DDSPIPE, "Endpoint removed/dropped: " << endpoint << ".");

    const auto& topic = utils::Heritable<DdsTopic>::make_heritable(endpoint.topic);

    if (!RpcTopic::is_service_topic(endpoint.topic))
    {
        if (endpoint.is_writer())
        {
            // An external writer has been removed.
            // The bridge and the tracks remain intact.
            return;
        }

        // Remove the subscriber from the topic.
        auto it_bridge = bridges_.find(topic);

        if (it_bridge == bridges_.end())
        {
            // The bridge does not exist. Error.
            logError(DDSPIPE,
                    "Error finding Bridge for topic " << topic <<
                    ". The Bridge does not exist.");
        }
        else
        {
            it_bridge->second->remove_from_tracks(endpoint.discoverer_participant_id);
        }
    }
    else if (endpoint.is_server_endpoint())
    {
        // Service server removed/dropped
        removed_service_nts_(RpcTopic(endpoint.topic), endpoint.discoverer_participant_id, endpoint.guid.guid_prefix());
    }
}

void DdsPipe::init_bridges_nts_(
        const std::set<utils::Heritable<DistributedTopic>>& builtin_topics)
{
    for (const auto& topic : builtin_topics)
    {
        discovered_topic_nts_(topic, "builtin-participant");
        create_new_bridge_nts_(topic, false);
    }
}

void DdsPipe::discovered_topic_nts_(
        const utils::Heritable<DistributedTopic>& topic,
        const ParticipantId& discoverer_participant_id) noexcept
{
    logInfo(DDSPIPE, "Discovered topic: " << topic << ", by: " << discoverer_participant_id << ".");

    // Check if the bridge (and the topic) already exist.
    auto it_bridge = bridges_.find(topic);

    if (it_bridge != bridges_.end())
    {
        // The bridge already exists. Add the discoverer to the tracks.
        it_bridge->second->add_to_tracks(discoverer_participant_id);
        return;
    }

    // Add topic to current_topics as non activated
    current_topics_.emplace(topic, false);

    // Save the id of the participant who discovered the topic
    current_topics_discoverers_.emplace(topic, discoverer_participant_id);

    // If Pipe is enabled and topic allowed, activate it
    if (enabled_ && allowed_topics_->is_topic_allowed(*topic))
    {
        activate_topic_nts_(topic);
    }
}

void DdsPipe::discovered_service_nts_(
        const RpcTopic& topic,
        const ParticipantId& server_participant_id,
        const GuidPrefix& server_guid_prefix) noexcept
{
    logInfo(DDSPIPE, "Discovered service: " << topic << ".");

    auto it_bridge = rpc_bridges_.find(topic);

    if (it_bridge == rpc_bridges_.end())
    {
        // Create RpcBridge even if topic not allowed, as we need to store server in database
        create_new_service_nts_(topic);

        if (allowed_topics_->is_service_allowed(topic))
        {
            current_services_[topic] = true;
        }
        else
        {
            current_services_[topic] = false;
        }
    }

    rpc_bridges_[topic]->discovered_service(server_participant_id, server_guid_prefix);
    if (enabled_ && current_services_[topic])
    {
        rpc_bridges_[topic]->enable();
    }
}

void DdsPipe::removed_service_nts_(
        const RpcTopic& topic,
        const ParticipantId& server_participant_id,
        const GuidPrefix& server_guid_prefix) noexcept
{
    logInfo(DDSPIPE, "Removed service: " << topic << ".");

    auto it_bridge = rpc_bridges_.find(topic);

    if (it_bridge != rpc_bridges_.end())
    {
        rpc_bridges_[topic]->removed_service(server_participant_id, server_guid_prefix);
    }
}

void DdsPipe::create_new_bridge_nts_(
        const utils::Heritable<DistributedTopic>& topic,
        bool enabled /*= false*/) noexcept
{
    logInfo(DDSPIPE, "Creating Bridge for topic: " << topic << ".");

    try
    {
        auto routes_config__ = topic_routes_config_().count(topic) !=
                0 ? topic_routes_config_()[topic] : routes_config_;
                
        auto discoverer_participant_id = current_topics_discoverers_[topic];

        // Create bridge instance
        auto new_bridge =  std::make_unique<DdsBridge>(topic,
                        participants_database_,
                        payload_pool_,
                        thread_pool_,
                        routes_config,
                        discoverer_participant_id);

        if (enabled)
        {
            new_bridge->enable();
        }

        bridges_[topic] = std::move(new_bridge);
    }
    catch (const utils::InitializationException& e)
    {
        logError(DDSPIPE,
                "Error creating Bridge for topic " << topic <<
                ". Error code:" << e.what() << ".");
    }
}

void DdsPipe::create_new_service_nts_(
        const RpcTopic& topic) noexcept
{
    logInfo(DDSPIPE, "Creating Service: " << topic << ".");

    // Endpoints not created until enabled for the first time, so no exception can be thrown
    rpc_bridges_[topic] = std::make_unique<RpcBridge>(topic, participants_database_, payload_pool_, thread_pool_);
}

void DdsPipe::activate_topic_nts_(
        const utils::Heritable<DistributedTopic>& topic) noexcept
{
    logInfo(DDSPIPE, "Activating topic: " << topic << ".");

    // Modify current_topics_ and set this topic as active
    current_topics_[topic] = true;

    // Enable bridge. In case it is already enabled nothing should happen
    auto it_bridge = bridges_.find(topic);

    if (it_bridge == bridges_.end())
    {
        // The Bridge did not exist
        create_new_bridge_nts_(topic, true);
    }
    else
    {
        // The Bridge already exists
        it_bridge->second->enable();
    }
}

void DdsPipe::deactivate_topic_nts_(
        const utils::Heritable<DistributedTopic>& topic) noexcept
{
    logInfo(DDSPIPE, "Deactivating topic: " << topic << ".");

    // Modify current_topics_ and set this topic as not active
    current_topics_[topic] = false;

    // Disable bridge. In case it is already disabled nothing should happen
    auto it_bridge = bridges_.find(topic);

    if (it_bridge != bridges_.end())
    {
        // The Bridge already exists
        it_bridge->second->disable();
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

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
