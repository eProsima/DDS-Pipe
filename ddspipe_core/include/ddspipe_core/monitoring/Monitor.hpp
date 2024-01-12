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

#include <mutex>

#include <cpp_utils/time/time_utils.hpp>

#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/monitoring/MonitoringData.h>
#include <ddspipe_core/types/monitoring/MonitoringDataPubSubTypes.h>
#include <ddspipe_core/types/participant/ParticipantId.hpp>

// Monitoring API:

// DDSPIPE MONITOR MACROS
//! TODO
#define monitor_msg_rx(topic, participant_id) MONITOR_MSG_RX_IMPL_(topic, participant_id)

namespace eprosima {
namespace ddspipe {
namespace core {

struct MonitoringInfo
{
    utils::Timestamp start_time;
    DdsTopicData data;
};

/**
 * TODO
 */
class Monitor
{
public:

    // Static method to get the instance of the class
    static Monitor& get_instance();

    // TODO
    void msg_received(
        const types::DdsTopic& topic,
        const types::ParticipantId& participant_id);

    // TODO
    MonitoringData save_data();

protected:

    // TODO
    Monitor();

    // TODO
    ~Monitor();

    //! Mutex to prevent simultaneous calls to msg_received
    std::mutex mutex_;

    // TODO
    std::map<types::DdsTopic, std::map<types::ParticipantId, MonitoringInfo>> data_;

    // Fast-DDS attributes to publish the data
    fastdds::dds::DomainParticipant* participant_;
    fastdds::dds::Publisher* publisher_;
    fastdds::dds::Topic* topic_;
    fastdds::dds::DataWriter* writer_;
    fastdds::dds::TypeSupport type_;
};

// The names of variables inside macros must be unique to avoid conflicts with external variables
// TODO: change to the positive case
#if !MONITOR_ENABLED

#define MONITOR_MSG_RX_IMPL_(topic, participant_id) eprosima::ddspipe::core::Monitor::get_instance().msg_received(topic, participant_id)

#else

#define MONITOR_MSG_RX_IMPL_(topic, participant_id)

#endif // ifndef MONITOR

} // namespace core
} // namespace ddspipe
} // namespace eprosima
