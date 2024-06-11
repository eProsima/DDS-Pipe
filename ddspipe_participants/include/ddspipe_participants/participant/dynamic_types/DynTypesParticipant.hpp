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

#include <memory>

#include <fastcdr/cdr/fixed_size_string.hpp>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/type_representation/detail/dds_xtypes_typeobject.hpp>
#include <fastdds/rtps/participant/RTPSParticipant.h>
#include <fastdds/rtps/reader/ReaderDiscoveryInfo.h>
#include <fastdds/rtps/writer/WriterDiscoveryInfo.h>

#include <ddspipe_participants/configuration/SimpleParticipantConfiguration.hpp>
#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/participant/rtps/SimpleParticipant.hpp>
#include <ddspipe_participants/reader/auxiliar/InternalReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * Simple RTPS Participant
 *
 * TODO: separate these 2 participants
 */
class DynTypesParticipant : public rtps::SimpleParticipant
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
    void onReaderDiscovery(
            fastrtps::rtps::RTPSParticipant* participant,
            fastrtps::rtps::ReaderDiscoveryInfo&& info) override;

    DDSPIPE_PARTICIPANTS_DllAPI
    void onWriterDiscovery(
            fastrtps::rtps::RTPSParticipant* participant,
            fastrtps::rtps::WriterDiscoveryInfo&& info) override;

protected:

    void notify_type_discovered_(
            const fastdds::dds::xtypes::TypeInformation& type_info,
            const fastcdr::string_255& type_name);

    //! Type Object Internal Reader
    std::shared_ptr<InternalReader> type_object_reader_;
};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
