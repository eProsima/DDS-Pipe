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

#include <fastrtps/types/DynamicTypePtr.h>

#include <ddspipe_core/interface/IRoutingData.hpp>
#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/types/topic/TopicInternalTypeDiscriminator.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

struct DynamicTypeData : public core::IRoutingData
{

    DDSPIPE_CORE_DllAPI
    DynamicTypeData() = default;

    DDSPIPE_CORE_DllAPI
    virtual types::TopicInternalTypeDiscriminator internal_type_discriminator() const noexcept override;

    fastrtps::types::DynamicType_ptr dynamic_type{nullptr};
};

DDSPIPE_CORE_DllAPI
DdsTopic type_object_topic();

DDSPIPE_CORE_DllAPI
bool is_type_object_topic(
        const ITopic& topic);

constexpr const char* TYPE_OBJECT_TOPIC_NAME = "__internal__/type_object";
constexpr const char* TYPE_OBJECT_DATA_TYPE_NAME = "__internal__::type_object";

const core::types::TopicInternalTypeDiscriminator INTERNAL_TOPIC_TYPE_DYNAMIC_TYPE = "dynamic::type::v0";

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
