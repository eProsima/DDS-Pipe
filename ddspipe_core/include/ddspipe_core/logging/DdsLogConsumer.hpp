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
#include <cpp_utils/logging/CustomStdLogConsumer.hpp>
#include <cpp_utils/logging/LogConfiguration.hpp>

#include <fastdds/dds/publisher/DataWriter.hpp>

#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/types/logging/LogEntry.h>
#include <ddspipe_core/types/logging/LogEntryPubSubTypes.h>
#include <ddspipe_core/types/logging/LogEntryTypeObject.h>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * DDS Log Consumer with Standard (logical) behaviour.
 *
 * This consumer publishes every log entry whose kind is higher or equal to a given one and whose content or category
 * match a regex filter.
 *
 * @attention The consumer filters the entries that it receives, but entries could be filtered beforehand by Fast DDS's
 * Log. To avoid this, set the verbosity to Info and do not apply a filter.
 */
class DdsLogConsumer : public utils::CustomStdLogConsumer
{
public:

    //! Create a new \c DdsLogConsumer from a \c LogConfiguration .
    DDSPIPE_CORE_DllAPI
    DdsLogConsumer(
            const utils::LogConfiguration& log_configuration);

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

    //! DataWriter to send log entries
    fastdds::dds::DataWriter* writer_;
};

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
