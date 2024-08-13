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

#include <atomic>
#include <map>
#include <mutex>

#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/rtps/common/InstanceHandle.hpp>
#include <fastdds/rtps/common/SerializedPayload.hpp>
#include <fastdds/rtps/history/IPayloadPool.hpp>

#include <cpp_utils/ReturnCode.hpp>

#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>
#include <ddspipe_core/types/data/RtpsPayloadData.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * DataWriters always allocate memory for a payload when writing it to the \c PayloadPool. In the DdsPipe, however, the
 * \c payload is already in the \c PayloadPool (saved by the Reader), so we can reuse it to avoid an extra copy.
 *
 * This class works as a mediator between the \c FastPayloadPool and the DataWriter's write functions, to avoid making
 * the extra copy.
 */
class PayloadPoolMediator : public fastdds::rtps::IPayloadPool
{
public:

    /**
     * @brief construct a \c PayloadPoolMediator to mediate for a \c PayloadPool.
     *
     * @param payload_pool the \c PayloadPool to mediate for.
     */
    DDSPIPE_CORE_DllAPI
    PayloadPoolMediator(
            const std::shared_ptr<PayloadPool>& payload_pool);

    /**
     * @brief write \c data with a \c writer but avoiding copying the \c data from and to the same \c PayloadPool.
     *
     * Save the \c data and call the \c write in the \c writer with \c data as an argument.
     * The \c write in the \c writer will call \c get_payload which, instead of saving a new chunk of memory for data it
     * already contains, will return the \c data it had saved at the time of writing.
     *
     * @warning This method locks the \c mutex to avoid retrieving the \c data saved in another call to \c write.
     *
     * Thread safe.
     *
     * @param writer the writer who has to write the \c data.
     * @param data the data to be written by the \c writer.
     *
     * @return ReturnCode::OK if the data is correctly sent or a ReturnCode related to the specific error otherwise.
     */
    DDSPIPE_CORE_DllAPI
    utils::ReturnCode write(
            fastdds::dds::DataWriter* writer,
            types::RtpsPayloadData* data);

    /**
     * @brief write \c data with a \c writer with \c params but avoiding copying the \c data from and to the same
     * \c PayloadPool.
     *
     * Save the \c data and call the \c write in the \c writer with \c data  and \c params as arguments.
     * The \c write in the \c writer will call \c get_payload which, instead of saving a new chunk of memory for data it
     * already contains, will return the \c data it had saved at the time of writing.
     *
     * @warning This method locks the \c mutex to avoid retrieving the \c data saved in another call to \c write.
     *
     * Thread safe.
     *
     * @param writer the writer who has to write the \c data.
     * @param data the data to be written by the \c writer.
     * @param params the data to be written by the \c writer.
     *
     * @return ReturnCode::OK if the data is correctly sent or a ReturnCode related to the specific error otherwise.
     */
    DDSPIPE_CORE_DllAPI
    utils::ReturnCode write(
            fastdds::dds::DataWriter* writer,
            types::RtpsPayloadData* data,
            fastdds::rtps::WriteParams& params);

    /**
     * @brief write \c data with a \c writer with a \c handle but avoiding copying the \c data from and to the same
     * \c PayloadPool.
     *
     * Save the \c data and call the \c write in the \c writer with \c data  and \c params as arguments.
     * The \c write in the \c writer will call \c get_payload which, instead of saving a new chunk of memory for data it
     * already contains, will return the \c data it had saved at the time of writing.
     *
     * @warning This method locks the \c mutex to avoid retrieving the \c data saved in another call to \c write.
     *
     * Thread safe.
     *
     * @param writer the writer who has to write the \c data.
     * @param data the data to be written by the \c writer.
     * @param handle the data to be used by the \c writer.
     *
     * @return ReturnCode::PRECONDITION_NOT_MET if the handle introduced does not match with the one associated to the
     * data, ReturnCode::OK if the data is correctly sent and ReturnCode::ERROR otherwise.
     */
    DDSPIPE_CORE_DllAPI
    utils::ReturnCode write(
            fastdds::dds::DataWriter* writer,
            types::RtpsPayloadData* data,
            const fastdds::rtps::InstanceHandle_t& handle);

    /**
     * Instead of reserving a block of memory of \c size in the \c payload_pool, we can redirect the call to
     * \c get_payload providing the \c payload (that we saved in the call to \c write) and the \c payload_pool.
     *
     * @param size size of the new chunk of data to allocate in the \c payload_pool.
     * @param cache_change object to store the new data in.
     *
     * @return true if everything OK
     * @return false if something went wrong
     */
    DDSPIPE_CORE_DllAPI
    virtual bool get_payload(
            uint32_t size,
            eprosima::fastdds::rtps::SerializedPayload_t& payload) override;

    /**
     * @brief redirect the call to the \c release_payload in the \c payload_pool.
     *
     * @param data the data to get from the \c PayloadPool.
     * @param data_owner the \c PayloadPool to get the \c data from.
     * @param cache_change object to store the \c data in.
     *
     * @return true if everything OK
     * @return false if something went wrong
     */
    DDSPIPE_CORE_DllAPI
    virtual bool get_payload(
            const eprosima::fastdds::rtps::SerializedPayload_t& src_payload,
            eprosima::fastdds::rtps::SerializedPayload_t& target_payload) override;

    /**
     * @brief redirect the call to the \c release_payload in the \c payload_pool.
     * @param cache_change object to release.
     *
     * @return true if everything OK
     * @return false if something went wrong
     */
    DDSPIPE_CORE_DllAPI
    virtual bool release_payload(
            eprosima::fastdds::rtps::SerializedPayload_t& payload) override;

protected:

    //! Mutex to prevent simultaneous calls to write
    std::mutex mutex_;

    //! A pointer to the payload_ we want to write down in the PayloadPool.
    eprosima::fastdds::rtps::SerializedPayload_t* payload_;

    //! The \c PayloadPool the \c PayloadPoolMediator is mediating for.
    const std::shared_ptr<PayloadPool>& payload_pool_;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
