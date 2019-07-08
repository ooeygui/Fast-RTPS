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
 * @file TestPublisher.h
 *
 */

#ifndef _TEST_PUBLISHER_H_
#define _TEST_PUBLISHER_H_

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastdds/domain/DomainParticipant.hpp>
#include <fastdds/domain/DomainParticipantListener.hpp>
#include <fastdds/topic/DataWriterListener.hpp>
#include <fastdds/topic/DataWriter.hpp>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastdds/topic/TypeSupport.hpp>
#include <condition_variable>
#include <fastrtps/types/TypeObjectFactory.h>
#include <fastrtps/rtps/common/Types.h>
#include <string>

class TestPublisher {
public:
    TestPublisher();

    virtual ~TestPublisher();

    //!Initialize
    bool init(
        const std::string& topicName,
        int domain,
        eprosima::fastdds::TypeSupport type,
        const eprosima::fastrtps::types::TypeObject* type_object,
        const eprosima::fastrtps::types::TypeIdentifier* type_identifier,
        const eprosima::fastrtps::types::TypeInformation* type_info,
        const std::string& name,
        const eprosima::fastrtps::DataRepresentationQosPolicy* dataRepresentationQos);

    //!Publish a sample
    bool publish();

    //!Run for number samples
    void run();

    // Auxiliar test methods
    bool isInitialized() const { return m_bInitialized; }

    void waitDiscovery(bool expectMatch = true, int maxWait = 10);

    void waitTypeDiscovery(bool expectMatch = true, int maxWait = 10);

    void matched();

    bool isMatched() { return m_pubListener.n_matched > 0; }

    void send() { waitDiscovery(); publish(); }

    eprosima::fastrtps::types::DynamicType_ptr discovered_type() const
    {
        return disc_type_;
    }

private:
    std::string m_Name;

    eprosima::fastdds::TypeSupport m_Type;

    int m_iSamples;

    int m_sentSamples;

    int m_iWaitTime;

    void *m_Data;

    bool m_bInitialized;

    std::mutex m_mDiscovery;

    std::mutex mtx_type_discovery_;

    std::condition_variable m_cvDiscovery;

    std::condition_variable cv_type_discovery_;

    eprosima::fastdds::DomainParticipant* mp_participant;

    eprosima::fastdds::Publisher* mp_publisher;

    eprosima::fastdds::DataWriter* writer_;

    eprosima::fastrtps::types::DynamicType_ptr disc_type_;

    class PartListener : public eprosima::fastdds::DomainParticipantListener
    {
    public:
        PartListener(TestPublisher* parent) : parent_(parent), discovered_(false) {}
        ~PartListener() override {}

        void on_type_discovery(
                eprosima::fastdds::DomainParticipant* participant,
                const eprosima::fastrtps::string_255& topic,
                const eprosima::fastrtps::types::TypeIdentifier* identifier,
                const eprosima::fastrtps::types::TypeObject* object,
                eprosima::fastrtps::types::DynamicType_ptr dyn_type) override;

        TestPublisher* parent_;
        std::atomic<bool> discovered_;

    } part_listener_;

    class PubListener:public eprosima::fastdds::DataWriterListener
    {
    public:
        PubListener() {}
        PubListener(TestPublisher* parent);

        ~PubListener() override {}

        void on_publication_matched(
            eprosima::fastdds::DataWriter* writer,
            eprosima::fastrtps::rtps::MatchingInfo& info) override;

        TestPublisher* mParent;
        int n_matched;
    }m_pubListener;

    void runThread();
};



#endif /* _TEST_PUBLISHER_H_ */