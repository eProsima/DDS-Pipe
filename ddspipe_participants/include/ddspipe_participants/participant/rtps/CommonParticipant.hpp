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

#pragma once

#include <cpp_utils/memory/Heritable.hpp>

#include <fastdds/rtps/attributes/RTPSParticipantAttributes.hpp>
#include <fastdds/rtps/builtin/data/ParticipantBuiltinTopicData.hpp>
#include <fastdds/rtps/builtin/data/PublicationBuiltinTopicData.hpp>
#include <fastdds/rtps/builtin/data/SubscriptionBuiltinTopicData.hpp>
#include <fastdds/rtps/participant/ParticipantDiscoveryInfo.hpp>
#include <fastdds/rtps/participant/RTPSParticipantListener.hpp>
#include <fastdds/rtps/reader/ReaderDiscoveryStatus.hpp>
#include <fastdds/rtps/RTPSDomain.hpp>
#include <fastdds/rtps/writer/WriterDiscoveryStatus.hpp>

#include <ddspipe_core/dynamic/DiscoveryDatabase.hpp>
#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>
#include <ddspipe_core/interface/IParticipant.hpp>
#include <ddspipe_core/types/dds/DomainId.hpp>
#include <ddspipe_core/types/dds/TopicQoS.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

#include <ddspipe_participants/configuration/ParticipantConfiguration.hpp>
#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/types/address/Address.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace rtps {

/**
 * Abstract generic class for a RTPS Participant wrapper.
 *
 * Concrete classes that inherit from this would only need to specialize specific methods related with the
 * qos and attributes.
 *
 * @warning This object is not RAII and must be initialized before used.
 */
class CommonParticipant
    : public core::IParticipant
    , public fastdds::rtps::RTPSParticipantListener
{
public:

    /////////////////////////
    // CONSTRUCTORS
    /////////////////////////

    // Protected ctor to make class abstract (only built by their childs).

    //! Remove internal RTPS Participant
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual ~CommonParticipant();

    /**
     * @brief Create the internal RTPS Participant using the attributes given.
     *
     * @attention this method should be called right after constructor to create enable internal entities.
     * This is required as this object is a Listener that could be called before finishing construction.
     * Other alternatives have been studied but none have really fit for this case.
     *
     * @throw InitializationException if RTPS Participant creation fails
     *
     * @warning this method is not thread safe.
     * @pre this method can only be called once.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void init();

    /////////////////////////
    // I PARTICIPANT METHODS
    /////////////////////////

    //! Implement parent method \c is_repeater .
    DDSPIPE_PARTICIPANTS_DllAPI
    bool is_repeater() const noexcept override;

    //! Implement parent method \c id .
    DDSPIPE_PARTICIPANTS_DllAPI
    core::types::ParticipantId id() const noexcept override;

    //! Implement parent method \c is_rtps_kind .
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool is_rtps_kind() const noexcept override;

    //! Implement parent method \c topic_qos .
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
    // RTPS LISTENER METHODS
    /////////////////////////

    /**
     * @brief Override method from \c RTPSParticipantListener .
     *
     * This method only is for debugging purposes.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void on_participant_discovery(
            fastdds::rtps::RTPSParticipant* participant,
            fastdds::rtps::ParticipantDiscoveryStatus reason,
            const fastdds::rtps::ParticipantBuiltinTopicData& info,
            bool& /*should_be_ignored*/) override;

    /**
     * @brief Override method from \c RTPSParticipantListener .
     *
     * This method adds to database the endpoint discovered or modified.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void on_reader_discovery(
            fastdds::rtps::RTPSParticipant* participant,
            fastdds::rtps::ReaderDiscoveryStatus reason,
            const fastdds::rtps::SubscriptionBuiltinTopicData& info,
            bool& /*should_be_ignored*/) override;

    /**
     * @brief Override method from \c RTPSParticipantListener .
     *
     * This method adds to database the endpoint discovered or modified.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void on_writer_discovery(
            fastdds::rtps::RTPSParticipant* participant,
            fastdds::rtps::WriterDiscoveryStatus reason,
            const fastdds::rtps::PublicationBuiltinTopicData& info,
            bool& /*should_be_ignored*/) override;

    //////////////////
    // STATIC METHODS
    //////////////////

    /**
     * @brief Create an endpoint in a given topic with unique GUID.
     *
     * The created endpoint is of reader kind, and the \c discoverer_id is user-provided.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    static core::types::Endpoint simulate_endpoint(
            const core::types::DdsTopic& topic,
            const core::types::ParticipantId& discoverer_id);

    /**
     * @brief Create a transport descriptor with given whitelist.
     *
     * This templated method is specialized for UPDv4, UDPv6, TCPv4 and TCPv6.
     */
    template<typename T>
    DDSPIPE_PARTICIPANTS_DllAPI
    static std::shared_ptr<T> create_descriptor(
            std::set<types::WhitelistType> whitelist = {});

protected:

    /**
     * @brief Construct a CommonParticipant
     *
     * @note This is meant to be called from child classes.
     */
    CommonParticipant(
            const std::shared_ptr<ParticipantConfiguration>& participant_configuration,
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            const std::shared_ptr<core::DiscoveryDatabase>& discovery_database,
            const core::types::DomainId& domain_id);

    /**
     * @brief Auxiliary method to create the internal RTPS participant.
     */
    void create_participant_(
            const core::types::DomainId& domain,
            const fastdds::rtps::RTPSParticipantAttributes& participant_attributes);

    /////
    // RTPS specific methods

    /**
     * @brief Virtual method that sets the common properties of std attributes for a Participant.
     *
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual fastdds::rtps::RTPSParticipantAttributes add_participant_att_properties_(
            fastdds::rtps::RTPSParticipantAttributes& params) const;

    /**
     * @brief Virtual method that gives the std attributes for a Participant.
     *
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual fastdds::rtps::RTPSParticipantAttributes reckon_participant_attributes_() const;

    /////
    // VARIABLES

    //! Participant configuration
    const std::shared_ptr<ParticipantConfiguration> configuration_;

    //! DDS Router shared Payload Pool
    const std::shared_ptr<core::PayloadPool> payload_pool_;

    //! DDS Router shared Discovery Database
    const std::shared_ptr<core::DiscoveryDatabase> discovery_database_;

    //! Internal RTPS Participant
    eprosima::fastdds::rtps::RTPSParticipant* rtps_participant_;

    //! Domain Id to create the internal RTPS Participant.
    core::types::DomainId domain_id_;

    //! Participant attributes to create the internal RTPS Participant.
    fastdds::rtps::RTPSParticipantAttributes participant_attributes_;
};

} /* namespace rtps */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
