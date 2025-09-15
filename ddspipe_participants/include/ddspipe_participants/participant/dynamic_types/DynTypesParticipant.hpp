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
#include <set>

#include <fastcdr/cdr/fixed_size_string.hpp>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/type_representation/detail/dds_xtypes_typeobject.hpp>
#include <fastdds/rtps/builtin/data/PublicationBuiltinTopicData.hpp>
#include <fastdds/rtps/builtin/data/SubscriptionBuiltinTopicData.hpp>
#include <fastdds/rtps/participant/RTPSParticipant.hpp>
#include <fastdds/rtps/reader/ReaderDiscoveryStatus.hpp>
#include <fastdds/rtps/writer/WriterDiscoveryStatus.hpp>

#include <ddspipe_participants/configuration/SimpleParticipantConfiguration.hpp>
#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/participant/rtps/SimpleParticipant.hpp>
#include <ddspipe_participants/reader/auxiliar/InternalReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * Simple RTPS Participant able to route dynamic types in addition to user data.
 *
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
    std::shared_ptr<core::IReader> create_reader_with_filter(
            const core::ITopic& topic,
            const std::set<std::string> partitions) override;

    class DynTypesRtpsListener : public rtps::CommonParticipant::RtpsListener
    {
    public:

        DDSPIPE_PARTICIPANTS_DllAPI
        explicit DynTypesRtpsListener(
                std::shared_ptr<ParticipantConfiguration> conf,
                std::shared_ptr<core::DiscoveryDatabase> ddb,
                std::shared_ptr<InternalReader> internal_reader);

        DDSPIPE_PARTICIPANTS_DllAPI
        void on_reader_discovery(
                fastdds::rtps::RTPSParticipant* participant,
                fastdds::rtps::ReaderDiscoveryStatus reason,
                const fastdds::rtps::SubscriptionBuiltinTopicData& info,
                bool& should_be_ignored) override;

        DDSPIPE_PARTICIPANTS_DllAPI
        void on_writer_discovery(
                fastdds::rtps::RTPSParticipant* participant,
                fastdds::rtps::WriterDiscoveryStatus reason,
                const fastdds::rtps::PublicationBuiltinTopicData& info,
                bool& should_be_ignored) override;

    protected:

        //! Copy of Type Object Internal Reader
        std::shared_ptr<InternalReader> type_object_reader_;
        //! Received types set
        std::set<std::string> received_types_;

        void notify_type_discovered_(
                const fastdds::dds::xtypes::TypeInformation& type_info,
                const std::string& type_name);

    };

protected:

    //! Override method from \c CommonParticipant to create the internal RTPS participant listener
    DDSPIPE_PARTICIPANTS_DllAPI
    std::unique_ptr<fastdds::rtps::RTPSParticipantListener> create_listener_() override;

    //! Type Object Internal Reader
    std::shared_ptr<InternalReader> type_object_reader_;

};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
