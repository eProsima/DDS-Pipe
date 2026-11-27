// Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

namespace eprosima {
namespace ddspipe {
namespace participants {

/**
 * How the QoS of the DDS endpoints (DataWriters / DataReaders) are obtained from the XML endpoint profiles.
 */
enum class EndpointQoSMode
{
    //! No XML endpoint profile is looked up: the endpoint QoS come from the Topic QoS.
    XML_DISABLED,
    //! A matching XML profile is used as base, and the Topic QoS (YAML and discovered) override it.
    XML_OVERRIDABLE,
    //! A matching XML profile is used as is.
    XML_STANDALONE,
};

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
