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

#include <ddspipe_core/interface/IReader.hpp>
#include <ddspipe_core/interface/IWriter.hpp>
#include <ddspipe_core/types/dds/TopicQoS.hpp>
#include <ddspipe_core/types/participant/ParticipantId.hpp>
#include <ddspipe_core/types/topic/Topic.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * Interface that represents a generic Participant as part of a DdsPipe.
 *
 * This class manages the discovery of new remote entities (that do not belong to the pipe).
 * It also works as a factory for Writers and Readers.
 *
 * Every Participant is associated to a \c ParticipantId that uniquely identifies it.
 *
 * @note In order to implement new Participants, create a subclass of this Interface and implement every method.
 */
class IParticipant
{
public:

    /**
     * @brief Virtual dtor to allow inheritance.
     */
    DDSPIPE_CORE_DllAPI
    virtual ~IParticipant() = default;

    //! The Participant's unique identifier.
    DDSPIPE_CORE_DllAPI
    virtual types::ParticipantId id() const noexcept = 0;

    //! Whether the Participant is RTPS.
    DDSPIPE_CORE_DllAPI
    virtual bool is_rtps_kind() const noexcept = 0;

    //! Whether the Participant requires to connect its own readers with its own writers.
    DDSPIPE_CORE_DllAPI
    virtual bool is_repeater() const noexcept = 0;

    //! The Participant's Topic QoS.
    DDSPIPE_CORE_DllAPI
    virtual types::TopicQoS topic_qos() const noexcept = 0;

    /**
     * @brief Return a new Writer
     *
     * Each writer is associated with a \c Bridge with the topic \c topic .
     * This writer will forward messages in this topic.
     *
     * @param [in] topic : Topic that this Writer will work with.
     *
     * @return Writer in this Participant referring this topic
     *
     * @throw \c InitializationException in case the writer creation fails.
     */
    DDSPIPE_CORE_DllAPI
    virtual std::shared_ptr<IWriter> create_writer(
            const ITopic& topic) = 0;

    /**
     * @brief Return a new Reader
     *
     * Each reader is associated with a \c Bridge with the topic \c topic .
     * This reader will receive messages in this topic.
     *
     * @param [in] topic : Topic that this Reader will work with.
     *
     * @return Reader in this Participant referring this topic
     *
     * @throw \c InitializationException in case the reader creation fails.
     */
    DDSPIPE_CORE_DllAPI
    virtual std::shared_ptr<IReader> create_reader(
            const ITopic& topic) = 0;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
