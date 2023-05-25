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

#include <fastdds/dds/topic/TopicDataType.hpp>

#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace dds {

using DataType = fastrtps::rtps::SerializedPayload_t;

class TopicDataType : public eprosima::fastdds::dds::TopicDataType
{
public:

    //! Default constructor
    TopicDataType(
        const std::string& type_name,
        const bool keyed,
        const std::shared_ptr<core::PayloadPool>& payload_pool);

    //! Default destructor
    virtual ~TopicDataType();

    //! \c serialize method overriden from \c TopicDataType
    virtual bool serialize(
            void* data,
            eprosima::fastrtps::rtps::SerializedPayload_t* payload) override;

    //! \c deserialize method overriden from \c TopicDataType
    virtual bool deserialize(
            eprosima::fastrtps::rtps::SerializedPayload_t* payload,
            void* data) override;

    //! \c getSerializedSizeProvider method overriden from \c TopicDataType
    virtual std::function<uint32_t()> getSerializedSizeProvider(
            void* data) override;

    //! \c getKey method overriden from \c TopicDataType
    virtual bool getKey(
            void* data,
            eprosima::fastrtps::rtps::InstanceHandle_t* handle,
            bool force_md5 = false) override;

    //! \c createData method overriden from \c TopicDataType
    virtual void* createData() override;

    //! \c deleteData method overriden from \c TopicDataType
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
