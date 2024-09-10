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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fastdds/rtps/common/SerializedPayload.hpp>

#include <cpp_utils/testing/LogChecker.hpp>
#include <cpp_utils/exception/InconsistencyException.hpp>

#include <ddspipe_core/types/dds/Payload.hpp>
#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>

using namespace eprosima::ddspipe;
using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;

// Using for gmock
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

namespace eprosima {
namespace fastdds {
namespace rtps {

/*
 * WORKAROUND:
 * This definition is needed due to googletest-distribution (1.11.0) requires to every class used inside ASSERT macro
 * to have the operator << defined in SAME namespace than the class.
 * In our case, Payload is defined as eprosima::fastdds::rtps::SerializedPayload_t but redefined as
 * eprosima::ddspipe::core::types::Payload and the operator << is defined in eprosima::ddspipe::core::types
 * Thus, gtest could not find this definition (arising a very messy and cryptic compilation error).
 * This definition corrects that problem.
 *
 * NOTE:
 * In googletest-distribution release-1.10.0 this does not happen.
 */
void PrintTo(
        const SerializedPayload_t&,
        std::ostream* os)
{
    *os << "::eprosima::fastdds::rtps::SerializedPayload_t";
}

} /* namespace rtps */
} /* namespace fastdds */
} /* namespace eprosima */

namespace eprosima {
namespace ddspipe {
namespace core {
namespace test {

/**
 * @brief Mock over Payload Pool re-implementing the needed methods returning error and
 * implementing public access to private variables.
 *
 */
class MockPayloadPool : public PayloadPool
{
public:

    // Using parent methods and variables as public
    using PayloadPool::PayloadPool;
    using PayloadPool::reserve_;
    using PayloadPool::release_;
    using PayloadPool::reserve_count_;
    using PayloadPool::release_count_;

    // Mock this virtual methods not implemented in parent class
    MOCK_METHOD(
        bool,
        get_payload,
        (uint32_t size, eprosima::fastdds::rtps::SerializedPayload_t& target_payload),
        (override));

    MOCK_METHOD(
        bool,
        get_payload,
        (
            const Payload& src_payload,
            eprosima::fastdds::rtps::SerializedPayload_t& target_payload
        ),
        (override));

    MOCK_METHOD(
        bool,
        release_payload,
        (eprosima::fastdds::rtps::SerializedPayload_t& target_payload),
        (override));
};

} /* namespace test */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */

/**
 * Test reserve_ method
 *
 * CASES:
 *  small size
 *  large size
 *  0 size
 */
TEST(PayloadPoolTest, reserve)
{
    // small size
    {
        test::MockPayloadPool pool;
        Payload payload;

        ASSERT_EQ(payload.max_size, 0u);
        ASSERT_EQ(payload.data, nullptr);

        ASSERT_TRUE(pool.reserve_(sizeof(PayloadUnit), payload));

        ASSERT_EQ(payload.max_size, sizeof(PayloadUnit));
        ASSERT_NE(payload.data, nullptr);

        // This would (maybe) fail with SEG FAULT if the data has not been correctly set
        payload.data[0] = 16u;

        ASSERT_TRUE(pool.release_(payload));
    }

    // large size
    {
        test::MockPayloadPool pool;
        Payload payload;

        ASSERT_EQ(payload.max_size, 0u);
        ASSERT_EQ(payload.data, nullptr);

        ASSERT_TRUE(pool.reserve_(sizeof(PayloadUnit) * 0x1000, payload));

        ASSERT_EQ(payload.max_size, sizeof(PayloadUnit) * 0x1000);
        ASSERT_NE(payload.data, nullptr);

        // This would (maybe) fail with SEG FAULT if the data has not been correctly set
        payload.data[0] = 4u;
        payload.data[0x1000 - 1] = 5u;

        ASSERT_TRUE(pool.release_(payload));
    }
}

/**
 * Test reserve_ method
 *
 * CASES:
 *  0 size
 */
TEST(PayloadPoolTest, reserve_negative)
{

    // 0 size
    {

        test::MockPayloadPool pool;
        Payload payload;

        ASSERT_EQ(payload.max_size, 0u);

        ASSERT_FALSE(pool.reserve_(0, payload));

        ASSERT_EQ(payload.max_size, 0u);
    }
}

/**
 * Test release_ method
 *
 * CASES:
 *  small size
 *  large size
 */
TEST(PayloadPoolTest, release)
{
    // small size
    {
        test::MockPayloadPool pool;
        Payload payload;
        pool.reserve_(sizeof(PayloadUnit), payload);

        ASSERT_EQ(payload.max_size, sizeof(PayloadUnit));
        ASSERT_NE(payload.data, nullptr);

        ASSERT_TRUE(pool.release_(payload));

        ASSERT_EQ(payload.max_size, 0u);
        ASSERT_EQ(payload.data, nullptr);
    }

    // large size
    {
        test::MockPayloadPool pool;
        Payload payload;
        pool.reserve_(sizeof(PayloadUnit) * 0x1000, payload);

        ASSERT_EQ(payload.max_size, sizeof(PayloadUnit) * 0x1000);
        ASSERT_NE(payload.data, nullptr);

        ASSERT_TRUE(pool.release_(payload));

        ASSERT_EQ(payload.max_size, 0u);
        ASSERT_EQ(payload.data, nullptr);
    }
}

/**
 * Test release_ method
 *
 * STEPS:
 *  store 5 values
 *  release 4 values
 *  store 5 more values
 *  release 6 values
 *  release more values than reserved
 */
TEST(PayloadPoolTest, reserve_and_release_counter)
{
    test::MockPayloadPool pool;
    std::vector<Payload> payloads(11);

    // store 5 values
    for (unsigned int i = 0; i < 5u; ++i)
    {
        ASSERT_EQ(pool.reserve_count_, i);
        pool.reserve_(sizeof(PayloadUnit), payloads[i]);
    }
    ASSERT_EQ(pool.reserve_count_, 5u);

    // release 4 values
    for (unsigned int i = 0; i < 4u; ++i)
    {
        ASSERT_EQ(pool.release_count_, i);
        pool.release_(payloads[i]);
    }
    ASSERT_EQ(pool.release_count_, 4u);

    // store 5 values
    for (unsigned int i = 5; i < 10u; ++i)
    {
        ASSERT_EQ(pool.reserve_count_, i);
        pool.reserve_(sizeof(PayloadUnit), payloads[i]);
    }
    ASSERT_EQ(pool.reserve_count_, 10u);

    // release 6 values
    for (unsigned int i = 4; i < 10u; ++i)
    {
        ASSERT_EQ(pool.release_count_, i);
        pool.release_(payloads[i]);
    }
    ASSERT_EQ(pool.release_count_, 10u);
}

/**
 * Test release_ method
 *
 * STEPS:
 *  store 5 values
 *  release 5 values
 *  release more values than reserved
 */
TEST(PayloadPoolTest, reserve_and_release_counter_negative)
{
    // 1 log error expected
    INSTANTIATE_LOG_TESTER(eprosima::utils::Log::Kind::Error, 1, 1);

    test::MockPayloadPool pool;
    std::vector<Payload> payloads(5);

    // store 5 values
    for (unsigned int i = 0; i < 5u; ++i)
    {
        ASSERT_EQ(pool.reserve_count_, i);
        pool.reserve_(sizeof(PayloadUnit), payloads[i]);
    }

    // release 5 values
    for (unsigned int i = 0; i < 5u; ++i)
    {
        ASSERT_EQ(pool.release_count_, i);
        pool.release_(payloads[i]);
    }

    // release more values than reserved
    ASSERT_THROW(pool.release_(payloads[4]), eprosima::utils::InconsistencyException);
}

/**
 * Test clean method
 *
 * STEPS:
 *  start clean
 *  reserve and not clean
 *  release and clean
 */
TEST(PayloadPoolTest, is_clean)
{
    test::MockPayloadPool pool;

    // start clean
    ASSERT_TRUE(pool.is_clean());

    // reserve and not clean
    Payload payload;
    pool.reserve_(sizeof(PayloadUnit), payload);
    ASSERT_FALSE(pool.is_clean());

    // release and clean
    pool.release_(payload);
    ASSERT_TRUE(pool.is_clean());
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
