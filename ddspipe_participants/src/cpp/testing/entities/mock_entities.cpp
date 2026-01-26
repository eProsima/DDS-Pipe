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

MockParticipant::MockParticipant(
        const core::types::ParticipantId& id)
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

    // Block access to internal struct
    std::lock_guard<std::mutex> _(mutex_);

    // Look in case it already exists
    auto it = writers_.find(topic.topic_unique_name());
    if (it != writers_.end())
    {
        return it->second;
    }

    // Create a new one, store it and return it
    auto entity = std::make_shared<MockWriter>(id_);
    writers_[topic.topic_unique_name()] = entity;

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

    // Block access to internal struct
    std::lock_guard<std::mutex> _(mutex_);

    // Look in case it already exists
    auto it = readers_.find(topic.topic_unique_name());
    if (it != readers_.end())
    {
        return it->second;
    }

    // Create a new one, store it and return it
    auto entity = std::make_shared<MockReader>(id_);
    readers_[topic.topic_unique_name()] = entity;

    return entity;
}

std::size_t MockParticipant::n_writers() const
{
    std::lock_guard<std::mutex> _(mutex_);
    return writers_.size();
}

std::size_t MockParticipant::n_readers() const
{
    std::lock_guard<std::mutex> _(mutex_);
    return readers_.size();
}

std::shared_ptr<MockWriter> MockParticipant::get_writer(
        const core::ITopic& topic) const
{
    std::lock_guard<std::mutex> _(mutex_);
    auto it = writers_.find(topic.topic_unique_name());
    if (it == writers_.end())
    {
        return std::shared_ptr<MockWriter>();
    }
    return it->second;
}

std::shared_ptr<MockReader> MockParticipant::get_reader(
        const core::ITopic& topic) const
{
    std::lock_guard<std::mutex> _(mutex_);
    auto it = readers_.find(topic.topic_unique_name());
    if (it == readers_.end())
    {
        return std::shared_ptr<MockReader>();
    }
    return it->second;
}

MockReader::MockReader(
        const core::types::ParticipantId& id)
    : BaseReader(id)
{
    // Do nothing
}

void MockReader::simulate_data_reception(
        MockRoutingData&& data)
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

void MockReader::enable_nts_() noexcept
{
    // If there is data, notify it
    std::lock_guard<utils::Atomicable<std::queue<MockRoutingData>>> _(data_queue_);
    if (!data_queue_.empty())
    {
        on_data_available_();
    }
}

MockWriter::MockWriter(
        const core::types::ParticipantId& id)
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

std::size_t MockWriter::n_to_send_data()
{
    // Lock access to queue
    std::lock_guard<utils::Atomicable<std::queue<MockRoutingData>>> _(data_queue_);
    return data_queue_.size();
}

core::types::TopicInternalTypeDiscriminator MockTopic::internal_type_discriminator() const noexcept
{
    return INTERNAL_TOPIC_TYPE_MOCK_TEST;
}

utils::Heritable<core::ITopic> MockTopic::copy() const noexcept
{
    MockTopic topic = *this;
    return utils::Heritable<MockTopic>::make_heritable(topic);
}

core::types::TopicInternalTypeDiscriminator MockRoutingData::internal_type_discriminator() const noexcept
{
    return INTERNAL_TOPIC_TYPE_MOCK_TEST;
}

bool MockRoutingData::operator ==(
        const MockRoutingData& other) const
{
    return data == other.data;
}

bool MockFilterAllTopic::contains(
        const core::types::IFilterTopic& other) const
{
    return true;
}

bool MockFilterAllTopic::matches(
        const core::ITopic& topic) const
{
    return true;
}

std::ostream& MockFilterAllTopic::serialize(
        std::ostream& os) const
{
    os << "{MockFilterAllTopic}";
    return os;
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
