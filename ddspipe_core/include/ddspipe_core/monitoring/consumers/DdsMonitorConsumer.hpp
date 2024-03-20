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

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/Topic.hpp>

#include <ddspipe_core/configuration/DdsPublishingConfiguration.hpp>
#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/monitoring/consumers/DdsMonitorParticipantRegistry.hpp>
#include <ddspipe_core/monitoring/consumers/IMonitorConsumer.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * The \c DdsMonitorConsumer consumes data by publishing it on a DDS Topic.
 *
 * @tparam T Type of the data that the \c DdsMonitorConsumer publishes.
 */
template <typename T>
class DdsMonitorConsumer : public IMonitorConsumer<T>
{
public:

    /**
     * @brief Initialize the Fast-DDS entities required to publish data.
     *
     * @param configuration Configuration of the \c DdsMonitorConsumer.
     * @param registry Contains the DDS Participants to avoid creating multiple participants on the same domain.
     * @param type TypeSupport of the data to be published.
     */
    DdsMonitorConsumer(
            const DdsPublishingConfiguration& configuration,
            DdsMonitorParticipantRegistry& registry,
            fastdds::dds::TypeSupport& type);

    /**
     * @brief Destroy the Fast-DDS entities.
     */
    virtual ~DdsMonitorConsumer();

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

protected:

    // Fast-DDS entities to publish data.
    fastdds::dds::DomainParticipant* participant_;
    fastdds::dds::Publisher* publisher_;
    fastdds::dds::Topic* topic_;
    fastdds::dds::DataWriter* writer_;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima

#include <ddspipe_core/monitoring/consumers/impl/DdsMonitorConsumer.ipp>
