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
 * @file schema_idl.cpp
 */

#include <ostream>
#include <map>

#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicTypeMember.h>
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/TypeDescriptor.h>

#include <cpp_utils/exception/InconsistencyException.hpp>
#include <cpp_utils/exception/UnsupportedException.hpp>
#include <cpp_utils/types/Tree.hpp>
#include <cpp_utils/utils.hpp>

#include <ddspipe_core/types/dynamic_types/schema.hpp>

namespace eprosima {
namespace ddspipe {
namespace core {
namespace types {
namespace idl {

constexpr const char* TYPE_OPENING =
        "\n{\n";

constexpr const char* TYPE_CLOSURE =
        "};\n";

constexpr const char* TAB_SEPARATOR =
        "    ";

struct TreeNodeType
{
    TreeNodeType(
            std::string member_name,
            std::string type_kind_name,
            fastrtps::types::DynamicType_ptr dynamic_type)
        : member_name(member_name)
        , type_kind_name(type_kind_name)
        , dynamic_type(dynamic_type)
    {
    }

    std::string member_name;
    std::string type_kind_name;
    fastrtps::types::DynamicType_ptr dynamic_type;
};

// Forward declaration
std::string type_kind_to_str(
        const fastrtps::types::DynamicType_ptr& type);

fastrtps::types::DynamicType_ptr container_internal_type(
        const fastrtps::types::DynamicType_ptr& dyn_type)
{
    return dyn_type->get_descriptor()->get_element_type();
}

std::vector<uint32_t> container_size(
        const fastrtps::types::DynamicType_ptr& dyn_type,
        bool unidimensional = true)
{
    if (unidimensional)
    {
        return {dyn_type->get_descriptor()->get_total_bounds()};
    }
    else
    {
        std::vector<uint32_t> result;
        for (unsigned int i = 0; i < dyn_type->get_descriptor()->get_bounds_size(); i++)
        {
            result.push_back(dyn_type->get_descriptor()->get_bounds(i));
        }
        return result;
    }
}

std::vector<std::pair<std::string, fastrtps::types::DynamicType_ptr>> get_members_sorted(
        const fastrtps::types::DynamicType_ptr& dyn_type)
{
    std::vector<std::pair<std::string, fastrtps::types::DynamicType_ptr>> result;

    std::map<fastrtps::types::MemberId, fastrtps::types::DynamicTypeMember*> members;
    dyn_type->get_all_members(members);

    for (const auto& member : members)
    {
        result.emplace_back(
            std::make_pair<std::string, fastrtps::types::DynamicType_ptr>(
                member.second->get_name(),
                member.second->get_descriptor()->get_type()));
    }
    return result;
}

std::string array_kind_to_str(
        const fastrtps::types::DynamicType_ptr& dyn_type)
{
    auto internal_type = container_internal_type(dyn_type);
    auto this_array_size = container_size(dyn_type);

    std::stringstream ss;
    ss << type_kind_to_str(internal_type);

    for (const auto& bound : this_array_size)
    {
        ss << "[" << bound << "]";
    }

    return ss.str();
}

std::string sequence_kind_to_str(
        const fastrtps::types::DynamicType_ptr& dyn_type)
{
    auto internal_type = container_internal_type(dyn_type);
    auto this_sequence_size = container_size(dyn_type);

    std::stringstream ss;
    ss << "sequence<" << type_kind_to_str(internal_type);

    for (const auto& bound : this_sequence_size)
    {
        if (bound != fastrtps::types::BOUND_UNLIMITED)
        {
            ss << ", " << bound;
        }
    }
    ss << ">";

    return ss.str();
}

std::string map_kind_to_str(
        const fastrtps::types::DynamicType_ptr& dyn_type)
{
    std::stringstream ss;

    auto key_type = dyn_type->get_descriptor()->get_key_element_type();
    auto value_type = dyn_type->get_descriptor()->get_element_type();
    ss << "map<" << type_kind_to_str(key_type) << ", " << type_kind_to_str(value_type) << ">";

    return ss.str();
}

std::string type_kind_to_str(
        const fastrtps::types::DynamicType_ptr& dyn_type)
{
    switch (dyn_type->get_kind())
    {
        case fastrtps::types::TK_BOOLEAN:
            return "boolean";

        case fastrtps::types::TK_BYTE:
            return "octet";

        case fastrtps::types::TK_INT16:
            return "short";

        case fastrtps::types::TK_INT32:
            return "long";

        case fastrtps::types::TK_INT64:
            return "long long";

        case fastrtps::types::TK_UINT16:
            return "unsigned short";

        case fastrtps::types::TK_UINT32:
            return "unsigned long";

        case fastrtps::types::TK_UINT64:
            return "unsigned long long";

        case fastrtps::types::TK_FLOAT32:
            return "float";

        case fastrtps::types::TK_FLOAT64:
            return "double";

        case fastrtps::types::TK_FLOAT128:
            return "long double";

        case fastrtps::types::TK_CHAR8:
            return "char";

        case fastrtps::types::TK_CHAR16:
            return "wchar";

        case fastrtps::types::TK_STRING8:
            return "string";

        case fastrtps::types::TK_STRING16:
            return "wstring";

        case fastrtps::types::TK_ARRAY:
            return array_kind_to_str(dyn_type);

        case fastrtps::types::TK_SEQUENCE:
            return sequence_kind_to_str(dyn_type);

        case fastrtps::types::TK_MAP:
            return map_kind_to_str(dyn_type);

        case fastrtps::types::TK_STRUCTURE:
        case fastrtps::types::TK_ENUM:
        case fastrtps::types::TK_UNION:
            return dyn_type->get_name();

        case fastrtps::types::TK_BITSET:
        case fastrtps::types::TK_BITMASK:
        case fastrtps::types::TK_NONE:
            throw utils::UnsupportedException(
                      STR_ENTRY << "Type " << dyn_type->get_name() << " is not supported.");

        default:
            throw utils::InconsistencyException(
                      STR_ENTRY << "Type " << dyn_type->get_name() << " has not correct kind.");

    }
}

utils::TreeNode<TreeNodeType> generate_dyn_type_tree(
        const fastrtps::types::DynamicType_ptr& type,
        const std::string& member_name = "PARENT")
{
    // Get kind
    fastrtps::types::TypeKind kind = type->get_kind();

    switch (kind)
    {
        case fastrtps::types::TK_STRUCTURE:
        {
            // If is struct, the call is recursive.
            // Create new tree node
            utils::TreeNode<TreeNodeType> parent(member_name, type->get_name(), type);

            // Get all members of this struct
            std::vector<std::pair<std::string,
                    fastrtps::types::DynamicType_ptr>> members_by_name = get_members_sorted(type);

            for (const auto& member : members_by_name)
            {
                // Add each member with its name as a new node in a branch (recursion)
                parent.add_branch(
                    generate_dyn_type_tree(member.second, member.first));
            }
            return parent;
        }

        case fastrtps::types::TK_ARRAY:
        case fastrtps::types::TK_SEQUENCE:
        {
            // If container (array or struct) has exactly one branch
            // Calculate child branch
            auto internal_type = container_internal_type(type);

            // Create this node
            utils::TreeNode<TreeNodeType> container(member_name, type_kind_to_str(type), type);
            // Add branch
            container.add_branch(generate_dyn_type_tree(internal_type, "CONTAINER_MEMBER"));

            return container;
        }

        default:
            return utils::TreeNode<TreeNodeType>(member_name, type_kind_to_str(type), type);
    }
}

std::ostream& node_to_str(
        std::ostream& os,
        const utils::TreeNode<TreeNodeType>& node)
{
    os << TAB_SEPARATOR;

    if (node.info.dynamic_type->get_kind() == fastrtps::types::TK_ARRAY)
    {
        auto dim_pos = node.info.type_kind_name.find("[");
        auto kind_name_str = node.info.type_kind_name.substr(0, dim_pos);
        auto dim_str = node.info.type_kind_name.substr(dim_pos, std::string::npos);

        os << kind_name_str << " " << node.info.member_name << dim_str;
    }
    else
    {
        os << node.info.type_kind_name << " " << node.info.member_name;
    }

    return os;
}

std::ostream& struct_to_str(
        std::ostream& os,
        const utils::TreeNode<TreeNodeType>& node)
{
    // Add types name
    os << "struct " << node.info.type_kind_name << TYPE_OPENING;

    // Add struct attributes
    for (auto const& child : node.branches())
    {
        node_to_str(os, child.info);
        os << ";\n";
    }

    // Close definition
    os << TYPE_CLOSURE;

    return os;
}

std::ostream& enum_to_str(
        std::ostream& os,
        const utils::TreeNode<TreeNodeType>& node)
{
    os << "enum " << node.info.type_kind_name << TYPE_OPENING << TAB_SEPARATOR;

    std::map<fastrtps::types::MemberId, fastrtps::types::DynamicTypeMember*> members;
    node.info.dynamic_type->get_all_members(members);
    bool first_iter = true;
    for (const auto& member : members)
    {
        if (!first_iter)
        {
            os << ",\n" << TAB_SEPARATOR;
        }
        first_iter = false;

        os << member.second->get_name();
    }

    // Close definition
    os << "\n" << TYPE_CLOSURE;

    return os;
}

std::ostream& union_to_str(
        std::ostream& os,
        const utils::TreeNode<TreeNodeType>& node)
{
    os << "union " << node.info.type_kind_name << " switch (" << type_kind_to_str(
        node.info.dynamic_type->get_descriptor()->get_discriminator_type()) << ")" << TYPE_OPENING;

    std::map<fastrtps::types::MemberId, fastrtps::types::DynamicTypeMember*> members;
    node.info.dynamic_type->get_all_members(members);  // WARNING: Default case not included in this collection, and currently not available
    for (const auto& member : members)
    {
        auto labels = member.second->get_union_labels();  // WARNING: There might be casting issues as discriminant type is currently not taken into consideration
        bool first_iter = true;
        for (const auto& label : labels)
        {
            if (first_iter)
            {
                os << TAB_SEPARATOR;
            }
            else
            {
                os << " ";
            }
            first_iter = false;

            os << "case " << std::to_string(label) << ":";
        }

        os << "\n" << TAB_SEPARATOR << TAB_SEPARATOR << type_kind_to_str(member.second->get_descriptor()->get_type()) <<
            " " << member.second->get_name() << ";\n";


    }

    // Close definition
    os << TYPE_CLOSURE;

    return os;
}

std::string generate_dyn_type_schema_from_tree(
        const utils::TreeNode<TreeNodeType>& parent_node)
{
    std::set<std::string> types_written;

    std::stringstream ss;

    // For every Node, check if it is of a "writable" type (i.e. struct, enum or union).
    // If it is, check if it is not yet written
    // If it is not, write it down
    for (const auto& node : parent_node.all_nodes())
    {
        auto kind = node.info.dynamic_type->get_kind();
        if (types_written.find(node.info.type_kind_name) == types_written.end())
        {
            switch (kind)
            {
                case fastrtps::types::TK_STRUCTURE:
                    struct_to_str(ss, node);
                    break;

                case fastrtps::types::TK_ENUM:
                    enum_to_str(ss, node);
                    break;

                case fastrtps::types::TK_UNION:
                    union_to_str(ss, node);
                    break;

                default:
                    continue;
            }
            ss << "\n"; // Introduce blank line between type definitions
            types_written.insert(node.info.type_kind_name);
        }
    }

    // Write struct parent node at last, after all its dependencies
    // NOTE: not a requirement for Foxglove IDL Parser, dependencies can be placed after parent
    struct_to_str(ss, parent_node);

    return ss.str();
}

std::string generate_idl_schema(
        const fastrtps::types::DynamicType_ptr& dynamic_type)
{
    // Generate type tree
    utils::TreeNode<TreeNodeType> parent_type = generate_dyn_type_tree(dynamic_type);

    // From tree, generate string
    return generate_dyn_type_schema_from_tree(parent_type);
}

} /* namespace idl */
} /* namespace types */
} /* namespace core */
} /* namespace ddspipe */
} /* namespace eprosima */
