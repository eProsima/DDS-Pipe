// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

/*!
 * @file numeric_arrayTypeObjectSupport.cxx
 * Source file containing the implementation to register the TypeObject representation of the described types in the IDL file
 *
 * This file was generated by the tool fastddsgen.
 */

#include "numeric_arrayTypeObjectSupport.hpp"

#include <mutex>
#include <string>

#include <fastcdr/xcdr/external.hpp>
#include <fastcdr/xcdr/optional.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/xtypes/common.hpp>
#include <fastdds/dds/xtypes/type_representation/ITypeObjectRegistry.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObject.hpp>
#include <fastdds/dds/xtypes/type_representation/TypeObjectUtils.hpp>

#include "numeric_array.hpp"


using namespace eprosima::fastdds::dds::xtypes;

// TypeIdentifier is returned by reference: dependent structures/unions are registered in this same method
void register_numeric_array_type_identifier(
        TypeIdentifierPair& type_ids_numeric_array)
{

    ReturnCode_t return_code_numeric_array {eprosima::fastdds::dds::RETCODE_OK};
    return_code_numeric_array =
        eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
        "numeric_array", type_ids_numeric_array);
    if (eprosima::fastdds::dds::RETCODE_OK != return_code_numeric_array)
    {
        StructTypeFlag struct_flags_numeric_array = TypeObjectUtils::build_struct_type_flag(eprosima::fastdds::dds::xtypes::ExtensibilityKind::NOT_APPLIED,
                false, false);
        QualifiedTypeName type_name_numeric_array = "numeric_array";
        eprosima::fastcdr::optional<AppliedBuiltinTypeAnnotations> type_ann_builtin_numeric_array;
        eprosima::fastcdr::optional<AppliedAnnotationSeq> ann_custom_numeric_array;
        CompleteTypeDetail detail_numeric_array = TypeObjectUtils::build_complete_type_detail(type_ann_builtin_numeric_array, ann_custom_numeric_array, type_name_numeric_array.to_string());
        CompleteStructHeader header_numeric_array;
        header_numeric_array = TypeObjectUtils::build_complete_struct_header(TypeIdentifier(), detail_numeric_array);
        CompleteStructMemberSeq member_seq_numeric_array;
        {
            TypeIdentifierPair type_ids_points;
            ReturnCode_t return_code_points {eprosima::fastdds::dds::RETCODE_OK};
            return_code_points =
                eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
                "anonymous_array_int32_t_3", type_ids_points);

            if (eprosima::fastdds::dds::RETCODE_OK != return_code_points)
            {
                return_code_points =
                    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
                    "_int32_t", type_ids_points);

                if (eprosima::fastdds::dds::RETCODE_OK != return_code_points)
                {
                    EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                            "Array element TypeIdentifier unknown to TypeObjectRegistry.");
                    return;
                }
                bool element_identifier_anonymous_array_int32_t_3_ec {false};
                TypeIdentifier* element_identifier_anonymous_array_int32_t_3 {new TypeIdentifier(TypeObjectUtils::retrieve_complete_type_identifier(type_ids_points, element_identifier_anonymous_array_int32_t_3_ec))};
                if (!element_identifier_anonymous_array_int32_t_3_ec)
                {
                    EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION, "Array element TypeIdentifier inconsistent.");
                    return;
                }
                EquivalenceKind equiv_kind_anonymous_array_int32_t_3 = EK_COMPLETE;
                if (TK_NONE == type_ids_points.type_identifier2()._d())
                {
                    equiv_kind_anonymous_array_int32_t_3 = EK_BOTH;
                }
                CollectionElementFlag element_flags_anonymous_array_int32_t_3 = 0;
                PlainCollectionHeader header_anonymous_array_int32_t_3 = TypeObjectUtils::build_plain_collection_header(equiv_kind_anonymous_array_int32_t_3, element_flags_anonymous_array_int32_t_3);
                {
                    SBoundSeq array_bound_seq;
                        TypeObjectUtils::add_array_dimension(array_bound_seq, static_cast<SBound>(3));

                    PlainArraySElemDefn array_sdefn = TypeObjectUtils::build_plain_array_s_elem_defn(header_anonymous_array_int32_t_3, array_bound_seq,
                                eprosima::fastcdr::external<TypeIdentifier>(element_identifier_anonymous_array_int32_t_3));
                    if (eprosima::fastdds::dds::RETCODE_BAD_PARAMETER ==
                            TypeObjectUtils::build_and_register_s_array_type_identifier(array_sdefn, "anonymous_array_int32_t_3", type_ids_points))
                    {
                        EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                            "anonymous_array_int32_t_3 already registered in TypeObjectRegistry for a different type.");
                    }
                }
            }
            StructMemberFlag member_flags_points = TypeObjectUtils::build_struct_member_flag(eprosima::fastdds::dds::xtypes::TryConstructKind::NOT_APPLIED,
                    false, false, false, false);
            MemberId member_id_points = 0x00000000;
            bool common_points_ec {false};
            CommonStructMember common_points {TypeObjectUtils::build_common_struct_member(member_id_points, member_flags_points, TypeObjectUtils::retrieve_complete_type_identifier(type_ids_points, common_points_ec))};
            if (!common_points_ec)
            {
                EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION, "Structure points member TypeIdentifier inconsistent.");
                return;
            }
            MemberName name_points = "points";
            eprosima::fastcdr::optional<AppliedBuiltinMemberAnnotations> member_ann_builtin_points;
            ann_custom_numeric_array.reset();
            CompleteMemberDetail detail_points = TypeObjectUtils::build_complete_member_detail(name_points, member_ann_builtin_points, ann_custom_numeric_array);
            CompleteStructMember member_points = TypeObjectUtils::build_complete_struct_member(common_points, detail_points);
            TypeObjectUtils::add_complete_struct_member(member_seq_numeric_array, member_points);
        }
        CompleteStructType struct_type_numeric_array = TypeObjectUtils::build_complete_struct_type(struct_flags_numeric_array, header_numeric_array, member_seq_numeric_array);
        if (eprosima::fastdds::dds::RETCODE_BAD_PARAMETER ==
                TypeObjectUtils::build_and_register_struct_type_object(struct_type_numeric_array, type_name_numeric_array.to_string(), type_ids_numeric_array))
        {
            EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                    "numeric_array already registered in TypeObjectRegistry for a different type.");
        }
    }
}

