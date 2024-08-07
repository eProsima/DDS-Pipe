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

#include <memory>
#include <string>

#include <fastdds/dds/core/policy/QosPolicies.hpp>
#include <fastdds/dds/core/ReturnCode.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/topic/TopicDataType.hpp>
#include <fastdds/dds/xtypes/type_representation/detail/dds_xtypes_typeobject.hpp>
#include <fastdds/rtps/common/InstanceHandle.hpp>
#include <fastdds/rtps/common/SerializedPayload.hpp>

#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>
#include <ddspipe_core/types/data/RtpsPayloadData.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

#include <ddspipe_participants/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

using DataType = core::types::RtpsPayloadData;

class TopicDataType : public fastdds::dds::TopicDataType
{
public:

    //! Default constructor
    DDSPIPE_PARTICIPANTS_DllAPI
    TopicDataType(
            const std::shared_ptr<core::PayloadPool>& payload_pool,
            const std::string& type_name,
            const fastdds::dds::xtypes::TypeIdentifierPair& type_ids,
            const bool keyed);

    //! Default destructor
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual ~TopicDataType();

    //! \c serialize method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool serialize(
            const void* const data,
            fastdds::rtps::SerializedPayload_t& payload,
            fastdds::dds::DataRepresentationId_t data_representation) override;

    //! \c deserialize method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool deserialize(
            fastdds::rtps::SerializedPayload_t& payload,
            void* data) override;

    //! \c calculate_serialized_size method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual uint32_t calculate_serialized_size(
            const void* const data,
            fastdds::dds::DataRepresentationId_t data_representation) override;

    //! \c compute_key method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool compute_key(
            fastdds::rtps::SerializedPayload_t& payload,
            fastdds::rtps::InstanceHandle_t& handle,
            bool force_md5 = false) override;

    //! \c compute_key method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool compute_key(
            const void* const /*data*/,
            fastdds::rtps::InstanceHandle_t& /*handle*/,
            bool force_md5 = false) override;

    //! \c create_data method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void* create_data() override;

    //! \c delete_data method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void delete_data(
            void* data) override;

protected:

    const std::string type_name_;
    const bool keyed_;

    std::shared_ptr<core::PayloadPool> payload_pool_;

};

} /* namespace dds */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
