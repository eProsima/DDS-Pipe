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
#include <fastdds/dds/topic/TypeSupport.hpp>

#include <ddspipe_core/configuration/MonitorConfiguration.hpp>
#include <ddspipe_core/monitoring/consumers/IMonitorConsumer.hpp>


namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
template <typename T>
class DdsMonitorConsumer : public IMonitorConsumer<T>
{
public:

    // TODO
    DdsMonitorConsumer(
            const MonitorConfiguration& configuration,
            const std::string& topic_name,
            const std::string& type_name,
            fastdds::dds::TypeSupport& type);

    // TODO
    ~DdsMonitorConsumer();

    // TODO
    void consume(const T* data) const override;

protected:

    // Fast-DDS attributes to publish
    fastdds::dds::DomainParticipant* participant_;
    fastdds::dds::Publisher* publisher_;
    fastdds::dds::Topic* topic_;
    fastdds::dds::DataWriter* writer_;
    fastdds::dds::TypeSupport type_;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima

#include <ddspipe_core/monitoring/consumers/impl/DdsMonitorConsumer.ipp>
