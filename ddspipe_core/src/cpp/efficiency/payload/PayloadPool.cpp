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

/**
 * @file PayloadPool.cpp
 *
 */

#include <cpp_utils/exception/InconsistencyException.hpp>
#include <cpp_utils/Log.hpp>

#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

using namespace eprosima::ddspipe::core::types;

PayloadPool::PayloadPool()
    : reserve_count_(0)
    , release_count_(0)
{
}

PayloadPool::~PayloadPool()
{
    if (reserve_count_ < release_count_)
    {
        logDevError(DDSPIPE_PAYLOADPOOL, "Removing non Consistent PayloadPool.");
    }
    else if (reserve_count_ != release_count_)
    {
        logDevError(DDSPIPE_PAYLOADPOOL,
                "From " << reserve_count_ << " payloads reserved only " << release_count_ << " has been released.");
    }
    else
    {
        logInfo(DDSPIPE_PAYLOADPOOL,
                "Removing PayloadPool correctly after reserve: " << reserve_count_ << " payloads.");
    }
}

bool PayloadPool::is_clean() const noexcept
{
    return reserve_count_ == release_count_;
}

/////
// INTERNAL PART

void PayloadPool::add_reserved_payload_()
{
    ++reserve_count_;
}

void PayloadPool::add_release_payload_()
{
    ++release_count_;
    if (release_count_ > reserve_count_)
    {
        logError(DDSPIPE_PAYLOADPOOL,
                "Inconsistent PayloadPool, releasing more payloads than reserved.");
        throw utils::InconsistencyException("Inconsistent PayloadPool, releasing more payloads than reserved.");
    }
}

bool PayloadPool::reserve_(
        uint32_t size,
        eprosima::fastdds::rtps::SerializedPayload_t& payload)
{
    if (size == 0)
    {
        logDevError(DDSPIPE_PAYLOADPOOL,
                "Trying to reserve a data block of 0 bytes.");
        return false;
    }

    payload.reserve(size);

    logDebug(DDSPIPE_PAYLOADPOOL, "Reserved payload ptr: " << payload.data << ".");

    add_reserved_payload_();

    return true;
}

bool PayloadPool::release_(
        eprosima::fastdds::rtps::SerializedPayload_t& payload)
{
    logDebug(DDSPIPE_PAYLOADPOOL, "Releasing payload ptr: " << payload.data << ".");

    payload.payload_owner = nullptr;

    payload.empty();

    if (payload.data != nullptr)
    {
        return false;
    }

    add_release_payload_();

    return true;
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
