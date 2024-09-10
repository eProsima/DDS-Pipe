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

utils::ReturnCode PayloadPoolMediator::write(
        fastdds::dds::DataWriter* writer,
        types::RtpsPayloadData* data)
{
    // Lock the mutex_ to ensure that the payload hasn't changed when we retrieve it in get_payload.
    std::lock_guard<std::mutex> lock(mutex_);

    payload_ = &data->payload;

    return writer->write(data);
}

utils::ReturnCode PayloadPoolMediator::write(
        fastdds::dds::DataWriter* writer,
        types::RtpsPayloadData* data,
        fastdds::rtps::WriteParams& params)
{
    // Lock the mutex_ to ensure that the payload hasn't changed when we retrieve it in get_payload.
    std::lock_guard<std::mutex> lock(mutex_);

    payload_ = &data->payload;

    return writer->write(data, params);
}

utils::ReturnCode PayloadPoolMediator::write(
        fastdds::dds::DataWriter* writer,
        types::RtpsPayloadData* data,
        const fastdds::rtps::InstanceHandle_t& handle)
{
    // Lock the mutex_ to ensure that the payload hasn't changed when we retrieve it in get_payload.
    std::lock_guard<std::mutex> lock(mutex_);

    payload_ = &data->payload;

    return writer->write(data, handle);
}

bool PayloadPoolMediator::get_payload(
        uint32_t size,
        eprosima::fastdds::rtps::SerializedPayload_t& payload)
{
    return get_payload(*payload_, payload);
}

bool PayloadPoolMediator::get_payload(
        const eprosima::fastdds::rtps::SerializedPayload_t& src_payload,
        eprosima::fastdds::rtps::SerializedPayload_t& target_payload)
{
    return payload_pool_->get_payload(src_payload, target_payload);
}

bool PayloadPoolMediator::release_payload(
        eprosima::fastdds::rtps::SerializedPayload_t& payload)
{
    return payload_pool_->release_payload(payload);
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
