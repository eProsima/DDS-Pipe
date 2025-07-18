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

#include <atomic>

#include <cpp_utils/pool/IPool.hpp>
#include <cpp_utils/ReturnCode.hpp>
#include <cpp_utils/time/time_utils.hpp>

#include <fastdds/dds/publisher/qos/WriterQos.hpp>
#include <fastdds/rtps/attributes/HistoryAttributes.hpp>
#include <fastdds/rtps/attributes/WriterAttributes.hpp>
#include <fastdds/rtps/builtin/data/TopicDescription.hpp>
#include <fastdds/rtps/common/CacheChange.hpp>
#include <fastdds/rtps/common/WriteParams.hpp>
#include <fastdds/rtps/history/WriterHistory.hpp>
#include <fastdds/rtps/participant/RTPSParticipant.hpp>
#include <fastdds/rtps/writer/RTPSWriter.hpp>
#include <fastdds/rtps/writer/WriterListener.hpp>

#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>
#include <ddspipe_core/interface/IRoutingData.hpp>
#include <ddspipe_core/types/data/RtpsPayloadData.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/writer/auxiliar/BaseWriter.hpp>

/////
// Forward declarations
namespace eprosima {

namespace fastdds {
namespace rtps {

class IReaderDataFilter;

} /* namespace rtps */
} /* namespace fastdds */
} /* namespace eprosima */

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace rtps {

/**
 * Abstract generic class for a RTPS Writer wrapper.
 *
 * It implements the WriterListener for itself with \c on_writer_matched callbacks.
 *
 * @warning This object is not RAII and must be initialized before used.
 */
class CommonWriter : public BaseWriter, public fastdds::rtps::WriterListener
{
public:

    /////////////////////////
    // CONSTRUCTOR
    /////////////////////////

    // Protected ctor to make class abstract (only built by their childs).

    /**
     * @brief Destroy the Writer object
     *
     * Delete the RTPS Writer and Writer History in case they are set.
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

    /////////////////////////
    // RTPS LISTENER METHODS
    /////////////////////////

    /**
     * @brief CommonWriter Listener callback when a new Reader is matched or unmatched
     *
     * This method is called every time a new Reader is matched or unmatched from this CommonWriter.
     * It only creates a log for matching and unmatching (in case it is not a reader from this same Participant)
     *
     * @param [in] info information about the matched Reader
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    void on_writer_matched(
            fastdds::rtps::RTPSWriter*,
            const fastdds::rtps::MatchingInfo& info) noexcept override;

    /**
     * @brief CommonWriter Listener callback when all the Readers have received a change.
     *
     * This method is called when all the Readers subscribed to a Topic acknowledge that they have received a change.
     * It removes the change from the Writer's history if the Writer is best-effort or volatile.
     *
     * @param [in] ch the change that has been acknowledged by all the Readers.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    void on_writer_change_received_by_all(
            fastdds::rtps::RTPSWriter*,
            fastdds::rtps::CacheChange_t* change) override;

    /**
     * This method is called when a new Reader is discovered, with a Topic that
     * matches that of a local writer, but with a requested QoS that is incompatible
     * with the one offered by the local writer
     * @param qos A mask with the bits of all incompatible Qos activated.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    void on_offered_incompatible_qos(
            fastdds::rtps::RTPSWriter*,
            eprosima::fastdds::dds::PolicyMask qos) noexcept override;

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
            fastdds::rtps::RTPSParticipant* rtps_participant,
            const bool repeater,
            const fastdds::rtps::HistoryAttributes& history_attributes,
            const fastdds::rtps::WriterAttributes& writer_attributes,
            const fastdds::rtps::TopicDescription& topic_description,
            const fastdds::dds::WriterQos& writer_qos,
            const utils::PoolConfiguration& pool_configuration);

    // Specific enable/disable do not need to be implemented

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

    /**
     * @brief Auxiliary method used in \c write to fill the cache change to send and write params.
     *
     * @param [out] to_send_change_to_fill cache change to be filled and sent.
     * @param [out] to_send_params write params to be filled and sent.
     * @param [in] data data received that must be sent.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual utils::ReturnCode fill_to_send_data_(
            fastdds::rtps::CacheChange_t* to_send_change_to_fill,
            eprosima::fastdds::rtps::WriteParams& to_send_params,
            const core::types::RtpsPayloadData& data) const noexcept;

    /**
     * @brief Auxiliary method used after \c write to fill data value.
     *
     * @param [in] to_send_params write params of the cache change sent.
     * @param [out] data data to be fulfilled with params.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void fill_sent_data_(
            const eprosima::fastdds::rtps::WriteParams& sent_params,
            core::types::RtpsPayloadData& data_to_fill) const noexcept;

    /////
    // RTPS specific methods

    /**
     * @brief Create \c this->rtps_writer_ internal object
     *
     * @note this method exists because it may be created from child classes, thus the methods may not be yet available
     * to get atts and qos.
     *
     * @param rtps_participant
     */
    void internal_entities_creation_(
            const fastdds::rtps::HistoryAttributes& history_attributes,
            const fastdds::rtps::WriterAttributes& writer_attributes,
            const fastdds::rtps::TopicDescription& topic_description,
            const fastdds::dds::WriterQos& writer_qos,
            const utils::PoolConfiguration& pool_configuration);

    /**
     * @brief History Attributes to create RTPS Writer History
     */
    static fastdds::rtps::HistoryAttributes reckon_history_attributes_(
            const core::types::DdsTopic& topic) noexcept;

    /**
     * @brief Writer Attributes to create RTPS Writer
     */
    static fastdds::rtps::WriterAttributes reckon_writer_attributes_(
            const core::types::DdsTopic& topic) noexcept;

    //! Topic Description to create RTPS Writer
    fastdds::rtps::TopicDescription reckon_topic_description_(
            const core::types::DdsTopic& topic) noexcept;

    //! QoS for RTPS Writer
    static fastdds::dds::WriterQos reckon_writer_qos_(
            const core::types::DdsTopic& topic) noexcept;

    //! Cache Change Pool Configuration
    static utils::PoolConfiguration reckon_cache_change_pool_configuration_(
            const core::types::DdsTopic& topic) noexcept;

    //! Whether a guid references this Participant
    bool come_from_this_participant_(
            const fastdds::rtps::GUID_t guid) const noexcept;

    /////
    // EXTERNAL VARIABLES

    //! RTPS Participant
    fastdds::rtps::RTPSParticipant* rtps_participant_;

    //! Whether it is repeater or not (used for data filters and/or qos)
    bool repeater_;

    /////
    // INTERNAL VARIABLES

    core::types::DdsTopic topic_;

    const std::shared_ptr<core::PayloadPool>& payload_pool_;

    //! RTPS CommonWriter pointer
    fastdds::rtps::RTPSWriter* rtps_writer_;

    //! RTPS CommonWriter History associated to \c rtps_reader_
    fastdds::rtps::WriterHistory* rtps_history_;

    //! Data Filter used to filter cache changes at the RTPSWriter level.
    std::unique_ptr<fastdds::rtps::IReaderDataFilter> data_filter_;

    //! History attributes to create the History for the internal RTPS Writer.
    fastdds::rtps::HistoryAttributes history_attributes_;

    //! Writer attributes to create the internal RTPS Writer.
    fastdds::rtps::WriterAttributes writer_attributes_;

    //! Topic description to create the internal RTPS Writer
    fastdds::rtps::TopicDescription topic_description_;

    //! Writer QoS to create the internal RTPS Writer.
    fastdds::dds::WriterQos writer_qos_;

    //! Pool Configuration to create the internal History.
    utils::PoolConfiguration pool_configuration_;
};

} /* namespace rtps */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
