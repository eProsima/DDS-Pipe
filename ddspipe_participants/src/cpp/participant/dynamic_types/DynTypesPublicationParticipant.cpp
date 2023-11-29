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

#include <memory>

#include <cpp_utils/Log.hpp>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastrtps/rtps/participant/RTPSParticipant.h>
#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/TypeObjectFactory.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/attributes/RTPSParticipantAttributes.h>
#include <fastdds/dds/publisher/Publisher.hpp>

#include <ddspipe_core/types/dynamic_types/types.hpp>

#include <ddspipe_participants/participant/rtps/CommonParticipant.hpp>
#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>
#include <ddspipe_participants/reader/rtps/SimpleReader.hpp>
#include <ddspipe_participants/reader/rtps/SpecificQoSReader.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>
#include <ddspipe_participants/writer/auxiliar/InternalWriter.hpp>

#include <ddspipe_participants/participant/dynamic_types/DynTypesPublicationParticipant.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {

using namespace eprosima::ddspipe::core;
using namespace eprosima::ddspipe::core::types;
using namespace eprosima::fastrtps::types;

DynTypesPublicationParticipant::DynTypesPublicationParticipant(
        const std::shared_ptr<SimpleParticipantConfiguration>& participant_configuration,
        const std::shared_ptr<core::DiscoveryDatabase>& discovery_database)
    : dds::DdsCommonParticipant(participant_configuration)
{
    // Create Internal Writer to receive Type Objects
    // TODO: study why couldn't do with bind and try it, better than create a lambda
    auto participant_callback = [this](ddspipe::core::IRoutingData& data)
            {
                return this->receive_type_object_(data);
            };

    type_object_writer_ = std::make_shared<InternalWriter>(
        participant_configuration->id,
        participant_callback);
}

DynTypesPublicationParticipant::~DynTypesPublicationParticipant()
{
    for (auto& writer : writers_)
    {
        if (nullptr != writer.second.second)
        {
            dds_publisher_->delete_datawriter(writer.second.second);
            dds_participant_->delete_topic(writer.second.first);
        }
    }
}

std::shared_ptr<IWriter> DynTypesPublicationParticipant::create_writer(
        const ITopic& topic)
{
    if (is_type_object_topic(topic))
    {
        return this->type_object_writer_;
    }

    // This participant does not write anything
    return std::make_shared<BlankWriter>();
}

std::shared_ptr<IReader> DynTypesPublicationParticipant::create_reader(
        const ITopic& topic)
{
    // In case of a DDS topic, check if datawriter must be created, or already exist
    if (topic.internal_type_discriminator() == INTERNAL_TOPIC_TYPE_RTPS)
    {
        std::lock_guard<std::mutex> _(mutex_);
        create_empty_datawriter_nts_(dynamic_cast<const core::types::DdsTopic&>(topic));
    }

    // This participant does not read anything
    return std::make_shared<BlankReader>();
}

utils::ReturnCode DynTypesPublicationParticipant::receive_type_object_(
        eprosima::fastrtps::types::DynamicType_ptr dynamic_type)
{
    // All this function is protected
    std::lock_guard<std::mutex> _(mutex_);

    auto type_name = dynamic_type->get_name();

    if (type_name == "DdsRecorderCommand")
    {
        logError(DEBUG_COMMAND, "(DYN TYPES PUBLISHER) Received type object for type " << type_name);
    }
    else
    {
        logInfo(DDSPIPE_DYNTYPES_DDSPARTICIPANT, "Received type object for type " << type_name);
    }

    // First, check if the type already exist. If so, nothing to do
    auto it = types_discovered_.find(type_name);
    if (it != types_discovered_.end())
    {
        return utils::ReturnCode::RETCODE_OK;
    }

    // It is a new type, so add it to the types
    types_discovered_[type_name] = dynamic_type;

    // Register Participant
    eprosima::fastdds::dds::TypeSupport type(new eprosima::fastrtps::types::DynamicPubSubType(dynamic_type));
    type->auto_fill_type_information(true);
    type->auto_fill_type_object(true);
    dds_participant_->register_type(type);

    // Check if any topic already discovered uses this new type. If so create data writer
    std::vector<core::types::DdsTopic> topics_to_create_writers;
    for (const auto& writer : writers_)
    {
        if (writer.first.type_name == type_name)
        {
            // This should always be nullptr, but just in case
            if (nullptr == writer.second.second)
            {
                topics_to_create_writers.push_back(writer.first);
            }
        }
    }

    for (const auto& topic : topics_to_create_writers)
    {
        create_empty_datawriter_nts_(topic);
    }

    return utils::ReturnCode::RETCODE_OK;
}

void DynTypesPublicationParticipant::create_empty_datawriter_nts_(
        const core::types::DdsTopic& topic)
{
    auto topic_name = topic.topic_name();
    auto type_name = topic.type_name;

    // Check if topic already exist
    auto it_topic = writers_.find(topic);

    // If not, create it empty
    if (it_topic == writers_.end())
    {
        writers_[topic] = {nullptr, nullptr};
        it_topic = writers_.find(topic);
    }

    // If writer already created, finish function
    if (nullptr != it_topic->second.first)
    {
        return;
    }

    // Writer is not created
    // Check if type exists. If so, create writer, if not leave
    auto it_type = types_discovered_.find(type_name);
    if (it_type == types_discovered_.end())
    {
        return;
    }

    // Create topic
    fastdds::dds::Topic* dds_topic = dds_participant_->create_topic(
        topic.topic_name(),
        topic.type_name,
        default_topic_qos_(topic)
    );

    // Create writer
    writers_[topic] =
    {
        dds_topic,
        dds_publisher_->create_datawriter(
            dds_topic,
            default_empty_datawriter_qos_(topic))
    };

    if (type_name == "DdsRecorderCommand")
    {
        logError(DEBUG_COMMAND, "(DYN TYPES PUBLISHER) Created dynamic types writer for topic " << topic);
    }
    else
    {
        logInfo(DDSPIPE_DYNTYPES_DDSPARTICIPANT, "Created writer for topic " << topic);
    }
}

fastdds::dds::DataWriterQos
DynTypesPublicationParticipant::default_empty_datawriter_qos_(
    const core::types::DdsTopic& topic) noexcept
{
    // TODO decide which qos to use. Using less restrictive
    auto qos = fastdds::dds::DataWriterQos();
    qos.durability().kind =
        ( topic.topic_qos.is_transient_local() ?
            fastdds::dds::DurabilityQosPolicyKind::TRANSIENT_LOCAL_DURABILITY_QOS :
            fastdds::dds::DurabilityQosPolicyKind::VOLATILE_DURABILITY_QOS
        );
    qos.reliability().kind =
        ( topic.topic_qos.is_reliable() ?
            fastdds::dds::ReliabilityQosPolicyKind::RELIABLE_RELIABILITY_QOS :
            fastdds::dds::ReliabilityQosPolicyKind::BEST_EFFORT_RELIABILITY_QOS
        );

    return qos;
}

fastdds::dds::TopicQos
DynTypesPublicationParticipant::default_topic_qos_(
    const core::types::DdsTopic& topic) noexcept
{
    // TODO decide which qos to use
    return fastdds::dds::TopicQos();
}

utils::ReturnCode DynTypesPublicationParticipant::receive_type_object_(
        ddspipe::core::IRoutingData& data)
{
    // Assuming that data is of type required
    auto& dynamic_type_data = dynamic_cast<core::types::DynamicTypeData&>(data);
    return receive_type_object_(dynamic_type_data.dynamic_type);
}

} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
