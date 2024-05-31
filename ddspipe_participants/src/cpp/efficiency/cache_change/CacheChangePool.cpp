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
// limitations under the License.

#include <efficiency/cache_change/CacheChangePool.hpp>
#include <types/dds/RouterCacheChange.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {

CacheChangePool::CacheChangePool(
        utils::PoolConfiguration configuration)
    : utils::UnboundedPool<fastrtps::rtps::CacheChange_t>(configuration)
{
    initialize_vector_();
}

bool CacheChangePool::reserve_cache(
        fastrtps::rtps::CacheChange_t*& cache_change)
{
    return loan(cache_change);
}

bool CacheChangePool::release_cache(
        fastrtps::rtps::CacheChange_t* cache_change)
{
    return return_loan(cache_change);
}

fastrtps::rtps::CacheChange_t* CacheChangePool::new_element_()
{
    return new types::RouterCacheChange();
}

void CacheChangePool::reset_element_(
        fastrtps::rtps::CacheChange_t* change)
{
    // NOTE: This could be done by =operator but it is deleted, so it must be done field by field
    change->kind = fastrtps::rtps::ALIVE;
    change->sequenceNumber.high = 0;
    change->sequenceNumber.low = 0;
    change->writerGUID = fastrtps::rtps::c_Guid_Unknown;
    change->instanceHandle.clear();
    change->isRead = 0;
    change->sourceTimestamp.seconds(0);
    change->sourceTimestamp.fraction(0);
    change->writer_info.num_sent_submessages = 0;
    change->setFragmentSize(0);
    change->serializedPayload.empty();
    change->inline_qos.empty();
}

} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
