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
    // Lock the mutex_ to ensure that the payload hasn't changed when we retrieve it in get_payload.
    std::lock_guard<std::mutex> lock(mutex_);

    payload_ = &data->payload;

    return writer->write(data);
}

bool PayloadPoolMediator::write(
        fastdds::dds::DataWriter* writer,
        types::RtpsPayloadData* data,
        fastrtps::rtps::WriteParams& params)
{
    // Lock the mutex_ to ensure that the payload hasn't changed when we retrieve it in get_payload.
    std::lock_guard<std::mutex> lock(mutex_);

    payload_ = &data->payload;

    return writer->write(data, params);
}

utils::ReturnCode PayloadPoolMediator::write(
        fastdds::dds::DataWriter* writer,
        types::RtpsPayloadData* data,
        const fastrtps::rtps::InstanceHandle_t& handle)
{
    // Lock the mutex_ to ensure that the payload hasn't changed when we retrieve it in get_payload.
    std::lock_guard<std::mutex> lock(mutex_);

    payload_ = &data->payload;

    return writer->write(data, handle);
}

bool PayloadPoolMediator::get_payload(
        uint32_t size,
        fastrtps::rtps::CacheChange_t& cache_change)
{
    fastrtps::rtps::IPayloadPool* payload_owner{payload_pool_.get()};

    return get_payload(*payload_, payload_owner, cache_change);
}

bool PayloadPoolMediator::get_payload(
        fastrtps::rtps::SerializedPayload_t& data,
        fastrtps::rtps::IPayloadPool*& data_owner,
        fastrtps::rtps::CacheChange_t& cache_change)
{
    return payload_pool_->get_payload(data, data_owner, cache_change);
}

bool PayloadPoolMediator::release_payload(
        fastrtps::rtps::CacheChange_t& cache_change)
{
    return payload_pool_->release_payload(cache_change);
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
