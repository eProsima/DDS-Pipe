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
 * @file XmlDynTypesParticipant.hpp
 * @brief Defines the XmlDynTypesParticipant class for routing dynamic types in DDS.
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

#include <ddspipe_participants/configuration/XmlParticipantConfiguration.hpp>
#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/participant/dds/XmlParticipant.hpp>
#include <ddspipe_participants/reader/auxiliar/InternalReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * @class XmlDynTypesParticipant
 * @brief A specialized RTPS participant capable of routing dynamic types and user data.
 */
class XmlDynTypesParticipant : public dds::XmlParticipant
{
public:

    /**
     * @brief Constructs an XmlDynTypesParticipant.
     *
     * @param participant_configuration Shared pointer to the participant configuration.
     * @param payload_pool Shared pointer to the payload pool.
     * @param discovery_database Shared pointer to the discovery database.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    XmlDynTypesParticipant(
            std::shared_ptr<XmlParticipantConfiguration> participant_configuration,
            std::shared_ptr<core::PayloadPool> payload_pool,
            std::shared_ptr<core::DiscoveryDatabase> discovery_database,
            bool has_filter = false);

    /**
     * @brief Creates a writer object based on the topic QoS.
     *
     * @param topic The topic for which the writer is created.
     * @return A shared pointer to the created writer.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<core::IWriter> create_writer(
            const core::ITopic& topic) override;

    /**
     * @brief Creates a reader object based on the topic QoS.
     *
     * @param topic The topic for which the reader is created.
     * @return A shared pointer to the created reader.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<core::IReader> create_reader(
            const core::ITopic& topic) override;

    /**
     * @class XmlDynTypesDdsListener
     * @brief Listener for handling DDS discovery events for dynamic types.
     */
    class XmlDynTypesDdsListener : public dds::CommonParticipant::DdsListener
    {
    public:

        /**
         * @brief Constructs an XmlDynTypesDdsListener.
         *
         * @param conf Shared pointer to the participant configuration.
         * @param ddb Shared pointer to the discovery database.
         * @param internal_reader Shared pointer to the internal reader.
         */
        DDSPIPE_PARTICIPANTS_DllAPI
        explicit XmlDynTypesDdsListener(
                std::shared_ptr<SimpleParticipantConfiguration> conf,
                std::shared_ptr<core::DiscoveryDatabase> ddb,
                std::shared_ptr<InternalReader> internal_reader);

        /**
         * @brief Callback for data reader discovery events.
         *
         * @param participant The domain participant.
         * @param reason The discovery reason.
         * @param info The discovered subscription information.
         * @param should_be_ignored Whether the reader should be ignored.
         */
        DDSPIPE_PARTICIPANTS_DllAPI
        void on_data_reader_discovery(
                fastdds::dds::DomainParticipant* participant,
                fastdds::rtps::ReaderDiscoveryStatus reason,
                const fastdds::dds::SubscriptionBuiltinTopicData& info,
                bool& /*should_be_ignored*/) override;

        /**
         * @brief Callback for data writer discovery events.
         *
         * @param participant The domain participant.
         * @param reason The discovery reason.
         * @param info The discovered publication information.
         * @param should_be_ignored Whether the writer should be ignored.
         */
        DDSPIPE_PARTICIPANTS_DllAPI
        void on_data_writer_discovery(
                fastdds::dds::DomainParticipant* participant,
                fastdds::rtps::WriterDiscoveryStatus reason,
                const fastdds::dds::PublicationBuiltinTopicData& info,
                bool& /*should_be_ignored*/) override;

    protected:

        /// Internal reader for type objects.
        std::shared_ptr<InternalReader> type_object_reader_;

        /// Set of received types.
        std::set<std::string> received_types_;

        /**
         * @brief Notifies that a type has been discovered.
         *
         * @param type_info The discovered type information.
         * @param type_name The name of the discovered type.
         */
        void notify_type_discovered_(
                const fastdds::dds::xtypes::TypeInformation& type_info,
                const std::string& type_name);
    };

protected:

    /**
     * @brief Creates the internal RTPS participant listener.
     *
     * @return A unique pointer to the created listener.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    std::unique_ptr<fastdds::dds::DomainParticipantListener> create_listener_() override;

    /// Internal reader for type objects.
    std::shared_ptr<InternalReader> type_object_reader_;
};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
