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

#include <ddspipe_core/monitoring/IMonitorConsumer.hpp>
#include <ddspipe_core/types/monitoring/MonitoringData.h>


namespace eprosima {
namespace ddspipe {
namespace core {

/**
 * TODO
 */
class DdsMonitorConsumer : public IMonitorConsumer
{
public:

    // TODO
    DdsMonitorConsumer();

    // TODO
    ~DdsMonitorConsumer();

    // TODO
    virtual void consume(const MonitoringData& data) const override;

protected:

    // Fast-DDS attributes to publish the data
    fastdds::dds::DomainParticipant* participant_;
    fastdds::dds::Publisher* publisher_;
    fastdds::dds::Topic* topic_;
    fastdds::dds::DataWriter* writer_;
    fastdds::dds::TypeSupport type_;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima
