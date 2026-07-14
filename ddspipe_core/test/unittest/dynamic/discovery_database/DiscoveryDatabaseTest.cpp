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

#include <atomic>
#include <cstdint>

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>

#include <cpp_utils/exception/InconsistencyException.hpp>
#include <cpp_utils/time/time_utils.hpp>

#include <ddspipe_core/dynamic/DiscoveryDatabase.hpp>
#include <ddspipe_core/testing/random_values.hpp>

using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::testing;

namespace test {

constexpr uint32_t WAIT_TIME_MS = 50;

std::map<types::Guid, types::Endpoint> get_all_endpoints(
        const DiscoveryDatabase& discovery_database)
{
    return discovery_database.get_endpoints(
        [](
            const types::Endpoint&)
        {
            return true;
        });
}

} // namespace test

/**
 * Test that updating an endpoint as inactive removes it from the discovery database
 *
 * STEPS:
 * - add one active endpoint
 * - verify it is stored in the database
 * - update the same endpoint as inactive
 * - verify it is erased, the erase callback is triggered, and further access fails
 */
TEST(DiscoveryDatabaseTest, inactive_update_erases_endpoint)
{
    DiscoveryDatabase discovery_database;
    discovery_database.start();

    auto endpoint = random_endpoint(5);
    endpoint.active = true;

    std::atomic<uint32_t> updated_callbacks{0};
    std::atomic<uint32_t> erased_callbacks{0};
    std::atomic<bool> erased_same_guid{false};

    discovery_database.add_endpoint_updated_callback(
        [&](
            types::Endpoint)
        {
            ++updated_callbacks;
        });

    discovery_database.add_endpoint_erased_callback(
        [&](
            types::Endpoint erased_endpoint)
        {
            erased_same_guid.store(erased_endpoint.guid == endpoint.guid);
            ++erased_callbacks;
        });

    discovery_database.add_endpoint(endpoint);
    eprosima::utils::sleep_for(test::WAIT_TIME_MS);

    ASSERT_TRUE(discovery_database.endpoint_exists(endpoint.guid));
    ASSERT_EQ(test::get_all_endpoints(discovery_database).size(), 1u);

    endpoint.active = false;
    discovery_database.update_endpoint(endpoint);
    eprosima::utils::sleep_for(test::WAIT_TIME_MS);

    EXPECT_FALSE(discovery_database.endpoint_exists(endpoint.guid));
    EXPECT_TRUE(test::get_all_endpoints(discovery_database).empty());
    EXPECT_EQ(updated_callbacks.load(), 0u);
    EXPECT_EQ(erased_callbacks.load(), 1u);
    EXPECT_TRUE(erased_same_guid.load());
    EXPECT_THROW(discovery_database.get_endpoint(endpoint.guid), eprosima::utils::InconsistencyException);

    discovery_database.stop();
}

/**
 * Test that updating a filtered-only endpoint as inactive clears the filter entry
 *
 * STEPS:
 * - add one filtered endpoint GUID
 * - update the same GUID as inactive
 * - verify the filter entry is removed and no erase callback is triggered
 */
TEST(DiscoveryDatabaseTest, inactive_update_clears_filtered_endpoint)
{
    DiscoveryDatabase discovery_database;
    discovery_database.start();

    auto endpoint = random_endpoint(6);
    endpoint.active = false;

    std::atomic<uint32_t> erased_callbacks{0};

    discovery_database.add_endpoint_erased_callback(
        [&](
            types::Endpoint)
        {
            ++erased_callbacks;
        });

    discovery_database.add_filtered_endpoint(endpoint.guid);
    eprosima::utils::sleep_for(test::WAIT_TIME_MS);

    ASSERT_TRUE(discovery_database.exists_filtered_endpoint(endpoint.guid));
    ASSERT_FALSE(discovery_database.endpoint_exists(endpoint.guid));

    discovery_database.update_endpoint(endpoint);
    eprosima::utils::sleep_for(test::WAIT_TIME_MS);

    EXPECT_FALSE(discovery_database.exists_filtered_endpoint(endpoint.guid));
    EXPECT_FALSE(discovery_database.endpoint_exists(endpoint.guid));
    EXPECT_TRUE(test::get_all_endpoints(discovery_database).empty());
    EXPECT_EQ(erased_callbacks.load(), 0u);
    EXPECT_THROW(discovery_database.get_endpoint(endpoint.guid), eprosima::utils::InconsistencyException);

    discovery_database.stop();
}

/**
 * Test that erasing a filtered-only endpoint keeps the filter entry untouched
 *
 * STEPS:
 * - add one filtered endpoint GUID
 * - request an erase for the same GUID
 * - verify the filter entry remains because there is no stored endpoint to erase
 */
TEST(DiscoveryDatabaseTest, erase_does_not_remove_filtered_only_endpoint)
{
    DiscoveryDatabase discovery_database;
    discovery_database.start();

    auto endpoint = random_endpoint(8);

    std::atomic<uint32_t> erased_callbacks{0};

    discovery_database.add_endpoint_erased_callback(
        [&](
            types::Endpoint)
        {
            ++erased_callbacks;
        });

    discovery_database.add_filtered_endpoint(endpoint.guid);
    eprosima::utils::sleep_for(test::WAIT_TIME_MS);

    ASSERT_TRUE(discovery_database.exists_filtered_endpoint(endpoint.guid));
    ASSERT_FALSE(discovery_database.endpoint_exists(endpoint.guid));

    discovery_database.erase_endpoint(endpoint);
    eprosima::utils::sleep_for(test::WAIT_TIME_MS);

    EXPECT_TRUE(discovery_database.exists_filtered_endpoint(endpoint.guid));
    EXPECT_FALSE(discovery_database.endpoint_exists(endpoint.guid));
    EXPECT_TRUE(test::get_all_endpoints(discovery_database).empty());
    EXPECT_EQ(erased_callbacks.load(), 0u);
    EXPECT_EQ(discovery_database.get_endpoint(endpoint.guid), types::Endpoint{});

    discovery_database.stop();
}

/**
 * Test that stopping the discovery database clears stored endpoints and filtered endpoints
 *
 * STEPS:
 * - add one active endpoint and one filtered endpoint
 * - verify both are stored
 * - stop the discovery database
 * - verify both entries are removed from the internal state
 */
TEST(DiscoveryDatabaseTest, stop_clears_stored_endpoints)
{
    DiscoveryDatabase discovery_database;
    discovery_database.start();

    auto endpoint = random_endpoint(7);
    endpoint.active = true;
    auto filtered_guid = random_guid(11);

    discovery_database.add_endpoint(endpoint);
    discovery_database.add_filtered_endpoint(filtered_guid);
    eprosima::utils::sleep_for(test::WAIT_TIME_MS);

    ASSERT_TRUE(discovery_database.endpoint_exists(endpoint.guid));
    ASSERT_TRUE(discovery_database.exists_filtered_endpoint(filtered_guid));

    discovery_database.stop();

    EXPECT_FALSE(discovery_database.endpoint_exists(endpoint.guid));
    EXPECT_FALSE(discovery_database.exists_filtered_endpoint(filtered_guid));
    EXPECT_TRUE(test::get_all_endpoints(discovery_database).empty());
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
