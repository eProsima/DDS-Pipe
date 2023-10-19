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

#pragma once

#include <mutex>
#include <queue>

#include <cpp_utils/memory/Heritable.hpp>
#include <cpp_utils/types/Atomicable.hpp>
#include <cpp_utils/wait/CounterWaitHandler.hpp>

#include <ddspipe_core/interface/IReader.hpp>
#include <ddspipe_core/interface/IWriter.hpp>
#include <ddspipe_core/interface/IParticipant.hpp>
#include <ddspipe_core/interface/ITopic.hpp>
#include <ddspipe_core/interface/IRoutingData.hpp>
#include <ddspipe_core/types/topic/filter/IFilterTopic.hpp>

#include <ddspipe_participants/participant/auxiliar/BlankParticipant.hpp>
#include <ddspipe_participants/reader/auxiliar/BaseReader.hpp>
#include <ddspipe_participants/writer/auxiliar/BaseWriter.hpp>

namespace eprosima {
namespace ddspipe {
namespace participants {
namespace testing {

// Forward declarations
class MockParticipant;
class MockReader;
class MockWriter;
class MockTopic;
struct MockRoutingData;

class MockParticipant : public BlankParticipant
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    MockParticipant(
            const core::types::ParticipantId& id);

    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<core::IWriter> create_writer(
            const core::ITopic& topic) override;

    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<core::IReader> create_reader(
            const core::ITopic& topic) override;

    DDSPIPE_PARTICIPANTS_DllAPI
    std::size_t n_writers() const;

    DDSPIPE_PARTICIPANTS_DllAPI
    std::size_t n_readers() const;

    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<MockWriter> get_writer(
            const core::ITopic& topic) const;

    DDSPIPE_PARTICIPANTS_DllAPI
    std::shared_ptr<MockReader> get_reader(
            const core::ITopic& topic) const;

protected:

    std::map<std::string, std::shared_ptr<MockWriter>> writers_;
    std::map<std::string, std::shared_ptr<MockReader>> readers_;

    mutable std::mutex mutex_;
};

/**
 * @note this should be declared before queue in readers and writers for windows sake (C2027)
 */
struct MockRoutingData : public core::IRoutingData
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    bool operator ==(
            const MockRoutingData& other) const;

    DDSPIPE_PARTICIPANTS_DllAPI
    core::types::TopicInternalTypeDiscriminator internal_type_discriminator() const noexcept override;

    std::string data;
};

class MockReader : public BaseReader
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    MockReader(
            const core::types::ParticipantId& id);

    DDSPIPE_PARTICIPANTS_DllAPI
    void simulate_data_reception(
            MockRoutingData&& data);

    DDSPIPE_PARTICIPANTS_DllAPI
    utils::ReturnCode take_nts_(
            std::unique_ptr<core::IRoutingData>& data) noexcept override;

protected:

    virtual void enable_nts_() noexcept override;

    utils::Atomicable<std::queue<MockRoutingData>> data_queue_;

};

class MockWriter : public BaseWriter
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    MockWriter(
            const core::types::ParticipantId& id);

    DDSPIPE_PARTICIPANTS_DllAPI
    utils::ReturnCode write_nts_(
            core::IRoutingData& data) noexcept override;

    DDSPIPE_PARTICIPANTS_DllAPI
    MockRoutingData wait_data();

    DDSPIPE_PARTICIPANTS_DllAPI
    std::size_t n_to_send_data();

protected:

    utils::event::CounterWaitHandler waiter_{0, 0, true};
    utils::Atomicable<std::queue<MockRoutingData>> data_queue_;
};

class MockTopic : public core::types::DistributedTopic
{
public:

    DDSPIPE_PARTICIPANTS_DllAPI
    core::types::TopicInternalTypeDiscriminator internal_type_discriminator() const noexcept override;

    DDSPIPE_PARTICIPANTS_DllAPI
    virtual utils::Heritable<core::ITopic> copy() const noexcept override;
};

class MockFilterAllTopic : public core::types::IFilterTopic
{
public:

    virtual bool contains(
            const core::types::IFilterTopic& other) const;

    virtual bool matches(
            const core::ITopic& topic) const;

    virtual std::ostream& serialize(
            std::ostream& os) const;
};

/**
 * Serialization method for \c MockRoutingData object.
 */
DDSPIPE_PARTICIPANTS_DllAPI
std::ostream& operator <<(
        std::ostream& os,
        const MockRoutingData& t);

const core::types::TopicInternalTypeDiscriminator INTERNAL_TOPIC_TYPE_MOCK_TEST = "testing::mock::v0";

} /* namespace testing */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
