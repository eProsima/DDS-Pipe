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
//

#pragma once

#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/monitoring/consumers/IMonitorConsumer.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * The \c LogMonitorConsumer consumes data by printing it on \c std::cout.
 *
 * @tparam T Type of the data that the \c LogMonitorConsumer publishes.
 */
template <typename T>
class LogMonitorConsumer : public IMonitorConsumer<T>
{
public:

    LogMonitorConsumer() = default;
    virtual ~LogMonitorConsumer() = default;

    /**
     * @brief Consume data by publishing it on \c topic_.
     *
     * @param data Pointer to the data to be consumed.
     */
    void consume(
            const T& data) const override;

    /**
     * @brief Get the name of the consumer.
     *
     * @return Name of the consumer.
     */
    std::string get_name() const override;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima

#include <ddspipe_core/monitoring/consumers/impl/LogMonitorConsumer.ipp>
