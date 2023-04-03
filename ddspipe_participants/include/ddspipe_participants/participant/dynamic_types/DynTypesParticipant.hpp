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
 * @file DynTypesParticipant.hpp
 */

#pragma once

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>

#include <ddspipe_participants/configuration/SimpleParticipantConfiguration.hpp>
#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/participant/rtps/SimpleParticipant.hpp>
#include <ddspipe_participants/reader/auxiliar/InternalReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * This is an abomination Participant that is a Simple RTPS Participant with a built-in DDS Participant.
 * The DDS Part that is only used to read type objects and type lookup services.
 *
 * TODO: separate these 2 participants
 */
class DynTypesParticipant : public rtps::SimpleParticipant, public eprosima::fastdds::dds::DomainParticipantListener
{
public:

    // TODO
    DDSPIPE_PARTICIPANTS_DllAPI
    DynTypesParticipant(
            std::shared_ptr<SimpleParticipantConfiguration> participant_configuration,
            std::shared_ptr<core::PayloadPool> payload_pool,
            std::shared_ptr<core::DiscoveryDatabase> discovery_database);

    DDSPIPE_PARTICIPANTS_DllAPI
    ~DynTypesParticipant();

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void init() override;

    /**
     * @brief Create a writer object
     *
     * Depending on the Topic QoS creates a Basic or Specific Writer.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<core::IWriter> create_writer(
            const core::ITopic& topic) override;

    /**
     * @brief Create a reader object
     *
     * Depending on the Topic QoS creates a Basic or Specific Reader.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<core::IReader> create_reader(
            const core::ITopic& topic) override;

    DDSPIPE_PARTICIPANTS_DllAPI
    void on_type_discovery(
            eprosima::fastdds::dds::DomainParticipant* participant,
            const eprosima::fastrtps::rtps::SampleIdentity& request_sample_id,
            const eprosima::fastrtps::string_255& topic,
            const eprosima::fastrtps::types::TypeIdentifier* identifier,
            const eprosima::fastrtps::types::TypeObject* object,
            eprosima::fastrtps::types::DynamicType_ptr dyn_type) override;

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void on_type_information_received(
            eprosima::fastdds::dds::DomainParticipant* participant,
            const eprosima::fastrtps::string_255 topic_name,
            const eprosima::fastrtps::string_255 type_name,
            const eprosima::fastrtps::types::TypeInformation& type_information) override;

protected:

    void internal_notify_type_object_(
            eprosima::fastrtps::types::DynamicType_ptr dynamic_type);

    void initialize_internal_dds_participant_();

    eprosima::fastdds::dds::DomainParticipant* dds_participant_;

    //! Type Object Internal Reader
    std::shared_ptr<InternalReader> type_object_reader_;
};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
