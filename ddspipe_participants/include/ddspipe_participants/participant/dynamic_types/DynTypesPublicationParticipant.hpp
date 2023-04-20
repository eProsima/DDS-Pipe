// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <mutex>
#include <map>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>

#include <ddspipe_core/interface/IParticipant.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

#include <ddspipe_participants/configuration/SimpleParticipantConfiguration.hpp>
#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/writer/auxiliar/InternalWriter.hpp>
#include <ddspipe_participants/participant/dds/DdsCommonParticipant.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * TODO comment
 */
class DynTypesPublicationParticipant : public dds::DdsCommonParticipant
{
public:

    // TODO
    DDSPIPE_PARTICIPANTS_DllAPI
    DynTypesPublicationParticipant(
            const std::shared_ptr<SimpleParticipantConfiguration>& participant_configuration,
            const std::shared_ptr<core::DiscoveryDatabase>& discovery_database);

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual ~DynTypesPublicationParticipant();

    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<core::IWriter> create_writer(
            const core::ITopic& topic) override;

    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<core::IReader> create_reader(
            const core::ITopic& topic) override;

protected:

    DDSPIPE_PARTICIPANTS_DllAPI
    utils::ReturnCode receive_type_object_(
            eprosima::fastrtps::types::DynamicType_ptr dynamic_type);

    DDSPIPE_PARTICIPANTS_DllAPI
    utils::ReturnCode receive_type_object_(
            ddspipe::core::IRoutingData& data);

    DDSPIPE_PARTICIPANTS_DllAPI
    void create_empty_datawriter_nts_(
        const core::types::DdsTopic& topic);

    static
    DDSPIPE_PARTICIPANTS_DllAPI
    fastdds::dds::DataWriterQos
    default_empty_datawriter_qos_(
        const core::types::DdsTopic& topic) noexcept;

    static
    DDSPIPE_PARTICIPANTS_DllAPI
    fastdds::dds::TopicQos
    default_topic_qos_(
        const core::types::DdsTopic& topic) noexcept;

    std::map<
        core::types::DdsTopic,
        std::pair<
            fastdds::dds::Topic*,
            fastdds::dds::DataWriter*>> writers_;

    std::map<std::string, eprosima::fastrtps::types::DynamicType_ptr> types_discovered_;

    //! Type Object Internal Writer
    std::shared_ptr<InternalWriter> type_object_writer_;

    std::mutex mutex_;
};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
