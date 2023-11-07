// Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
// limitations under the License\.

#pragma once

#include <iostream>
#include <string>

#include <cpp_utils/memory/Heritable.hpp>
#include <cpp_utils/types/Fuzzy.hpp>

#include <ddspipe_core/library/library_dll.h>
#include <ddspipe_core/types/topic/dds/DistributedTopic.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {

/**
 * Data struct that represents a DDS Topic of data flow in the Router.
 */
struct DdsTopic : public DistributedTopic
{

    /////////////////////////
    // CONSTRUCTORS
    /////////////////////////

    //! Default constructor
    DDSPIPE_CORE_DllAPI
    DdsTopic();

    /////////////////////////
    // OPERATORS
    /////////////////////////

    DDSPIPE_CORE_DllAPI
    virtual DdsTopic& operator =(
            const DdsTopic& other) noexcept;

    /////////////////////////
    // METHODS
    /////////////////////////

    DDSPIPE_CORE_DllAPI
    virtual bool is_valid(
            utils::Formatter& error_msg) const noexcept override;

    DDSPIPE_CORE_DllAPI
    virtual std::string topic_unique_name() const noexcept override;

    DDSPIPE_CORE_DllAPI
    virtual std::string serialize() const noexcept override;

    //! Make a copy of the Topic
    DDSPIPE_CORE_DllAPI
    virtual utils::Heritable<ITopic> copy() const noexcept override;

    /////////////////////////
    // STATIC METHODS
    /////////////////////////

    //! Whether a name and type can refer to a correct DDS Topic.
    DDSPIPE_CORE_DllAPI
    static bool is_valid_dds_topic(
            const std::string& topic_name,
            const std::string& type_name,
            utils::Formatter& error_msg) noexcept;

    /////////////////////////
    // VARIABLES
    /////////////////////////

    //! Topic Type name
    std::string type_name{};
};

} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
