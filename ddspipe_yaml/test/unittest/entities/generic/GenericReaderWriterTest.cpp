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

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>

#include <ddspipe_core/testing/arbitrary_values.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

#include <ddspipe_participants/types/address/Address.hpp>

#include <ddspipe_yaml/reader/YamlReader.hpp>
#include <ddspipe_yaml/writer/YamlWriter.hpp>

using namespace eprosima;

namespace test {

constexpr const unsigned int N_TEST_CASES = 1000;

template <typename T>
T arbitrary(
        unsigned int seed)
{
    return ddspipe::core::testing::arbitrary<T>(seed);
}

template <>
ddspipe::core::types::TopicQoS arbitrary(
        unsigned int seed)
{
    return ddspipe::core::testing::arbitrary<ddspipe::core::types::TopicQoS, bool>(seed, false);
}

} // namespace test

// Empty class to parametrized tests
template<class T>
struct GenericReaderWriterTest : public ::testing::Test
{};
// Needed gtest macro
TYPED_TEST_SUITE_P(GenericReaderWriterTest);

TYPED_TEST_P(GenericReaderWriterTest, generic_from_and_to)
{
    for (unsigned int i = 0 ; i < test::N_TEST_CASES ; ++i)
    {
        TypeParam write_object = test::arbitrary<TypeParam>(i);

        ddspipe::yaml::Yaml yml;
        ddspipe::yaml::write(yml, write_object);

        TypeParam read_object;
        ddspipe::yaml::read(yml, read_object);

        ASSERT_EQ(write_object, read_object) << "Failure in value " << i;
    }
}

// Register test class and test cases
REGISTER_TYPED_TEST_SUITE_P(
    GenericReaderWriterTest,
    generic_from_and_to
);

// Set types used in parametrization
typedef ::testing::Types<
        ddspipe::core::types::DdsTopic,
        ddspipe::core::types::TopicQoS
        // ddspipe::core::types::WildcardDdsFilterTopic
    > CaseTypes;

// Generate each test case for each type case
INSTANTIATE_TYPED_TEST_SUITE_P(
    ParametrizedGenericReaderWriterTest,
    GenericReaderWriterTest,
    CaseTypes);

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
