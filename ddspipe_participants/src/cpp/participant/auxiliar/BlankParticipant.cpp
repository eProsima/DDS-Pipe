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


#include <ddspipe_participants/participant/auxiliar/BlankParticipant.hpp>
#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

BlankParticipant::BlankParticipant(
        const core::types::ParticipantId& id)
    : id_(id)
{
}

core::types::ParticipantId BlankParticipant::id() const noexcept
{
    return id_;
}

bool BlankParticipant::is_repeater() const noexcept
{
    return false;
}

bool BlankParticipant::is_rtps_kind() const noexcept
{
    return false;
}

core::types::TopicQoS BlankParticipant::topic_qos() const noexcept
{
    core::types::TopicQoS m_topic_qos;
    return m_topic_qos;
}

std::map<std::string, std::map<std::string, std::string>> BlankParticipant::topic_partitions() const noexcept
{
    return partition_names;
}

std::shared_ptr<core::IWriter> BlankParticipant::create_writer(
        const core::ITopic& topic)
{
    return std::make_shared<BlankWriter>();
}

std::shared_ptr<core::IReader> BlankParticipant::create_reader(
        const core::ITopic& topic)
{
    return std::make_shared<BlankReader>();
}

bool BlankParticipant::add_topic_partition(
        const std::string& topic_name,
        const std::string& writer_guid,
        const std::string& partition)
{
    if (partition_names.find(topic_name) != partition_names.end())
    {
        // the topic exists
        if (partition_names[topic_name].find(writer_guid) != partition_names[topic_name].end())
        {
            // the writer is already added in the topic
            return false;
        }
    }
    else
    {
        // there is no topic in the dictionary
        partition_names[topic_name] = std::map<std::string, std::string>();
    }

    // adds [writer, partition] in the topic
    partition_names[topic_name][writer_guid] = partition;

    return true;
}

bool BlankParticipant::update_topic_partition(
        const std::string& topic_name,
        const std::string& writer_guid,
        const std::string& partition)
{
    if (partition_names.find(topic_name) == partition_names.end())
    {
        // the topic dont exists
        return false;
    }
    if (partition_names[topic_name].find(writer_guid) == partition_names[topic_name].end())
    {
        // the writer dont exist in the topic

        return false;
    }

    // update [writer, partition] in the topic
    partition_names[topic_name][writer_guid] = partition;

    return true;
}

bool BlankParticipant::delete_topic_partition(
        const std::string& topic_name,
        const std::string& writer_guid,
        const std::string& partition)
{
    if (partition_names.find(topic_name) == partition_names.end())
    {
        // the topic dont exists
        return false;
    }
    if (partition_names[topic_name].find(writer_guid) == partition_names[topic_name].end())
    {
        // the writer dont exist in the topic
        return false;
    }

    // delete [writer, partition] in the topic
    partition_names.erase(writer_guid);

    return true;
}

void BlankParticipant::clear_topic_partitions()
{
    partition_names.clear();
}

void BlankParticipant::update_filters(
        const int flag,
        std::set<std::string> partitions,
        const std::string& topic_name,
        const std::string& expression)
{
    // Nothing
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
