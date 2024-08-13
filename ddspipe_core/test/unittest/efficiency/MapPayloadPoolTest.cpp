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

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>

#include <fastdds/rtps/common/CacheChange.hpp>

#include <cpp_utils/testing/LogChecker.hpp>
#include <cpp_utils/exception/InconsistencyException.hpp>

#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>
#include <ddspipe_core/efficiency/payload/MapPayloadPool.hpp>

using namespace eprosima::ddspipe;
using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;

const constexpr unsigned int TEST_NUMBER = 5;
const constexpr size_t DEFAULT_SIZE = sizeof(PayloadUnit);

namespace eprosima {
namespace ddspipe {
namespace core {
namespace test {

/**
 * @brief Mock over MapPayloadPool implementing public access to private variables.
 *
 */
class MockMapPayloadPool : public MapPayloadPool
{
public:

    using MapPayloadPool::MapPayloadPool;

    uint64_t pointers_stored()
    {
        return reserved_payloads_.size();
    }

    uint64_t reference_count(
            const types::Payload& payload)
    {
        return reserved_payloads_[payload.data];
    }

    void clean_all(
            std::vector<types::Payload>& payloads)
    {
        for (types::Payload& payload : payloads)
        {
            release_payload(payload);
        }
    }

};

} /* namespace test */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */

/*
 * This tests does not check the methods calling cacheChange, this is tested in generic PayloadPool test.
 */

/**
 * Test get_payload method for new changes
 *
 * CASES:
 *  Get N different pointers
 *  fail reserve memory
 */
TEST(MapPayloadPoolTest, get_payload)
{
    // Get N different pointers
    {
        test::MockMapPayloadPool pool;
        std::vector<eprosima::ddspipe::core::types::Payload> payloads(TEST_NUMBER);

        for (unsigned int i = 0; i < TEST_NUMBER; i++)
        {
            pool.get_payload(DEFAULT_SIZE, payloads[i]);

            ASSERT_EQ(payloads[i].max_size, DEFAULT_SIZE);
            ASSERT_EQ(pool.pointers_stored(), i + 1);
            ASSERT_EQ(pool.reference_count(payloads[i]), 1u);
        }

        // END : Clean all remaining payloads
        pool.clean_all(payloads);
    }

    // fail reserve memory
    {
        test::MockMapPayloadPool pool;
        eprosima::ddspipe::core::types::Payload payload;

        ASSERT_FALSE(pool.get_payload(0, payload));
    }
}

/**
 * Check to get_payload from a source that has been created in same pool increase references.
 *
 * STEPS:
 *  get payload0
 *  get payload1 from src payload0
 *  get payload2 from src payload1
 *  release payload0
 *  get payload3 from src payload1
 *  get payload4
 *  get payload5 from src payload4
 *  release all
 */
TEST(MapPayloadPoolTest, get_payload_from_src)
{
    eprosima::fastdds::rtps::IPayloadPool* pool = new test::MockMapPayloadPool();
    test::MockMapPayloadPool* pool_ = static_cast<test::MockMapPayloadPool*>(pool);

    eprosima::ddspipe::core::types::Payload payload0;
    eprosima::ddspipe::core::types::Payload payload1;
    eprosima::ddspipe::core::types::Payload payload2;
    eprosima::ddspipe::core::types::Payload payload3;
    eprosima::ddspipe::core::types::Payload payload4;
    eprosima::ddspipe::core::types::Payload payload5;

    // get payload0
    ASSERT_TRUE(pool_->get_payload(DEFAULT_SIZE, payload0));
    ASSERT_EQ(pool_->pointers_stored(), 1u);
    ASSERT_EQ(pool_->reference_count(payload0), 1u);

    // get payload1 from src payload0
    ASSERT_TRUE(pool_->get_payload(payload0, payload1));
    ASSERT_EQ(pool_->pointers_stored(), 1u);
    ASSERT_EQ(pool_->reference_count(payload1), 2u);
    ASSERT_EQ(payload1.max_size, payload0.max_size);
    ASSERT_EQ(payload1.data, payload0.data);

    // get payload2 from src payload1
    ASSERT_TRUE(pool_->get_payload(payload1, payload2));
    ASSERT_EQ(pool_->pointers_stored(), 1u);
    ASSERT_EQ(pool_->reference_count(payload2), 3u);
    ASSERT_EQ(payload2.max_size, payload0.max_size);
    ASSERT_EQ(payload2.data, payload0.data);

    // release payload0
    ASSERT_TRUE(pool_->release_payload(payload0));
    ASSERT_EQ(pool_->pointers_stored(), 1u);
    ASSERT_EQ(pool_->reference_count(payload2), 2u);

    // get payload3 from src payload1
    ASSERT_TRUE(pool_->get_payload(payload1, payload3));
    ASSERT_EQ(pool_->pointers_stored(), 1u);
    ASSERT_EQ(pool_->reference_count(payload3), 3u);
    ASSERT_EQ(payload3.max_size, payload1.max_size);
    ASSERT_EQ(payload3.data, payload1.data);

    // get payload4
    ASSERT_TRUE(pool_->get_payload(DEFAULT_SIZE * 0x100, payload4));
    ASSERT_EQ(pool_->pointers_stored(), 2u);
    ASSERT_EQ(pool_->reference_count(payload1), 3u);
    ASSERT_EQ(pool_->reference_count(payload4), 1u);

    // get payload5 from src payload4
    ASSERT_TRUE(pool_->get_payload(payload4, payload5));
    ASSERT_EQ(pool_->pointers_stored(), 2u);
    ASSERT_EQ(pool_->reference_count(payload1), 3u);
    ASSERT_EQ(pool_->reference_count(payload5), 2u);
    ASSERT_EQ(payload5.max_size, payload4.max_size);
    ASSERT_EQ(payload5.data, payload4.data);

    // release all
    ASSERT_TRUE(pool_->release_payload(payload1));
    ASSERT_TRUE(pool_->release_payload(payload2));
    ASSERT_TRUE(pool_->release_payload(payload3));
    ASSERT_TRUE(pool_->release_payload(payload4));
    ASSERT_TRUE(pool_->release_payload(payload5));

    // Check payload pool is empty
    ASSERT_TRUE(pool_->is_clean());
    ASSERT_EQ(pool_->pointers_stored(), 0u);

    delete pool;
}

/**
 * Check to get_payload from a source that has been created in a different pool
 *
 * STEPS:
 *  get payload aux from pool aux
 *  get payload from src payload aux
 *  release payload aux from pool aux
 *  release payload
 */
TEST(MapPayloadPoolTest, get_payload_from_src_no_owner)
{
    // Each pool has a IPayloadPool and a MockMapPayloadPool so it can be called to get_payload from source
    // and specific methods from mock
    eprosima::fastdds::rtps::IPayloadPool* pool = new test::MockMapPayloadPool(); // Requires to be ptr to pass it to get_payload
    test::MockMapPayloadPool* pool_ = static_cast<test::MockMapPayloadPool*>(pool);
    eprosima::fastdds::rtps::IPayloadPool* pool_aux = new test::MockMapPayloadPool(); // Requires to be ptr to pass it to get_payload
    test::MockMapPayloadPool* pool_aux_ = static_cast<test::MockMapPayloadPool*>(pool_aux);

    eprosima::ddspipe::core::types::Payload payload_src;
    eprosima::ddspipe::core::types::Payload payload_target;

    // get payload aux from pool aux
    pool_aux_->get_payload(DEFAULT_SIZE, payload_src);
    ASSERT_EQ(pool_aux_->pointers_stored(), 1u);
    ASSERT_EQ(pool_->pointers_stored(), 0u);

    // get payload from src payload aux
    ASSERT_TRUE(pool_->get_payload(payload_src, payload_target));
    ASSERT_EQ(pool_->pointers_stored(), 1u);

    // release payload aux from pool aux
    pool_aux_->release_payload(payload_src);
    ASSERT_EQ(pool_aux_->pointers_stored(), 0u);
    ASSERT_EQ(pool_->pointers_stored(), 1u);

    // release payload
    pool_->release_payload(payload_target);
    ASSERT_EQ(pool_->pointers_stored(), 0u);

    delete pool_aux;
    delete pool;
}

/**
 * Check negative cases for get_payload from source
 *
 * CASES:
 *  Source has size 0 and different owner
 */
TEST(MapPayloadPoolTest, get_payload_from_src_negative)
{
    // Source has size 0 and different owner
    {
        eprosima::fastdds::rtps::IPayloadPool* pool = new test::MockMapPayloadPool(); // Requires to be ptr to pass it to get_payload
        test::MockMapPayloadPool* pool_ = static_cast<test::MockMapPayloadPool*>(pool);
        eprosima::fastdds::rtps::IPayloadPool* pool_aux = nullptr; // nullptr

        eprosima::ddspipe::core::types::Payload payload_src;
        eprosima::ddspipe::core::types::Payload payload_target;

        ASSERT_FALSE(
            pool_->get_payload(
                payload_src,
                payload_target));

        delete pool;
    }
}

/**
 * Get some payloads from pool from src and release each of them separatly checking reference count
 *
 * STEPS:
 *  get first payload
 *  get N-1 payloads from first
 *  release N-2 payloads
 *  get N-2 more payloads from first
 *  release N payloads
 */
TEST(MapPayloadPoolTest, release_payload)
{
    eprosima::fastdds::rtps::IPayloadPool* pool = new test::MockMapPayloadPool(); // Requires to be ptr to pass it to get_payload
    test::MockMapPayloadPool* pool_ = static_cast<test::MockMapPayloadPool*>(pool);
    std::vector<eprosima::ddspipe::core::types::Payload> payloads(TEST_NUMBER);

    // get first payload
    pool_->get_payload(DEFAULT_SIZE, payloads[0]);

    // get N-1 payloads from first
    for (unsigned int i = 1; i < TEST_NUMBER; i++)
    {
        pool_->get_payload(payloads[0], payloads[i]);
        ASSERT_EQ(pool_->reference_count(payloads[0]), i + 1) << i;
    }

    // release N-2 payloads
    for (unsigned int i = 2; i < TEST_NUMBER; i++)
    {
        ASSERT_TRUE(pool_->release_payload(payloads[i]));
        ASSERT_EQ(pool_->reference_count(payloads[0]), TEST_NUMBER + 1 - i) << i;
    }

    // get N-2 more payloads from first
    for (unsigned int i = 2; i < TEST_NUMBER; i++)
    {
        pool_->get_payload(payloads[0], payloads[i]);
        ASSERT_EQ(pool_->reference_count(payloads[0]), i + 1) << i;
    }

    // release N payloads
    for (unsigned int i = 1; i < TEST_NUMBER; i++)
    {
        ASSERT_TRUE(pool_->release_payload(payloads[i]));
        ASSERT_EQ(pool_->reference_count(payloads[0]), TEST_NUMBER - i) << i;
    }
    // Removing last payload because if not the reference count cannot be done
    ASSERT_TRUE(pool_->release_payload(payloads[0]));
    ASSERT_EQ(pool_->pointers_stored(), 0u);

    // Check payload pool is empty
    ASSERT_TRUE(pool_->is_clean());
    ASSERT_EQ(pool_->pointers_stored(), 0u);

    delete pool;
}

/**
 * Check release a payload that has been get from a different payload pool
 */
TEST(MapPayloadPoolTest, release_payload_negative)
{
    // 1 log error expected
    INSTANTIATE_LOG_TESTER(eprosima::utils::Log::Kind::Error, 1, 1);

    test::MockMapPayloadPool pool;
    test::MockMapPayloadPool pool_aux;
    eprosima::ddspipe::core::types::Payload payload;

    pool_aux.get_payload(DEFAULT_SIZE, payload);

    ASSERT_THROW(pool.release_payload(payload), eprosima::utils::InconsistencyException);
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
