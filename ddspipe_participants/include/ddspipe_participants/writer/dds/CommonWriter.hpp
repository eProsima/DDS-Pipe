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

#include <cpp_utils/time/time_utils.hpp>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>

#include <ddspipe_core/efficiency/payload/PayloadPoolMediator.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/data/RtpsPayloadData.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/efficiency/cache_change/CacheChangePool.hpp>
#include <ddspipe_participants/writer/auxiliar/BaseWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

/**
 * Abstract generic class for a DDS DataWriter wrapper.
 *
 * @warning This object is not RAII and must be initialized before used.
 */
class CommonWriter : public BaseWriter
{
public:

    /////////////////////////
    // CONSTRUCTOR
    /////////////////////////

    // Protected ctor to make class abstract (only built by their childs).

    /**
     * @brief Destroy the Writer object
     *
     * Delete the DDS Writer and Publisher in case they are set.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual ~CommonWriter();

    /**
     * @brief Create the internal RTPS Writer.
     *
     * @attention this method should be called right after constructor to create enable internal entities.
     * This is required as this object is a Listener that could be called before finishing construction.
     * Other alternatives have been studied but none have really fit for this case.
     *
     * @throw InitializationException if RTPS Writer creation fails
     *
     * @warning this method is not thread safe.
     * @pre this method can only be called once.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    void init();

    /////////////////////
    // STATIC ATTRIBUTES
    /////////////////////

    //! Maximum time that a RELIABLE RTPSWriter will wait to receive the acknowledgements relative to all sent messages.
    DDSPIPE_PARTICIPANTS_DllAPI
    static std::atomic<utils::Duration_ms> wait_all_acked_timeout;

protected:

    /**
     * @brief Construct a new Writer object
     *
     * It receives all the attributes and QoS needed to create the internal entities.
     *
     * @note Only protected so only concrete classes are instantiated.
     *
     * @throw \c InitializationException in case any creation has failed
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    CommonWriter(
            const core::types::ParticipantId& participant_id,
            const core::types::DdsTopic& topic,
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            fastdds::dds::DomainParticipant* participant,
            fastdds::dds::Topic* topic_entity);

    /////////////////////////
    // IWRITER METHODS
    /////////////////////////

    /**
     * @brief Write specific method
     *
     * Store new data as message to send (synchronously) (it could use PayloadPool to not copy payload).
     * Take next Untaken Change.
     * Set \c data with the message taken (data payload must be stored from PayloadPool).
     * Remove this change from Reader History and release.
     *
     * It does not require mutex, it will be guarded by RTPS CommonWriter mutex in internal methods.
     *
     * @param data : oldest data to take
     * @return \c RETCODE_OK if data has been correctly taken
     * @return \c RETCODE_NO_DATA if \c data is empty
     * @return \c RETCODE_ERROR if error occurred
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual utils::ReturnCode write_nts_(
            core::IRoutingData& data) noexcept override;

    /////////////////////////
    // INTERNAL METHODS
    /////////////////////////

    virtual
    fastdds::dds::PublisherQos
    reckon_publisher_qos_() const noexcept;

    virtual
    fastdds::dds::DataWriterQos
    reckon_writer_qos_() const noexcept;

    /////////////////////////
    // EXTERNAL VARIABLES
    /////////////////////////

    fastdds::dds::DomainParticipant* dds_participant_;
    fastdds::dds::Topic* dds_topic_;

    /////////////////////////
    // INTERNAL VARIABLES
    /////////////////////////

    std::shared_ptr<core::PayloadPoolMediator> payload_pool_;

    core::types::DdsTopic topic_;

    fastdds::dds::Publisher* dds_publisher_;
    fastdds::dds::DataWriter* writer_;
};

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
