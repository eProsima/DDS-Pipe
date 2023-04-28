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

/**
 * @file RpcBridge.cpp
 *
 */

#include <functional>


#include <cpp_utils/exception/InitializationException.hpp>
#include <cpp_utils/Log.hpp>
#include <cpp_utils/utils.hpp>

#include <ddspipe_core/types/data/RpcPayloadData.hpp>
#include <ddspipe_core/communication/rpc/RpcBridge.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

using namespace eprosima::ddspipe::core::types;

RpcBridge::RpcBridge(
        const RpcTopic& topic,
        const std::shared_ptr<ParticipantsDatabase>& participants_database,
        const std::shared_ptr<PayloadPool>& payload_pool,
        const std::shared_ptr<utils::SlotThreadPool>& thread_pool)
    : Bridge(participants_database, payload_pool, thread_pool)
    , init_(false)
    , rpc_topic_(topic)
{
    logDebug(DDSPIPE_RPCBRIDGE, "Creating RpcBridge " << *this << ".");

    logDebug(DDSPIPE_RPCBRIDGE, "RpcBridge " << *this << " created.");
}

RpcBridge::~RpcBridge()
{
    logDebug(DDSPIPE_RPCBRIDGE, "Destroying RpcBridge " << *this << ".");

    // Disable all entities before destruction
    disable();

    logDebug(DDSPIPE_RPCBRIDGE, "RpcBridge " << *this << " destroyed.");
}

void RpcBridge::init_nts_()
{
    logInfo(DDSPIPE_RPCBRIDGE, "Creating endpoints in RpcBridge for service " << rpc_topic_ << ".");

    // TODO: remove and use every participant
    std::set<ParticipantId> ids = participants_->get_rtps_participants_ids();

    // Create a proxy client and server in each RTPS participant
    for (ParticipantId id: ids)
    {
        create_proxy_client_nts_(id);
        create_proxy_server_nts_(id);
        if (current_servers_[id].size())
        {
            service_registries_[id]->enable();
        }
    }

    // TODO: This should not be done
    // Wait for the new entities created to match before sending data from one side to the other
    init_ = true;
    // utils::sleep_for(500);
}

void RpcBridge::create_proxy_server_nts_(
        ParticipantId participant_id)
{
    std::shared_ptr<IParticipant> participant = participants_->get_participant(participant_id);

    reply_writers_[participant_id] = participant->create_writer(rpc_topic_.reply_topic());
    request_readers_[participant_id] = participant->create_reader(rpc_topic_.request_topic());

    create_slot_(request_readers_[participant_id]);
}

void RpcBridge::create_proxy_client_nts_(
        ParticipantId participant_id)
{
    std::shared_ptr<IParticipant> participant = participants_->get_participant(participant_id);

    // Safe casting as we are only getting RTPS participants
    request_writers_[participant_id] = participant->create_writer(rpc_topic_.request_topic());
    reply_readers_[participant_id] = participant->create_reader(rpc_topic_.reply_topic());

    create_slot_(reply_readers_[participant_id]);

    // Create service registry associated to this proxy client
    service_registries_[participant_id] = std::make_shared<ServiceRegistry>(rpc_topic_, participant_id);
}

void RpcBridge::enable() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!enabled_ && servers_available_())
    {
        logInfo(DDSPIPE_RPCBRIDGE, "Enabling RpcBridge for service " << rpc_topic_ << ".");

        if (!init_)
        {
            try
            {
                init_nts_();
            }
            catch (const utils::InitializationException& e)
            {
                logError(DDSPIPE_RPCBRIDGE,
                        "Error while creating endpoints in RpcBridge for service " << rpc_topic_ <<
                        ". Error code:" << e.what() << ".");
                return;
            }
        }

        enabled_ = true;

        // Enable writers before readers, to avoid starting a transmission (not protected with \c mutex_) which may
        // attempt to write with a yet disabled writer

        for (auto& writer_it : request_writers_)
        {
            writer_it.second->enable();
        }

        for (auto& writer_it : reply_writers_)
        {
            writer_it.second->enable();
        }

        for (auto& reader_it : reply_readers_)
        {
            reader_it.second->enable();
        }

        for (auto& reader_it : request_readers_)
        {
            reader_it.second->enable();
        }
    }
}

void RpcBridge::disable() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (enabled_)
    {
        logInfo(DDSPIPE_RPCBRIDGE, "Disabling RpcBridge for service " << rpc_topic_ << ".");

        enabled_ = false;

        {
            // Wait for a transmission to finish before disabling endpoints
            std::unique_lock<std::shared_timed_mutex> lock(on_transmission_mutex_);
        }

        for (auto& reader_it : request_readers_)
        {
            reader_it.second->disable();
        }

        for (auto& reader_it : reply_readers_)
        {
            reader_it.second->disable();
        }

        for (auto& writer_it : reply_writers_)
        {
            writer_it.second->disable();
        }

        for (auto& writer_it : request_writers_)
        {
            writer_it.second->disable();
        }
    }
}

void RpcBridge::discovered_service(
        const types::ParticipantId& server_participant_id,
        const types::GuidPrefix& server_guid_prefix) noexcept
{
    current_servers_[server_participant_id].emplace(server_guid_prefix);
    if (init_)
    {
        service_registries_[server_participant_id]->enable();
    }
    else
    {
    }
}

void RpcBridge::removed_service(
        const types::ParticipantId& server_participant_id,
        const types::GuidPrefix& server_guid_prefix) noexcept
{
    current_servers_[server_participant_id].erase(server_guid_prefix);
    if (!current_servers_[server_participant_id].size() && !servers_available_())
    {
        disable();
    }
}

bool RpcBridge::servers_available_() const noexcept
{
    for (auto it = current_servers_.begin(); it != current_servers_.end(); it++)
    {
        if (it->second.size())
        {
            return true;
        }
    }
    return false;
}

void RpcBridge::data_available_(
        const Guid& reader_guid) noexcept
{
    // Only hear callback if it is enabled
    if (enabled_)
    {
        logDebug(
            DDSPIPE_RPCBRIDGE, "RpcBridge " << *this <<
                " has data ready to be sent in reader " << reader_guid << " .");

        // Protected by internal RTPS Reader mutex, as called within \c onNewCacheChangeAdded callback
        // This method is also called from Reader's \c enable_ , so Reader's mutex must also be taken there beforehand
        std::pair<bool, utils::TaskId>& task = tasks_map_[reader_guid];
        if (!task.first)
        {
            task.first = true;
            thread_pool_->emit(task.second);
            logDebug(DDSPIPE_RPCBRIDGE, "RpcBridge " << *this <<
                    " - " << reader_guid << " send callback to queue.");
        }
        else
        {
            logDebug(DDSPIPE_RPCBRIDGE, "RpcBridge " << *this <<
                    " - " << reader_guid << " callback NOT sent (task already queued).");
        }
    }
}

void RpcBridge::transmit_(
        std::shared_ptr<IReader> reader) noexcept
{
    // Avoid being disabled while transmitting
    std::shared_lock<std::shared_timed_mutex> lock(on_transmission_mutex_);

    logDebug(DDSPIPE_RPCBRIDGE, "RpcBridge " << *this <<
            " transmitting for reader " << reader->guid() << " .");

    while (true)
    {
        {
            std::lock_guard<eprosima::fastrtps::RecursiveTimedMutex> lock(reader->get_rtps_mutex());

            if (!enabled_ || !(reader->get_unread_count() > 0))
            {
                if (!enabled_)
                {
                    logDebug(DDSPIPE_RPCBRIDGE,
                            "RpcBridge service " << *this << " finishing transmitting: bridge disabled.");
                }
                else
                {
                    logDebug(DDSPIPE_RPCBRIDGE,
                            "RpcBridge service " << *this << " finishing transmitting: no more data available.");
                }

                // Finish transmission
                tasks_map_[reader->guid()].first = false;

                return;
            }
        }

        // Get data received
        std::unique_ptr<IRoutingData> data;
        utils::ReturnCode ret = reader->take(data);

        RpcPayloadData& rpc_data = dynamic_cast<RpcPayloadData&>(*data);


        // Will never return \c RETCODE_NO_DATA, otherwise would have finished before
        if (!ret)
        {
            // Error reading data
            logWarning(DDSPIPE_RPCBRIDGE,
                    "Error taking data at service Reader in topic " << reader->topic()
                                                                    << ". Error code " << ret
                                                                    << ". Skipping data and continue.");
            continue;
        }

        if (RpcTopic::is_request_topic(reader->topic()))
        {
            logDebug(DDSPIPE_RPCBRIDGE,
                    "RpcBridge for service " << rpc_topic_ <<
                    " transmitting request from remote endpoint " << rpc_data.source_guid << ".");

            SampleIdentity reply_related_sample_identity =
                    rpc_data.write_params.get_reference().sample_identity();
            reply_related_sample_identity.sequence_number(rpc_data.origin_sequence_number);

            if (reply_related_sample_identity == SampleIdentity::unknown())
            {
                logWarning(DDSPIPE_RPCBRIDGE,
                        "RpcBridge for service " << rpc_topic_ <<
                        " received ill-formed request from remote endpoint " << rpc_data.source_guid <<
                        ". Ignoring...");
            }
            else
            {
                for (auto& service_registry : service_registries_)
                {
                    // Do not send request through same participant who received it (unless repeater), or if there are no servers to process it
                    if ((rpc_data.participant_receiver == service_registry.first &&
                            !participants_->get_participant(service_registry.first)->is_repeater()) ||
                            !service_registry.second->enabled())
                    {
                        continue;
                    }

                    // Perform write + add entry to registry atomically -> avoid reply processed before entry added to registry
                    std::lock_guard<std::recursive_mutex> lock(service_registry.second->get_mutex());

                    // Attach the information the server needs in order to reply to the appropiate proxy client.
                    rpc_data.write_params.set_level();
                    rpc_data.write_params.get_reference().related_sample_identity().writer_guid(
                        reply_readers_[service_registry.first]->guid());


                    ret = request_writers_[service_registry.first]->write(*data);

                    if (!ret)
                    {
                        logWarning(DDSPIPE_RPCBRIDGE, "Error writting request in RpcBridge for service "
                                << rpc_topic_ << ". Error code " << ret <<
                                ". Skipping data for this writer and continue.");
                        continue;
                    }

                    eprosima::fastrtps::rtps::SequenceNumber_t sequence_number =
                            rpc_data.sent_sequence_number;
                    // Add entry to registry associated to the transmission of this request through this proxy client.
                    service_registry.second->add(
                        sequence_number,
                        {rpc_data.participant_receiver, reply_related_sample_identity});

                }
            }
        }
        else if (RpcTopic::is_reply_topic(reader->topic()))
        {
            logDebug(DDSPIPE_RPCBRIDGE,
                    "RpcBridge for service " << rpc_topic_ <<
                    " transmitting reply from remote endpoint " << rpc_data.source_guid << ".");

            // A Server could be answering a different client in this same DDS Pipe or a remote client
            // Thus, it must be filtered so only replies to this client are processed.
            if (rpc_data.write_params.get_reference().sample_identity().writer_guid() != reader->guid())
            {
                logDebug(DDSPIPE_RPCBRIDGE,
                        "RpcBridge for service " << *this << " from reader " << reader->guid() <<
                        " received response meant for other client: " <<
                        rpc_data.write_params.get_reference().sample_identity().writer_guid());
            }
            else
            {
                std::pair<ParticipantId, SampleIdentity> registry_entry;
                {
                    // Wait for request transmission to be finished (entry added to registry)
                    std::lock_guard<std::recursive_mutex> lock(
                        service_registries_[reader->participant_id()]->get_mutex());

                    // Fetch information required for transmission; which proxy server should send it and with what parameters
                    registry_entry = service_registries_[reader->participant_id()]->get(
                        rpc_data.write_params.get_reference().sample_identity().sequence_number());
                }

                // Not valid means:
                //   Case 1: (SimpleParticipant) Request already replied by another server connected to the same participant as this one.
                //   Case 2: (WAN Participant repeater) Request already replied by another PROXY server connected to the same participant as this one.
                // TODO: recheck ParticipantId non valid
                if (!registry_entry.first.empty())
                {
                    rpc_data.write_params.set_level();
                    rpc_data.write_params.get_reference().related_sample_identity(registry_entry.second);

                    ret = reply_writers_[registry_entry.first]->write(*data);

                    if (!ret)
                    {
                        logWarning(DDSPIPE_RPCBRIDGE, "Error writting reply in RpcBridge for service "
                                << rpc_topic_ << ". Error code " << ret << ".");
                    }
                    else
                    {
                        service_registries_[reader->participant_id()]->erase(
                            rpc_data.write_params.get_reference().sample_identity().sequence_number());
                    }
                }
            }
        }
        else
        {
            utils::tsnh(
                utils::Formatter() << "Data to be transmitted in RpcBridge is not in RpcTopic.");
        }

        payload_pool_->release_payload(rpc_data.payload);
    }/*  */
}

void RpcBridge::create_slot_(
        std::shared_ptr<IReader> reader) noexcept
{
    Guid reader_guid = reader->guid();

    reader->set_on_data_available_callback(
        [=]()
        {
            data_available_(reader_guid);
        });

    // Set slot in thread pool for this reader
    utils::TaskId task_id = utils::new_unique_task_id();
    thread_pool_->slot(
        task_id,
        [=]()
        {
            transmit_(reader);
        });
    tasks_map_[reader_guid] = {false, task_id};
}

std::ostream& operator <<(
        std::ostream& os,
        const RpcBridge& bridge)
{
    os << "RpcBridge{" << bridge.rpc_topic_ << "}";
    return os;
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
