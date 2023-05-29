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

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>

#include <ddspipe_core/interface/IParticipant.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/dynamic/DiscoveryDatabase.hpp>

#include <ddspipe_participants/configuration/SimpleParticipantConfiguration.hpp>
#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/writer/auxiliar/InternalWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

/**
 * TODO comment
 */
class DdsCommonParticipant : public core::IParticipant, public eprosima::fastdds::dds::DomainParticipantListener
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual ~DdsCommonParticipant();

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual core::types::ParticipantId id() const noexcept override;

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool is_rtps_kind() const noexcept override;

    // NOTE: This should not be initialized here, but it is just for simplicity and less code in childs
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool is_repeater() const noexcept override;

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void init();

protected:

    // TODO
    DDSPIPE_PARTICIPANTS_DllAPI
    DdsCommonParticipant(
            const std::shared_ptr<SimpleParticipantConfiguration>& participant_configuration);

    eprosima::fastdds::dds::DomainParticipant* dds_participant_;
    eprosima::fastdds::dds::Publisher* dds_publisher_;
    eprosima::fastdds::dds::Subscriber* dds_subscriber_;

    core::types::ParticipantId id_;

    std::shared_ptr<SimpleParticipantConfiguration> participant_configuration_;
};

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
