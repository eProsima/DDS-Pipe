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

#include <cpp_utils/wait/CounterWaitHandler.hpp>
#include <cpp_utils/types/Atomicable.hpp>

#include <ddspipe_core/interface/IReader.hpp>
#include <ddspipe_core/interface/IWriter.hpp>
#include <ddspipe_core/interface/IParticipant.hpp>
#include <ddspipe_core/interface/ITopic.hpp>
#include <ddspipe_core/interface/IRoutingData.hpp>

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
class MockRoutingData;

class MockParticipant : public BlankParticipant
{
public:

    MockParticipant(const core::types::ParticipantId& id);

    std::shared_ptr<core::IWriter> create_writer(
            const core::ITopic& topic) override;

    std::shared_ptr<core::IReader> create_reader(
            const core::ITopic& topic) override;

    std::map<std::string, std::shared_ptr<MockWriter>> writers;
    std::map<std::string, std::shared_ptr<MockReader>> readers;
};

class MockReader : public BaseReader
{
public:

    MockReader(const core::types::ParticipantId& id);

    void simulate_data_reception(MockRoutingData&& data);

    utils::ReturnCode take_nts_(
            std::unique_ptr<core::IRoutingData>& data) noexcept override;

protected:

    utils::Atomicable<std::queue<MockRoutingData>> data_queue_;

};

class MockWriter : public BaseWriter
{
public:

    MockWriter(const core::types::ParticipantId& id);

    utils::ReturnCode write_nts_(
            core::IRoutingData& data) noexcept override;

    MockRoutingData wait_data();

protected:

    utils::event::CounterWaitHandler waiter_{0, 0, true};
    utils::Atomicable<std::queue<MockRoutingData>> data_queue_;
};

class MockTopic : public core::types::DistributedTopic
{
public:

    core::types::TopicInternalTypeDiscriminator internal_type_discriminator() const noexcept override;
};

struct MockRoutingData : public core::IRoutingData
{
public:

    bool operator==(const MockRoutingData& other) const;

    core::types::TopicInternalTypeDiscriminator internal_type_discriminator() const noexcept override;

    std::string data;
};

/**
 * Serialization method for \c MockRoutingData object.
 */
std::ostream& operator <<(
        std::ostream& os,
        const MockRoutingData& t);

const core::types::TopicInternalTypeDiscriminator INTERNAL_TOPIC_TYPE_MOCK_TEST = "testing::mock::v0";

} /* namespace testing */
} /* namespace participants */
} /* namespace ddspipe */
} /* namespace eprosima */
