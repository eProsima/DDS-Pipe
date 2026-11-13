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

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeMember.hpp>

#include <cpp_utils/ReturnCode.hpp>

#include <ddspipe_participants/utils/idl_types.hpp>

using namespace eprosima;
using namespace eprosima::ddspipe;

/**
 * Only the structures of an IDL file are loaded.
 */
TEST(IdlTypesTest, load_struct_types)
{
    std::vector<fastdds::dds::DynamicType::_ref_type> types;

    ASSERT_EQ(utils::ReturnCode::RETCODE_OK, participants::load_idl_types({"idl/Structs.idl"}, {}, types));

    ASSERT_EQ(2u, types.size());
    EXPECT_EQ("First", types[0]->get_name().to_string());
    EXPECT_EQ("Second", types[1]->get_name().to_string());
}

/**
 * A file that does not exist loads nothing and is reported.
 */
TEST(IdlTypesTest, load_missing_file)
{
    std::vector<fastdds::dds::DynamicType::_ref_type> types;

    EXPECT_NE(utils::ReturnCode::RETCODE_OK, participants::load_idl_types({"idl/DoesNotExist.idl"}, {}, types));
    EXPECT_TRUE(types.empty());
}

/**
 * A file without structures loads no types.
 */
TEST(IdlTypesTest, load_without_structs)
{
    std::vector<fastdds::dds::DynamicType::_ref_type> types;

    EXPECT_EQ(utils::ReturnCode::RETCODE_NO_DATA, participants::load_idl_types({"idl/NoStructs.idl"}, {}, types));
    EXPECT_TRUE(types.empty());
}

/**
 * A type name keeps the definition of the first file, and a different one is reported.
 */
TEST(IdlTypesTest, load_keeps_first_definition)
{
    std::vector<fastdds::dds::DynamicType::_ref_type> types;

    EXPECT_EQ(utils::ReturnCode::RETCODE_ERROR,
            participants::load_idl_types({"idl/Structs.idl", "idl/DifferentFirst.idl"}, {}, types));

    ASSERT_EQ(2u, types.size());
    ASSERT_EQ("First", types[0]->get_name().to_string());

    fastdds::dds::DynamicTypeMember::_ref_type member;
    EXPECT_EQ(fastdds::dds::RETCODE_OK, types[0]->get_member_by_name(member, "value"));
}

int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
