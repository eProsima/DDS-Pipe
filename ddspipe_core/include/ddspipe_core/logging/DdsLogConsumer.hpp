// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
 * @file DdsLogConsumer.hpp
 */

#pragma once

#include <cpp_utils/Log.hpp>
#include <cpp_utils/logging/BaseLogConsumer.hpp>

#include <ddspipe_core/configuration/DdsPipeLogConfiguration.hpp>
#include <ddspipe_core/library/library_dll.h>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/Topic.hpp>

#if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)
    #include <ddspipe_core/types/logging/v1/LogEntry.h>
    #include <ddspipe_core/types/logging/v1/LogEntryPubSubTypes.h>
    #include <ddspipe_core/types/logging/v1/LogEntryTypeObject.h>
#else
    #include <ddspipe_core/types/logging/v2/LogEntry.h>
    #include <ddspipe_core/types/logging/v2/LogEntryPubSubTypes.h>
    #include <ddspipe_core/types/logging/v2/LogEntryTypeObject.h>
#endif // if FASTRTPS_VERSION_MAJOR < 2 || (FASTRTPS_VERSION_MAJOR == 2 && FASTRTPS_VERSION_MINOR < 13)

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * DDS Log Consumer with Standard (logical) behaviour.
 *
 * Registering this consumer in Fast DDS's Log publishes the log entries accepted by the BaseLogConsumer.
 */
class DdsLogConsumer : public utils::BaseLogConsumer
{
public:

    //! Create a new \c DdsLogConsumer from a \c DdsPipeLogConfiguration .
    DDSPIPE_CORE_DllAPI
    DdsLogConsumer(
            const DdsPipeLogConfiguration* configuration);

    DDSPIPE_CORE_DllAPI
    ~DdsLogConsumer();

    /**
     * @brief Implements \c LogConsumer \c Consume method.
     *
     * The entry's kind must be higher or equal to the verbosity level \c verbosity_ .
     * The entry's content or category must match the \c filter_ regex.
     *
     * This method will publish the \c entry with DDS.
     *
     * @param entry entry to consume
     */
    DDSPIPE_CORE_DllAPI
    void Consume(
            const utils::Log::Entry& entry) override;

protected:

    //! Convert the \c Log::Kind to the \c LogEntry::Kind
    constexpr Kind get_log_entry_kind_(
            const utils::Log::Kind kind) const noexcept;

    //! Fast-DDS entities for publishing log entries
    fastdds::dds::DomainParticipant* participant_;
    fastdds::dds::Publisher* publisher_;
    fastdds::dds::Topic* topic_;
    fastdds::dds::DataWriter* writer_;

    //! Map relating the pattern string to its corresponding event
    std::map<std::string, long> events_{
        {"SAMPLE_LOST", SAMPLE_LOST},
        {"TOPIC_MISMATCH_TYPE", TOPIC_MISMATCH_TYPE},
        {"TOPIC_MISMATCH_QOS", TOPIC_MISMATCH_QOS}
    };
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
