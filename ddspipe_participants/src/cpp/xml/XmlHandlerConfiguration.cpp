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

#include <cpp_utils/file/file_utils.hpp>
#include <cpp_utils/utils.hpp>

#include <ddspipe_participants/xml/XmlHandlerConfiguration.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {


bool XmlHandlerConfiguration::is_valid(
        utils::Formatter& error_msg) const noexcept
{
    // Check files
    for (const auto& file : files)
    {
        if (!utils::is_file_accessible(file.c_str(), utils::FileAccessMode::read))
        {
            error_msg << "File " << file << " has not exist or does not have read access. ";
            return false;
        }
    }

    return true;
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
