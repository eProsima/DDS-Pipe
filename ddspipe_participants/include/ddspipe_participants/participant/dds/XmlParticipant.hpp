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

#include <ddspipe_participants/library/library_dll.h>
#include <ddspipe_participants/configuration/XmlParticipantConfiguration.hpp>
#include <ddspipe_participants/participant/dds/CommonParticipant.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

/**
 * Concrete class of a DDS Participant \c CommonParticipant in which the participant is created
 * following a profile name loaded by XML configuration.
 */
class XmlParticipant
    : public CommonParticipant
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    XmlParticipant(
            const std::shared_ptr<XmlParticipantConfiguration>& participant_configuration,
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            const std::shared_ptr<core::DiscoveryDatabase>& discovery_database);

    /**
     * Specialized parent call so if it fails returns a blank one.
     * This is because security may fail to create some endpoints due to insufficient permissions, but show must go on.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<core::IWriter> create_writer(
            const core::ITopic& topic) override;

    /**
     * Specialized parent call so if it fails returns a blank one.
     * This is because security may fail to create some endpoints due to insufficient permissions, but show must go on.
     */
    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<core::IReader> create_reader(
            const core::ITopic& topic) override;

protected:

    /////////////////////////
    // INTERNAL METHODS
    /////////////////////////

    virtual
    fastdds::dds::DomainParticipantQos
    reckon_participant_qos_() const override;

    virtual
    fastdds::dds::DomainParticipant*
    create_dds_participant_() override;

    /////////////////////////
    // INTERNAL VARIABLES
    /////////////////////////

    //! Participant configuration
    const XmlParticipantConfiguration& xml_specific_configuration_;
};

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
