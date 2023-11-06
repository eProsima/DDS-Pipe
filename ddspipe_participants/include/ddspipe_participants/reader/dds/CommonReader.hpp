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

#include <cpp_utils/time/time_utils.hpp>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/Topic.hpp>

#include <ddspipe_core/types/data/RtpsPayloadData.hpp>
#include <ddspipe_core/types/dds/Guid.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/reader/auxiliar/BaseReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

/**
 * Abstract generic class for a DDS DataReader wrapper.
 *
 * It implements the ReaderListener for itself with \c on_data_available callback.
 *
 * @warning This object is not RAII and must be initialized before used.
 */
class CommonReader : public BaseReader, public fastdds::dds::DataReaderListener
{
public:

    /////////////////////////
    // CTOR / DTOR
    /////////////////////////

    // Protected ctor to make class abstract (only built by their childs).

    /**
     * @brief Destroy the CommonReader object
     *
     * Delete the DDS CommonReader and Subscriber in case they are set.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual ~CommonReader();

    DDSPIPE_PARTICIPANTS_DllAPI
    void init();

    /////////////////////////
    // RTPS LISTENER METHODS
    /////////////////////////

    virtual void on_data_available(
            fastdds::dds::DataReader* reader);

protected:

    /////////////////////////
    // PROTECTED CTOR
    /////////////////////////

    /**
     * @brief Construct a new CommonReader object
     *
     * It receives all the attributes and QoS needed to create the internal entities.
     *
     * @note Only protected so only concrete classes are instantiated.
     *
     * @throw \c InitializationException in case any creation has failed
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    CommonReader(
            const core::types::ParticipantId& participant_id,
            const core::types::DdsTopic& topic,
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            fastdds::dds::DomainParticipant* participant,
            fastdds::dds::Topic* topic_entity);

    // Specific enable/disable do not need to be implemented

    /////////////////////////
    // IREADER OVERRIDE METHODS
    /////////////////////////

    /**
     * @brief Take specific method
     *
     * Check if there are messages to take.
     * Take next Untaken Change.
     * Set \c data with the message taken (data payload must be stored from PayloadPool).
     * Remove this change from CommonReader History and release.
     *
     * @note guard by mutex \c rtps_mutex_
     *
     * @param data : oldest data to take
     * @return \c RETCODE_OK if data has been correctly taken
     * @return \c RETCODE_NO_DATA if there is no data to send
     * @return \c RETCODE_ERROR if there has been an error reading the data or the data read is corrupted
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual utils::ReturnCode take_nts_(
            std::unique_ptr<core::IRoutingData>& data) noexcept override;

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void enable_nts_() noexcept override;

    /////////////////////////
    // INTERNAL METHODS
    /////////////////////////

    virtual
    fastdds::dds::SubscriberQos
    reckon_subscriber_qos_() const;

    virtual
    fastdds::dds::DataReaderQos
    reckon_reader_qos_() const;

    //! Whether a sample received should be processed
    virtual bool should_accept_sample_(
            const fastdds::dds::SampleInfo& info) noexcept;

    virtual void fill_received_data_(
            const fastdds::dds::SampleInfo& info,
            core::types::RtpsPayloadData& data_to_fill) const noexcept;


    /////////////////////////
    // EXTERNAL METHODS
    /////////////////////////

    fastdds::dds::DomainParticipant* dds_participant_;
    fastdds::dds::Topic* dds_topic_;

    /////////////////////////
    // INTERNAL VARIABLES
    /////////////////////////

    const std::shared_ptr<core::PayloadPool>& payload_pool_;

    core::types::DdsTopic topic_;

    fastdds::dds::Subscriber* dds_subscriber_;
    fastdds::dds::DataReader* reader_;
};

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
