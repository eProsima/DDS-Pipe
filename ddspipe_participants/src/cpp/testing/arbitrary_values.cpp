// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <ddspipe_core/testing/arbitrary_values.hpp>
#include <ddspipe_core/types/dds/GuidPrefix.hpp>

#include <ddspipe_participants/types/address/Address.hpp>
#include <ddspipe_participants/types/address/DiscoveryServerConnectionAddress.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace testing {

using namespace eprosima::ddspipe::participants::types;

template <>
Address arbitrary(
        unsigned int seed /* = 0 */)
{
    // TODO do it more generic and arbitrary
    return Address("127.0.0.1", 10000 + seed, 10000 + seed, TransportProtocol::udp);
}

template <>
DiscoveryServerConnectionAddress arbitrary(
        unsigned int seed /* = 0 */)
{
    DiscoveryServerConnectionAddress object;

    object.discovery_server_guid_prefix = arbitrary<core::types::GuidPrefix>(seed);

    unsigned int size = seed % 10;

    // TODO do it more generic and arbitrary
    for (unsigned int i = 0; i < size; ++i)
    {
        object.addresses.insert(
            arbitrary<Address>((seed + i)));
    }

    return object;
}

} /* namespace testing */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
