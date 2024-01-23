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

#include <ddspipe_core/configuration/MonitorStatusConfiguration.hpp>
#include <ddspipe_core/configuration/MonitorTopicsConfiguration.hpp>
#include <ddspipe_core/monitoring/IMonitorConsumer.hpp>
#include <ddspipe_core/types/monitoring/status/MonitoringStatus.h>
#include <ddspipe_core/types/monitoring/topics/MonitoringData.h>


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
    DdsMonitorConsumer(
            const MonitorStatusConfiguration& status_config,
            const MonitorTopicsConfiguration& topics_config);

    // TODO
    ~DdsMonitorConsumer();

    // TODO
    void consume_status(const MonitoringStatus& data) const override;

    // TODO
    void consume_topics(const MonitoringData& data) const override;

protected:

    // TODO
    fastdds::dds::Topic* create_topic_(const std::string& topic_name, const std::string& type_name);

    // TODO
    fastdds::dds::DataWriter* create_writer_(fastdds::dds::Topic* topic);

    // Fast-DDS attributes to publish
    fastdds::dds::DomainParticipant* participant_;
    fastdds::dds::Publisher* publisher_;

    // Fast-DDS attributes to publish the status
    fastdds::dds::Topic* status_topic_;
    fastdds::dds::DataWriter* status_writer_;
    fastdds::dds::TypeSupport status_type_;

    // Fast-DDS attributes to publish the topics
    fastdds::dds::Topic* topics_topic_;
    fastdds::dds::DataWriter* topics_writer_;
    fastdds::dds::TypeSupport topics_type_;
};

} // namespace core
} // namespace ddspipe
} // namespace eprosima
