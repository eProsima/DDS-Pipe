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

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

#include <ddspipe_core/interface/IParticipant.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/dynamic/DiscoveryDatabase.hpp>
#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>

#include <ddspipe_participants/configuration/SimpleParticipantConfiguration.hpp>
#include <ddspipe_participants/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

/**
 * TODO comment
 */
class CommonParticipant : public core::IParticipant , public fastdds::dds::DomainParticipantListener
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
    virtual core::types::ParticipantId id() const noexcept override;

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool is_rtps_kind() const noexcept override;

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool is_repeater() const noexcept override;

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

    virtual void on_subscriber_discovery(
            fastdds::dds::DomainParticipant* participant,
            fastrtps::rtps::ReaderDiscoveryInfo&& info) override;

    virtual void on_publisher_discovery(
            fastdds::dds::DomainParticipant* participant,
            fastrtps::rtps::WriterDiscoveryInfo&& info) override;

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
    // INTERNAL VIRTUAL METHODS
    /////////////////////////

    virtual
    fastdds::dds::DomainParticipantQos
    reckon_participant_qos_() const;

    virtual
    fastdds::dds::DomainParticipant*
    create_dds_participant_();

    /////////////////////////
    // INTERNAL METHODS
    /////////////////////////

    /**
     * @brief Create a endpoint from info object
     *
     * Specialized for \c WriterDiscoveryInfo and \c ReaderDiscoveryInfo .
     */
    template<class DiscoveryInfoKind>
    core::types::Endpoint create_endpoint_from_info_(
            DiscoveryInfoKind& info);

    //! Create a endpoint from common info from method \c create_endpoint_from_info_ .
    template<class DiscoveryInfoKind>
    core::types::Endpoint create_common_endpoint_from_info_(
            DiscoveryInfoKind& info);

    fastdds::dds::Topic* topic_related_(const core::types::DdsTopic& topic);

    /////////////////////////
    // INTERNAL VARIABLES
    /////////////////////////

    fastdds::dds::DomainParticipant* dds_participant_{nullptr};
    fastdds::dds::Publisher* dds_publisher_{nullptr};
    fastdds::dds::Subscriber* dds_subscriber_{nullptr};

    using TopicsMapType = utils::Atomicable<
        std::map<
            core::types::DdsTopic,
            fastdds::dds::Topic*>>;
    TopicsMapType dds_topics_;

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
