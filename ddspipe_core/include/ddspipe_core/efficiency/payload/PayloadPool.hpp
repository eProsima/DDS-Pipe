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

#include <fastdds/rtps/history/IPayloadPool.hpp>

#include <ddspipe_core/types/dds/Payload.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * Pool to store and release payloads.
 *
 * A Payload is an object with a blind pointer to a string of bytes and extra info (size, length, etc.).
 * Ideally, any payload data will be copied just once, and move the reference from one element to another.
 * When a payload data has not more references to it, it will be erased.
 *
 * This Pool will be called to store a new message that a Reader receives (ideally it will be called just one
 * per message received).
 * Then, this payload will be moved to the Track. As the payload is already in the pool, there will be no copy.
 * Finally, the payload will be moved to every Writer that has to send the data (ideally without copy).
 */
class PayloadPool : public eprosima::fastdds::rtps::IPayloadPool
{
public:

    //! Construct an empty PayloadPool
    DDSPIPE_CORE_DllAPI
    PayloadPool();

    //! Delete PayloadPool and erase every Payload still without release
    DDSPIPE_CORE_DllAPI
    virtual ~PayloadPool();

    //! Wether every payload get has been released.
    DDSPIPE_CORE_DllAPI
    virtual bool is_clean() const noexcept;

protected:

    /**
     * @brief Reserve a new space of memory for new data.
     *
     * It increases \c reserve_count_ .
     *
     * @param size size of memory chunk to reserve
     * @param payload object where introduce the new data pointer
     *
     * @return true if everything ok
     * @return false if something went wrong
     */
    DDSPIPE_CORE_DllAPI
    virtual bool reserve_(
            uint32_t size,
            types::Payload& payload);

    /**
     * @brief Free a memory space.
     *
     * It increases \c release_count_ .
     *
     * @param payload object to free the data from
     *
     * @return true if everything ok
     * @return false if something went wrong
     *
     * @throw \c IncosistencyException if more releases than reserves has been done
     */
    DDSPIPE_CORE_DllAPI
    virtual bool release_(
            types::Payload& payload);

    //! Increase \c reserve_count_
    DDSPIPE_CORE_DllAPI
    void add_reserved_payload_();

    //! Increase \c release_count_ . Show a warning if there are more releases than reserves.
    DDSPIPE_CORE_DllAPI
    void add_release_payload_();

    //! Count the number of reserved data from this pool
    std::atomic<uint64_t> reserve_count_;
    //! Count the number of released data from this pool
    std::atomic<uint64_t> release_count_;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
