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

#include <cpp_utils/ReturnCode.hpp>

#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/rtps/common/InstanceHandle.h>
#include <fastdds/rtps/common/SerializedPayload.h>
#include <fastdds/rtps/history/IPayloadPool.h>

#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>
#include <ddspipe_core/types/data/RtpsPayloadData.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * This class works as a mediator between the FastPayloadPool, the TopicDataType, and the DDS DataWriter write
 * function. Its purpose is to save a copy between the DataWriter and the FastPayloadPool.
 */
class PayloadPoolMediator : public fastrtps::rtps::IPayloadPool
{
public:

    /**
     * TODO
     */
    PayloadPoolMediator(
            const std::shared_ptr<PayloadPool>& payload_pool);

    /**
     * TODO
     */
    bool write(
            fastdds::dds::DataWriter* writer,
            types::RtpsPayloadData* data);

    /**
     * TODO
     */
    bool write(
            fastdds::dds::DataWriter* writer,
            types::RtpsPayloadData* data,
            fastrtps::rtps::WriteParams& params);

    /**
     * TODO
     */
    utils::ReturnCode write(
            fastdds::dds::DataWriter* writer,
            types::RtpsPayloadData* data,
            const fastrtps::rtps::InstanceHandle_t& handle);

    /**
     * TODO
     */
    DDSPIPE_CORE_DllAPI
    virtual bool get_payload(
            uint32_t size,
            fastrtps::rtps::CacheChange_t& cache_change) override;

    /**
     * TODO
     */
    DDSPIPE_CORE_DllAPI
    virtual bool get_payload(
            fastrtps::rtps::SerializedPayload_t& data,
            fastrtps::rtps::IPayloadPool*& data_owner,
            fastrtps::rtps::CacheChange_t& cache_change) override;

    /**
     * TODO
     */
    DDSPIPE_CORE_DllAPI
    virtual bool release_payload(
            fastrtps::rtps::CacheChange_t& cache_change) override;

    /**
     * Instead of reserving a new payload, since we know the payload and its owner,
     * we redirect it to the other FastPayloadPool function.
     *
     * @param size size of the new chunk of data
     * @param payload object to store the new data
     *
     * @return true if everything OK
     * @return false if something went wrong
     */
    DDSPIPE_CORE_DllAPI
    virtual bool get_payload(
            uint32_t size,
            types::Payload& payload);

    /**
     * TODO
     */
    DDSPIPE_CORE_DllAPI
    virtual bool get_payload(
            const types::Payload& src_payload,
            fastrtps::rtps::IPayloadPool*& data_owner,
            types::Payload& dst_payload);

    /**
     * TODO
     */
    DDSPIPE_CORE_DllAPI
    virtual bool release_payload(
            types::Payload& payload);

protected:

    //! Mutex to prevent simultaneous calls to write
    std::mutex mutex_;

    //! A pointer to the data_ we are trying to write with the write operation.
    types::RtpsPayloadData* data_;

    //! TODO
    const std::shared_ptr<PayloadPool>& payload_pool_;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
