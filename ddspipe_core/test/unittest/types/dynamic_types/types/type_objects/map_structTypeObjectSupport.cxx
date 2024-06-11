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
 * @file map_structTypeObjectSupport.cxx
 * Source file containing the implementation to register the TypeObject representation of the described types in the IDL file
 *
 * This file was generated by the tool fastddsgen.
 */

#include "map_structTypeObjectSupport.hpp"

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

#include "map_struct.hpp"


using namespace eprosima::fastdds::dds::xtypes;

// TypeIdentifier is returned by reference: dependent structures/unions are registered in this same method
void register_map_struct_type_identifier(
        TypeIdentifierPair& type_ids_map_struct)
{

    ReturnCode_t return_code_map_struct {eprosima::fastdds::dds::RETCODE_OK};
    return_code_map_struct =
        eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
        "map_struct", type_ids_map_struct);
    if (eprosima::fastdds::dds::RETCODE_OK != return_code_map_struct)
    {
        StructTypeFlag struct_flags_map_struct = TypeObjectUtils::build_struct_type_flag(eprosima::fastdds::dds::xtypes::ExtensibilityKind::NOT_APPLIED,
                false, false);
        QualifiedTypeName type_name_map_struct = "map_struct";
        eprosima::fastcdr::optional<AppliedBuiltinTypeAnnotations> type_ann_builtin_map_struct;
        eprosima::fastcdr::optional<AppliedAnnotationSeq> ann_custom_map_struct;
        CompleteTypeDetail detail_map_struct = TypeObjectUtils::build_complete_type_detail(type_ann_builtin_map_struct, ann_custom_map_struct, type_name_map_struct.to_string());
        CompleteStructHeader header_map_struct;
        header_map_struct = TypeObjectUtils::build_complete_struct_header(TypeIdentifier(), detail_map_struct);
        CompleteStructMemberSeq member_seq_map_struct;
        {
            TypeIdentifierPair type_ids_my_map;
            ReturnCode_t return_code_my_map {eprosima::fastdds::dds::RETCODE_OK};
            return_code_my_map =
                eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
                "anonymous_map_anonymous_string_unbounded_bool_unbounded", type_ids_my_map);

            if (eprosima::fastdds::dds::RETCODE_OK != return_code_my_map)
            {
                return_code_my_map =
                    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
                    "_bool", type_ids_my_map);

                if (eprosima::fastdds::dds::RETCODE_OK != return_code_my_map)
                {
                    EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                            "Map element TypeIdentifier unknown to TypeObjectRegistry.");
                    return;
                }
                bool element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded_ec {false};
                TypeIdentifier* element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded {new TypeIdentifier(TypeObjectUtils::retrieve_complete_type_identifier(type_ids_my_map, element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded_ec))};
                if (!element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded_ec)
                {
                    EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                        "anonymous_map_anonymous_string_unbounded_bool_unbounded inconsistent element TypeIdentifier.");
                    return;
                }
                return_code_my_map =
                    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->type_object_registry().get_type_identifiers(
                    "anonymous_string_unbounded", type_ids_my_map);

                if (eprosima::fastdds::dds::RETCODE_OK != return_code_my_map)
                {
                    {
                        SBound bound = 0;
                        StringSTypeDefn string_sdefn = TypeObjectUtils::build_string_s_type_defn(bound);
                        if (eprosima::fastdds::dds::RETCODE_BAD_PARAMETER ==
                                TypeObjectUtils::build_and_register_s_string_type_identifier(string_sdefn,
                                "anonymous_string_unbounded", type_ids_my_map))
                        {
                            EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                                "anonymous_string_unbounded already registered in TypeObjectRegistry for a different type.");
                        }
                    }
                }
                bool key_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded_ec {false};
                TypeIdentifier* key_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded {new TypeIdentifier(TypeObjectUtils::retrieve_complete_type_identifier(type_ids_my_map, key_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded_ec))};
                if (!key_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded_ec)
                {
                    EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                        "anonymous_map_anonymous_string_unbounded_bool_unbounded inconsistent key TypeIdentifier.");
                    return;
                }
                EquivalenceKind equiv_kind_anonymous_map_anonymous_string_unbounded_bool_unbounded = EK_BOTH;
                if ((EK_COMPLETE == key_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->_d() || EK_COMPLETE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->_d()) ||
                        (TI_PLAIN_SEQUENCE_SMALL == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->_d() && EK_COMPLETE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->seq_sdefn().header().equiv_kind()) ||
                        (TI_PLAIN_SEQUENCE_LARGE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->_d() && EK_COMPLETE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->seq_ldefn().header().equiv_kind()) ||
                        (TI_PLAIN_ARRAY_SMALL == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->_d() && EK_COMPLETE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->array_sdefn().header().equiv_kind()) ||
                        (TI_PLAIN_ARRAY_LARGE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->_d() && EK_COMPLETE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->array_ldefn().header().equiv_kind()) ||
                        (TI_PLAIN_MAP_SMALL == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->_d() && (EK_COMPLETE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->map_sdefn().key_identifier()->_d() || EK_COMPLETE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->map_sdefn().header().equiv_kind())) ||
                        (TI_PLAIN_MAP_LARGE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->_d() && (EK_COMPLETE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->map_ldefn().key_identifier()->_d() || EK_COMPLETE == element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded->map_ldefn().header().equiv_kind())))
                {
                    equiv_kind_anonymous_map_anonymous_string_unbounded_bool_unbounded = EK_COMPLETE;
                }
                CollectionElementFlag element_flags_anonymous_map_anonymous_string_unbounded_bool_unbounded = 0;
                CollectionElementFlag key_flags_anonymous_map_anonymous_string_unbounded_bool_unbounded = 0;
                PlainCollectionHeader header_anonymous_map_anonymous_string_unbounded_bool_unbounded = TypeObjectUtils::build_plain_collection_header(equiv_kind_anonymous_map_anonymous_string_unbounded_bool_unbounded, element_flags_anonymous_map_anonymous_string_unbounded_bool_unbounded);
                {
                    SBound bound = 0;
                    PlainMapSTypeDefn map_sdefn = TypeObjectUtils::build_plain_map_s_type_defn(header_anonymous_map_anonymous_string_unbounded_bool_unbounded, bound,
                                eprosima::fastcdr::external<TypeIdentifier>(element_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded), key_flags_anonymous_map_anonymous_string_unbounded_bool_unbounded,
                                eprosima::fastcdr::external<TypeIdentifier>(key_identifier_anonymous_map_anonymous_string_unbounded_bool_unbounded));
                    if (eprosima::fastdds::dds::RETCODE_BAD_PARAMETER ==
                            TypeObjectUtils::build_and_register_s_map_type_identifier(map_sdefn, "anonymous_map_anonymous_string_unbounded_bool_unbounded", type_ids_my_map))
                    {
                        EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                            "anonymous_map_anonymous_string_unbounded_bool_unbounded already registered in TypeObjectRegistry for a different type.");
                    }
                }
            }
            StructMemberFlag member_flags_my_map = TypeObjectUtils::build_struct_member_flag(eprosima::fastdds::dds::xtypes::TryConstructKind::NOT_APPLIED,
                    false, false, false, false);
            MemberId member_id_my_map = 0x00000000;
            bool common_my_map_ec {false};
            CommonStructMember common_my_map {TypeObjectUtils::build_common_struct_member(member_id_my_map, member_flags_my_map, TypeObjectUtils::retrieve_complete_type_identifier(type_ids_my_map, common_my_map_ec))};
            if (!common_my_map_ec)
            {
                EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION, "Structure my_map member TypeIdentifier inconsistent.");
                return;
            }
            MemberName name_my_map = "my_map";
            eprosima::fastcdr::optional<AppliedBuiltinMemberAnnotations> member_ann_builtin_my_map;
            ann_custom_map_struct.reset();
            CompleteMemberDetail detail_my_map = TypeObjectUtils::build_complete_member_detail(name_my_map, member_ann_builtin_my_map, ann_custom_map_struct);
            CompleteStructMember member_my_map = TypeObjectUtils::build_complete_struct_member(common_my_map, detail_my_map);
            TypeObjectUtils::add_complete_struct_member(member_seq_map_struct, member_my_map);
        }
        CompleteStructType struct_type_map_struct = TypeObjectUtils::build_complete_struct_type(struct_flags_map_struct, header_map_struct, member_seq_map_struct);
        if (eprosima::fastdds::dds::RETCODE_BAD_PARAMETER ==
                TypeObjectUtils::build_and_register_struct_type_object(struct_type_map_struct, type_name_map_struct.to_string(), type_ids_map_struct))
        {
            EPROSIMA_LOG_ERROR(XTYPES_TYPE_REPRESENTATION,
                    "map_struct already registered in TypeObjectRegistry for a different type.");
        }
    }
}

