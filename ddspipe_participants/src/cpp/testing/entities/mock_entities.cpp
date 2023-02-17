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

#include <ddspipe_participants/testing/entities/mock_entities.hpp>
#include <ddspipe_participants/writer/auxiliar/BlankWriter.hpp>
#include <ddspipe_participants/reader/auxiliar/BlankReader.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace testing {

MockParticipant::MockParticipant(const core::types::ParticipantId& id)
    : BlankParticipant(id)
{
    // Do nothing
}

std::shared_ptr<core::IWriter> MockParticipant::create_writer(
        const core::ITopic& topic)
{
    // If topic is not of kind required (mock) return nothing
    if (topic.internal_type_discriminator() != INTERNAL_TOPIC_TYPE_MOCK_TEST)
    {
        return std::make_shared<BlankWriter>();
    }

    // Look in case it already exists
    auto it = writers.find(topic.topic_unique_name());
    if (it != writers.end())
    {
        return it->second;
    }

    // Create a new one, store it and return it
    auto entity = std::make_shared<MockWriter>(id_);
    writers[topic.topic_unique_name()] = entity;

    return entity;
}

std::shared_ptr<core::IReader> MockParticipant::create_reader(
        const core::ITopic& topic)
{
    // If topic is not of kind required (mock) return nothing
    if (topic.internal_type_discriminator() != INTERNAL_TOPIC_TYPE_MOCK_TEST)
    {
        return std::make_shared<BlankReader>();
    }

    // Look in case it already exists
    auto it = readers.find(topic.topic_unique_name());
    if (it != readers.end())
    {
        return it->second;
    }

    // Create a new one, store it and return it
    auto entity = std::make_shared<MockReader>(id_);
    readers[topic.topic_unique_name()] = entity;

    return entity;
}

MockReader::MockReader(const core::types::ParticipantId& id)
    : BaseReader(id)
{
    // Do nothing
}

void MockReader::simulate_data_reception(MockRoutingData&& data)
{
    {
        // Lock access to queue
        std::lock_guard<utils::Atomicable<std::queue<MockRoutingData>>> _(data_queue_);
        data_queue_.push(std::move(data));
    }

    on_data_available_();
}

utils::ReturnCode MockReader::take_nts_(
        std::unique_ptr<core::IRoutingData>& data) noexcept
{
    // Lock access to queue
    std::lock_guard<utils::Atomicable<std::queue<MockRoutingData>>> _(data_queue_);

    if (data_queue_.empty())
    {
        return utils::ReturnCode::RETCODE_NO_DATA;
    }

    data.reset(new MockRoutingData(std::move(data_queue_.front())));
    data_queue_.pop();

    return utils::ReturnCode::RETCODE_OK;
}

MockWriter::MockWriter(const core::types::ParticipantId& id)
    : BaseWriter(id)
{
    // Do nothing
}

utils::ReturnCode MockWriter::write_nts_(
        core::IRoutingData& data) noexcept
{
    auto data_cast = dynamic_cast<MockRoutingData&>(data);

    {
        // Lock access to queue
        std::lock_guard<utils::Atomicable<std::queue<MockRoutingData>>> _(data_queue_);
        data_queue_.push(data_cast);
    }

    ++waiter_;
    return utils::ReturnCode::RETCODE_OK;
}

MockRoutingData MockWriter::wait_data()
{
    waiter_.wait_and_decrement();

    // Lock access to queue
    std::lock_guard<utils::Atomicable<std::queue<MockRoutingData>>> _(data_queue_);
    auto data = std::move(data_queue_.front());
    data_queue_.pop();
    return data;
}

core::types::TopicInternalTypeDiscriminator MockTopic::internal_type_discriminator() const noexcept
{
    return INTERNAL_TOPIC_TYPE_MOCK_TEST;
}

core::types::TopicInternalTypeDiscriminator MockRoutingData::internal_type_discriminator() const noexcept
{
    return INTERNAL_TOPIC_TYPE_MOCK_TEST;
}

bool MockRoutingData::operator==(const MockRoutingData& other) const
{
    return data == other.data;
}

std::ostream& operator <<(
        std::ostream& os,
        const MockRoutingData& t)
{
    os << "{" << t.data << "}";
    return os;
}

} /* namespace testing */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
