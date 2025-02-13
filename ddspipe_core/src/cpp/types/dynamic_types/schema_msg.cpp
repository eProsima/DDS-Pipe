// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

/**
 * @file schema_msg.cpp
 */

#include <cassert>
#include <map>
#include <ostream>

#include <fastdds/dds/core/ReturnCode.hpp>
#include <fastdds/dds/core/Types.hpp>
#include <fastdds/dds/xtypes/dynamic_types/detail/dynamic_language_binding.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeMember.hpp>
#include <fastdds/dds/xtypes/dynamic_types/TypeDescriptor.hpp>
#include <fastdds/dds/xtypes/type_representation/detail/dds_xtypes_typeobject.hpp>

#include <cpp_utils/exception/InconsistencyException.hpp>
#include <cpp_utils/exception/UnsupportedException.hpp>
#include <cpp_utils/types/Tree.hpp>
#include <cpp_utils/utils.hpp>
#include <cpp_utils/ros2_mangling.hpp>

#include <ddspipe_core/types/dynamic_types/schema.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {
namespace msg {

constexpr const char* TYPE_SEPARATOR =
        "================================================================================\n";

struct TreeNodeType
{
    TreeNodeType(
            std::string member_name,
            std::string type_kind_name,
            bool is_struct = false)
        : member_name(member_name)
        , type_kind_name(type_kind_name)
        , is_struct(is_struct)
    {
    }

    std::string member_name;
    std::string type_kind_name;
    bool is_struct;
};

// Forward declaration
std::string type_kind_to_str(
        const fastdds::dds::DynamicType::_ref_type& type);

fastdds::dds::DynamicType::_ref_type container_internal_type(
        const fastdds::dds::DynamicType::_ref_type& dyn_type)
{
    fastdds::dds::TypeDescriptor::_ref_type type_descriptor {fastdds::dds::traits<fastdds::dds::TypeDescriptor>::
                                                             make_shared()};
    if (fastdds::dds::RETCODE_OK != dyn_type->get_descriptor(type_descriptor))
    {
        throw utils::InconsistencyException(
                  "Failed to get Type Descriptor for type: " + dyn_type->get_name().to_string());
    }

    return type_descriptor->element_type();
}

fastdds::dds::BoundSeq array_size(
        const fastdds::dds::DynamicType::_ref_type& dyn_type)
{
    fastdds::dds::TypeDescriptor::_ref_type type_descriptor {fastdds::dds::traits<fastdds::dds::TypeDescriptor>::
                                                             make_shared()};
    if (fastdds::dds::RETCODE_OK != dyn_type->get_descriptor(type_descriptor))
    {
        throw utils::InconsistencyException(
                  "Failed to get Type Descriptor for type: " + dyn_type->get_name().to_string());
    }

    return type_descriptor->bound();
}

std::vector<std::pair<std::string, fastdds::dds::DynamicType::_ref_type>> get_members_sorted(
        const fastdds::dds::DynamicType::_ref_type& dyn_type)
{
    std::vector<std::pair<std::string, fastdds::dds::DynamicType::_ref_type>> result;

    std::map<fastdds::dds::MemberId, fastdds::dds::DynamicTypeMember::_ref_type> members;
    if (fastdds::dds::RETCODE_OK != dyn_type->get_all_members(members))
    {
        throw utils::InconsistencyException(
                  "Failed to get Type Members for type: " + dyn_type->get_name().to_string());
    }

    for (const auto& member : members)
    {
        fastdds::dds::ObjectName dyn_name = member.second->get_name();

        fastdds::dds::MemberDescriptor::_ref_type member_descriptor {fastdds::dds::traits<fastdds::dds::MemberDescriptor>
                                                                     ::make_shared()};
        if (fastdds::dds::RETCODE_OK != member.second->get_descriptor(member_descriptor))
        {
            throw utils::InconsistencyException(
                      "Failed to get Type Descriptor of Member with name: " + dyn_name.to_string());
        }

        result.emplace_back(
            std::make_pair<std::string, fastdds::dds::DynamicType::_ref_type>(
                utils::demangle_if_ros_type(dyn_name.to_string()),
                std::move(member_descriptor->type())));
    }
    return result;
}

std::string container_kind_to_str(
        const fastdds::dds::DynamicType::_ref_type& dyn_type,
        bool allow_bounded = false)
{
    auto internal_type = container_internal_type(dyn_type);
    auto this_array_size = array_size(dyn_type);

    // TODO: Change exception -> RETCODE
    if (this_array_size.size() != 1)
    {
        throw utils::UnsupportedException(
                STR_ENTRY << "Failed to convert container type: Multidimensional arrays are not supported in ROS2.");
    }

    std::stringstream ss;
    ss << type_kind_to_str(internal_type);

    for (const auto& bound : this_array_size)
    {
        if (bound != static_cast<std::uint32_t>(fastdds::dds::LENGTH_UNLIMITED))
        {
            if (allow_bounded)
            {
                ss << "[<=" << bound << "]";
            }
            else
            {
                ss << "[" << bound << "]";
            }
        }
        else
        {
            ss << "[]";
        }
    }

    return ss.str();
}

std::string type_kind_to_str(
        const fastdds::dds::DynamicType::_ref_type& dyn_type)
{
    switch (dyn_type->get_kind())
    {
        case fastdds::dds::xtypes::TK_BOOLEAN:
            return "bool";

        case fastdds::dds::xtypes::TK_BYTE:
            return "byte";

        case fastdds::dds::xtypes::TK_INT8:
            return "int8";

        case fastdds::dds::xtypes::TK_INT16:
            return "int16";

        case fastdds::dds::xtypes::TK_INT32:
            return "int32";

        case fastdds::dds::xtypes::TK_INT64:
            return "int64";

        case fastdds::dds::xtypes::TK_UINT8:
            return "uint8";

        case fastdds::dds::xtypes::TK_UINT16:
            return "uint16";

        case fastdds::dds::xtypes::TK_UINT32:
            return "uint32";

        case fastdds::dds::xtypes::TK_UINT64:
            return "uint64";

        case fastdds::dds::xtypes::TK_FLOAT32:
            return "float32";

        case fastdds::dds::xtypes::TK_FLOAT64:
            return "float64";

        case fastdds::dds::xtypes::TK_CHAR8:
            return "char";

        case fastdds::dds::xtypes::TK_STRING8:
            return "string";

        case fastdds::dds::xtypes::TK_ARRAY:
            return container_kind_to_str(dyn_type);

        case fastdds::dds::xtypes::TK_SEQUENCE:
            return container_kind_to_str(dyn_type, true);

        case fastdds::dds::xtypes::TK_STRUCTURE:
            return utils::demangle_if_ros_type((dyn_type->get_name()).to_string());

        case fastdds::dds::xtypes::TK_FLOAT128:
        case fastdds::dds::xtypes::TK_CHAR16:
        case fastdds::dds::xtypes::TK_STRING16:
        case fastdds::dds::xtypes::TK_ENUM:
        case fastdds::dds::xtypes::TK_BITSET:
        case fastdds::dds::xtypes::TK_MAP:
        case fastdds::dds::xtypes::TK_UNION:
        case fastdds::dds::xtypes::TK_NONE:
            throw utils::UnsupportedException(
                      STR_ENTRY << "Type " << utils::demangle_if_ros_type(
                          (dyn_type->get_name()).to_string()) << " is not supported in ROS2 msg.");

        default:
            throw utils::InconsistencyException(
                      STR_ENTRY << "Type " << utils::demangle_if_ros_type(
                          (dyn_type->get_name()).to_string()) << " has not correct kind.");
    }
}

utils::TreeNode<TreeNodeType> generate_dyn_type_tree(
        const fastdds::dds::DynamicType::_ref_type& type,
        const std::string& member_name = "PARENT")
{
    // Get kind
    fastdds::dds::xtypes::TypeKind kind = type->get_kind();

    switch (kind)
    {
        case fastdds::dds::xtypes::TK_STRUCTURE:
        {
            // If is struct, the call is recursive.
            // Create new tree node
            utils::TreeNode<TreeNodeType> parent(member_name, utils::demangle_if_ros_type(
                        (type->get_name()).to_string()), true);

            // Get all members of this struct
            std::vector<std::pair<std::string,
                    fastdds::dds::DynamicType::_ref_type>> members_by_name = get_members_sorted(type);

            for (const auto& member : members_by_name)
            {
                // Add each member with its name as a new node in a branch (recursion)
                parent.add_branch(
                    generate_dyn_type_tree(member.second, member.first));
            }
            return parent;
        }

        case fastdds::dds::xtypes::TK_ARRAY:
        case fastdds::dds::xtypes::TK_SEQUENCE:
        {
            // If container (array or struct) has exactly one branch
            // Calculate child branch
            auto internal_type = container_internal_type(type);

            // Create this node
            utils::TreeNode<TreeNodeType> container(member_name, type_kind_to_str(type));
            // Add branch
            container.add_branch(generate_dyn_type_tree(internal_type, "CONTAINER_MEMBER"));

            return container;
        }

        default:
            return utils::TreeNode<TreeNodeType>(member_name, type_kind_to_str(type));
    }
}

std::ostream& node_to_str(
        std::ostream& os,
        const TreeNodeType& node)
{
    os << node.type_kind_name << " " << node.member_name;

    return os;
}

std::ostream& generate_schema_from_node(
        std::ostream& os,
        const utils::TreeNode<TreeNodeType>& node)
{
    // We know for sure this is called from structs

    for (auto const& child : node.branches())
    {
        node_to_str(os, child.info);
        os << "\n";
    }

    return os;
}

std::string generate_dyn_type_schema_from_tree(
        const utils::TreeNode<TreeNodeType>& parent_node)
{
    std::set<std::string> types_written;

    std::stringstream ss;

    // Write down main node
    generate_schema_from_node(ss, parent_node);
    types_written.insert(parent_node.info.type_kind_name);

    // For every Node, check if it is a struct.
    // If it is, check if it is not yet written
    // If it is not, write it down
    for (const auto& node : parent_node.all_nodes())
    {
        if (node.info.is_struct && types_written.find(node.info.type_kind_name) == types_written.end())
        {
            // Add types separator
            ss << TYPE_SEPARATOR;

            // Add types name
            if (node.info.type_kind_name.find('/') == std::string::npos)
            {
                // Append namespace when none present as required by Foxglove (TODO: check for types generated with namespace)
                ss << "MSG: fastdds/" << node.info.type_kind_name << "\n";
            }
            else
            {
                ss << "MSG: " << node.info.type_kind_name << "\n";
            }

            // Add next type
            generate_schema_from_node(ss, node);
            types_written.insert(node.info.type_kind_name);
        }
    }

    return ss.str();
}

std::string generate_ros2_schema(
        const fastdds::dds::DynamicType::_ref_type& dynamic_type)
{
    // Generate type tree
    utils::TreeNode<TreeNodeType> parent_type = generate_dyn_type_tree(dynamic_type);

    // From tree, generate string
    return generate_dyn_type_schema_from_tree(parent_type);
}

} /* namespace msg */
} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
