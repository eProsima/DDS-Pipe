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


namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * @brief Interface that represents a generic consumer of the \c Monitor.
 *
 * The Monitor's consumers are the entities that consume the data produced by the Monitor's producers.
 *
 * @tparam T Type of the data that the consumer is going to consume.
 */
template <typename T>
class IMonitorConsumer
{
public:

    IMonitorConsumer() = default;
    virtual ~IMonitorConsumer() = default;

    /**
     * @brief Virtual method that consumes the data.
     *
     * @param data Pointer to the data to be consumed.
     */
    virtual void consume(
            const T& data) const = 0;

    /**
     * @brief Virtual method that returns the name of the consumer.
     *
     * @return Name of the consumer.
     */
    virtual std::string get_name() const = 0;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima
