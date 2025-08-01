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

#pragma once

#include <mutex>

#include <cpp_utils/types/Atomicable.hpp>

#include <fastdds/dds/builtin/topic/ParticipantBuiltinTopicData.hpp>
#include <fastdds/dds/builtin/topic/PublicationBuiltinTopicData.hpp>
#include <fastdds/dds/builtin/topic/SubscriptionBuiltinTopicData.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/rtps/participant/ParticipantDiscoveryInfo.hpp>
#include <fastdds/rtps/reader/ReaderDiscoveryStatus.hpp>
#include <fastdds/rtps/writer/WriterDiscoveryStatus.hpp>

#include <ddspipe_core/dynamic/DiscoveryDatabase.hpp>
#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>
#include <ddspipe_core/interface/IParticipant.hpp>
#include <ddspipe_core/types/dds/TopicQoS.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

#include <ddspipe_participants/configuration/SimpleParticipantConfiguration.hpp>
#include <ddspipe_participants/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {


/**
 * Abstract generic class for a Fast DDS Participant wrapper.
 *
 * Concrete classes that inherit from this would only need to specialize specific methods related with qos:
 * - \c reckon_participant_qos_()
 * and the creation of entities:
 * - \c create_writer()
 * - \c create_reader()
 * - \c create_dds_participant_() (only required for specific participant creation).
 *
 * @warning This object is not RAII and must be initialized before used using \c init .
 *
 * @warning This Participant class does not support RPC so far.
 * @todo TODO
 */
class CommonParticipant : public core::IParticipant
{
public:

    /////////////////////////
    // CTOR / DTOR
    /////////////////////////

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual ~CommonParticipant();

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void init();

    /////////////////////////
    // IPARTICIPANT METHODS
    /////////////////////////

    DDSPIPE_PARTICIPANTS_DllAPI
    core::types::ParticipantId id() const noexcept override;

    DDSPIPE_PARTICIPANTS_DllAPI
    bool is_rtps_kind() const noexcept override;

    DDSPIPE_PARTICIPANTS_DllAPI
    bool is_repeater() const noexcept override;

    DDSPIPE_PARTICIPANTS_DllAPI
    core::types::TopicQoS topic_qos() const noexcept override;

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

    /////////////////////////
    // LISTENER METHODS
    /////////////////////////

    class DdsListener : public fastdds::dds::DomainParticipantListener
    {
    public:

        DDSPIPE_PARTICIPANTS_DllAPI
        explicit DdsListener(
                std::shared_ptr<SimpleParticipantConfiguration> conf,
                std::shared_ptr<core::DiscoveryDatabase> ddb);

        /**
         * @brief Override method from \c DomainParticipantListener
         *
         * This method is only used for debugging purposes.
         */
        DDSPIPE_PARTICIPANTS_DllAPI
        void on_participant_discovery(
                fastdds::dds::DomainParticipant* participant,
                fastdds::rtps::ParticipantDiscoveryStatus reason,
                const fastdds::rtps::ParticipantBuiltinTopicData& info,
                bool& /*should_be_ignored*/) override;

        /**
         * @brief Override method from \c DomainParticipantListener .
         *
         * This method adds to the database the discovered or modified endpoint.
         */
        DDSPIPE_PARTICIPANTS_DllAPI
        void on_data_reader_discovery(
                fastdds::dds::DomainParticipant* participant,
                fastdds::rtps::ReaderDiscoveryStatus reason,
                const fastdds::dds::SubscriptionBuiltinTopicData& info,
                bool& /*should_be_ignored*/) override;

        /**
         * @brief Override method from \c DomainParticipantListener .
         *
         * This method adds to the database the discovered or modified endpoint.
         */
        DDSPIPE_PARTICIPANTS_DllAPI
        void on_data_writer_discovery(
                fastdds::dds::DomainParticipant* participant,
                fastdds::rtps::WriterDiscoveryStatus reason,
                const fastdds::dds::PublicationBuiltinTopicData& info,
                bool& /*should_be_ignored*/) override;

    protected:

        //! Shared pointer to the configuration of the participant
        const std::shared_ptr<SimpleParticipantConfiguration> configuration_;
        //! Shared pointer to the discovery database
        const std::shared_ptr<core::DiscoveryDatabase> discovery_database_;

    };

    //! Unique pointer to the internal DDS Participant Listener
    std::unique_ptr<fastdds::dds::DomainParticipantListener> dds_participant_listener_;

protected:

    /////////////////////////
    // PROTECTED CTOR
    /////////////////////////

    DDSPIPE_PARTICIPANTS_DllAPI
    CommonParticipant(
            const std::shared_ptr<SimpleParticipantConfiguration>& participant_configuration,
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            const std::shared_ptr<core::DiscoveryDatabase>& discovery_database);

    /////////////////////////
    // VIRTUAL METHODS
    /////////////////////////

    /**
     * @brief Virtual method that creates a listener for the internal DDS Participant.
     *        It should be overridden if a different listener is needed.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual std::unique_ptr<fastdds::dds::DomainParticipantListener> create_listener_();

    /////////////////////////
    // INTERNAL VIRTUAL METHODS
    /////////////////////////

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual
    fastdds::dds::DomainParticipantQos
    reckon_participant_qos_() const;

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual
    void
    add_qos_properties_(
            fastdds::dds::DomainParticipantQos& qos) const;

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual
    fastdds::dds::DomainParticipant*
    create_dds_participant_();

    /////////////////////////
    // INTERNAL METHODS
    /////////////////////////

    fastdds::dds::Topic* topic_related_(
            const core::types::DdsTopic& topic);

    /////////////////////////
    // INTERNAL VARIABLES
    /////////////////////////

    fastdds::dds::DomainParticipant* dds_participant_{nullptr};

    // NOTE: the publisher and subscriber are inside each endpoint because partitions depend on them.

    using TopicsMapType = utils::Atomicable<
        std::map<
            core::types::DdsTopic,
            fastdds::dds::Topic*>>;
    TopicsMapType dds_topics_;

    /**
     * @brief
     *
     * @note this variable is protected with dds_topics_ mutex
     */
    std::set<std::string> type_names_registered_;

    const std::shared_ptr<SimpleParticipantConfiguration> configuration_;

    //! DDS Router shared Payload Pool
    const std::shared_ptr<core::PayloadPool> payload_pool_;

    //! DDS Router shared Discovery Database
    const std::shared_ptr<core::DiscoveryDatabase> discovery_database_;
};

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
