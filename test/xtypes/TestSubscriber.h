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

/**
 * @file TestSubscriber.h
 *
 */

#ifndef _TEST_SUBSCRIBER_H_
#define _TEST_SUBSCRIBER_H_

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastdds/domain/DomainParticipant.hpp>
#include <fastdds/domain/DomainParticipantListener.hpp>
#include <fastdds/topic/DataReaderListener.hpp>
#include <fastdds/topic/DataReader.hpp>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastdds/topic/TypeSupport.hpp>
#include <fastrtps/types/TypeObjectFactory.h>
#include <fastrtps/rtps/common/Types.h>

#include <condition_variable>
#include <atomic>

class TestSubscriber
{
public:
    TestSubscriber();

    virtual ~TestSubscriber();

    //!Initialize the subscriber
    bool init(
        const std::string& topicName,
        int domain,
        eprosima::fastrtps::rtps::TopicKind_t topic_kind,
        eprosima::fastdds::TypeSupport type,
        const eprosima::fastrtps::types::TypeObject* type_object,
        const eprosima::fastrtps::types::TypeIdentifier* type_identifier,
        const eprosima::fastrtps::types::TypeInformation* type_info,
        const std::string& name,
        const eprosima::fastrtps::DataRepresentationQosPolicy* dataRepresentationQos,
        const eprosima::fastrtps::TypeConsistencyEnforcementQosPolicy* typeConsistencyQos);

    //!RUN the subscriber
    void run();

    // Auxiliar test methods
    bool isInitialized() const { return m_bInitialized; }
    void waitDiscovery(bool expectMatch = true, int maxWait = 10);
    void waitTypeDiscovery(bool expectMatch = true, int maxWait = 10);
    void matched(bool unmatched = false);
    bool isMatched() { return m_subListener.n_matched > 0; }
    uint32_t samplesReceived() { return m_subListener.n_samples; }

    void block(std::function<bool()> checker)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, checker);
    }

    size_t block_for_at_least(size_t at_least)
    {
        block([this, at_least]() -> bool {
                return samplesReceived() >= at_least;
                });
        return samplesReceived();
    }

    eprosima::fastrtps::types::DynamicType_ptr discovered_type() const
    {
        return disc_type_;
    }

    bool register_discovered_type();

    eprosima::fastdds::DataReader* create_datareader();

    void delete_datareader(eprosima::fastdds::DataReader* reader);

private:
    std::string m_Name;
    eprosima::fastdds::TypeSupport m_Type;
    eprosima::fastdds::DomainParticipant* mp_participant;
    eprosima::fastdds::Subscriber* mp_subscriber;
    eprosima::fastdds::DataReader* reader_;
    void *m_Data;
    bool m_bInitialized;
    std::mutex m_mDiscovery;
    std::mutex mtx_type_discovery_;
    std::mutex mutex_;
    std::condition_variable m_cvDiscovery;
    std::condition_variable cv_type_discovery_;
    std::condition_variable cv_;
    eprosima::fastrtps::types::DynamicType_ptr disc_type_;
    eprosima::fastrtps::TopicAttributes topic_att;
    eprosima::fastrtps::ReaderQos reader_qos;

public:
    class PartListener : public eprosima::fastdds::DomainParticipantListener
    {
    public:
        PartListener(TestSubscriber* parent) : parent_(parent), discovered_(false) {}
        ~PartListener() override {}

        void on_type_discovery(
                eprosima::fastdds::DomainParticipant* participant,
                const eprosima::fastrtps::string_255& topic,
                const eprosima::fastrtps::types::TypeIdentifier* identifier,
                const eprosima::fastrtps::types::TypeObject* object,
                eprosima::fastrtps::types::DynamicType_ptr dyn_type) override;

        TestSubscriber* parent_;
        std::atomic<bool> discovered_;

    } part_listener_;

    class SubListener :public eprosima::fastdds::DataReaderListener
    {
    public:
        SubListener() {}
        SubListener(TestSubscriber* parent);

        ~SubListener() override {}

        void on_subscription_matched(
                eprosima::fastdds::DataReader* reader,
                eprosima::fastrtps::rtps::MatchingInfo& info) override;

        void on_data_available(eprosima::fastdds::DataReader* reader) override;

        TestSubscriber* mParent;
        eprosima::fastrtps::SampleInfo_t m_info;
        int n_matched;
        uint32_t n_samples;
    }m_subListener;
};

#endif /* _TEST_SUBSCRIBER_H_ */
