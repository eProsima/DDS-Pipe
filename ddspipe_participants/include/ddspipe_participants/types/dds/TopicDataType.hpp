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

#include <fastdds/dds/core/ReturnCode.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/topic/TopicDataType.hpp>
#include <fastdds/dds/xtypes/type_representation/detail/dds_xtypes_typeobject.hpp>
#include <fastdds/rtps/common/SerializedPayload.h>

#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>
#include <ddspipe_core/types/data/RtpsPayloadData.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

#include <ddspipe_participants/library/library_dll.h>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

using DataType = core::types::RtpsPayloadData;

class TopicDataType : public eprosima::fastdds::dds::TopicDataType
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
            const void* data,
            eprosima::fastdds::rtps::SerializedPayload_t* payload) override;

    //! \c deserialize method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool deserialize(
            eprosima::fastdds::rtps::SerializedPayload_t* payload,
            void* data) override;

    //! \c getSerializedSizeProvider method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual std::function<uint32_t()> getSerializedSizeProvider(
            const void* data) override;

    //! \c getKey method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual bool getKey(
            const void* const data,
            eprosima::fastdds::rtps::InstanceHandle_t* handle,
            bool force_md5 = false) override;

    //! \c createData method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void* createData() override;

    //! \c deleteData method overriden from \c TopicDataType
    DDSPIPE_PARTICIPANTS_DllAPI
    virtual void deleteData(
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
