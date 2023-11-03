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
 * @file PayloadPoolMediator.cpp
 *
 */

#include <cpp_utils/exception/UnsupportedException.hpp>
#include <cpp_utils/exception/InconsistencyException.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/efficiency/payload/PayloadPoolMediator.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

PayloadPoolMediator::PayloadPoolMediator(
        const std::shared_ptr<PayloadPool>& payload_pool)
    : payload_pool_(payload_pool)
{
}

bool PayloadPoolMediator::write(
        fastdds::dds::DataWriter* writer,
        types::RtpsPayloadData* data)
{
    // Lock the mutex_ to make sure that when we get the PayloadPool get_payload call,
    // we haven't replaced the data.
    std::lock_guard<std::mutex> lock(mutex_);

    data_ = data;

    return writer->write(data);
}

bool PayloadPoolMediator::write(
        fastdds::dds::DataWriter* writer,
        types::RtpsPayloadData* data,
        fastrtps::rtps::WriteParams& params)
{
    // Lock the mutex_ to make sure that when we get the PayloadPool get_payload call,
    // we haven't replaced the data.
    std::lock_guard<std::mutex> lock(mutex_);

    data_ = data;

    return writer->write(data, params);
}

utils::ReturnCode PayloadPoolMediator::write(
        fastdds::dds::DataWriter* writer,
        types::RtpsPayloadData* data,
        const fastrtps::rtps::InstanceHandle_t& handle)
{
    // Lock the mutex_ to make sure that when we get the PayloadPool get_payload call,
    // we haven't replaced the data.
    std::lock_guard<std::mutex> lock(mutex_);

    data_ = data;

    return writer->write(data, handle);
}

bool PayloadPoolMediator::get_payload(
        uint32_t size,
        fastrtps::rtps::CacheChange_t& cache_change)
{
    fastrtps::rtps::IPayloadPool* payload_owner =
            dynamic_cast<IPayloadPool*>(payload_pool_.get());

    if (get_payload(data_->payload, payload_owner, cache_change))
    {
        // Replace the pointer to destroy the data.
        data_ = nullptr;

        cache_change.payload_owner(payload_pool_.get());
        return true;
    }
    else
    {
        logDevError(DDSPIPE_PAYLOADPOOL, "Error occurred while creating payload.")
        return false;
    }
}

bool PayloadPoolMediator::get_payload(
        fastrtps::rtps::SerializedPayload_t& data,
        fastrtps::rtps::IPayloadPool*& data_owner,
        fastrtps::rtps::CacheChange_t& cache_change)
{
    if (payload_pool_->get_payload(data, data_owner, cache_change.serializedPayload))
    {
        cache_change.payload_owner(payload_pool_.get());
        return true;
    }
    else
    {
        logDevError(DDSPIPE_PAYLOADPOOL, "Error occurred while referencing payload.")
        return false;
    }
}

bool PayloadPoolMediator::release_payload(
        fastrtps::rtps::CacheChange_t& cache_change)
{
    if (cache_change.payload_owner() == payload_pool_.get())
    {
        if (payload_pool_->release_payload(cache_change.serializedPayload))
        {
            cache_change.payload_owner(nullptr);
            return true;
        }
        else
        {
            logDevError(DDSPIPE_PAYLOADPOOL, "Error occurred while releasing payload.")
            return false;
        }
    }
    else
    {
        logError(DDSPIPE_PAYLOADPOOL, "Trying to remove a cache change in an incorrect pool.")
        throw utils::InconsistencyException("Trying to remove a cache change in an incorrect pool.");
    }
}

bool PayloadPoolMediator::get_payload(
        uint32_t size,
        types::Payload& payload)
{
    fastrtps::rtps::IPayloadPool* payload_owner =
            dynamic_cast<IPayloadPool*>(payload_pool_.get());

    const bool got_payload_successfully = get_payload(data_->payload, payload_owner, payload);

    // Replace the pointer to destroy the data.
    data_ = nullptr;

    return got_payload_successfully;
}

bool PayloadPoolMediator::get_payload(
        const types::Payload& src_payload,
        fastrtps::rtps::IPayloadPool*& data_owner,
        types::Payload& dst_payload)
{
    return payload_pool_->get_payload(src_payload, data_owner, dst_payload);
}

bool PayloadPoolMediator::release_payload(
        types::Payload& payload)
{
    return payload_pool_->release_payload(payload);
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
