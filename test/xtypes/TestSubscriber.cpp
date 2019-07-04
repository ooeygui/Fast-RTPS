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
 * @file TestSubscriber.cpp
 *
 */

#include "TestSubscriber.h"
#include <fastdds/domain/DomainParticipantFactory.hpp>
#include <fastdds/domain/DomainParticipant.hpp>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/transport/UDPv4TransportDescriptor.h>
#include <fastrtps/transport/TCPv4TransportDescriptor.h>
#include <fastrtps/transport/UDPv6TransportDescriptor.h>
#include <fastrtps/transport/TCPv6TransportDescriptor.h>
#include <fastdds/subscriber/Subscriber.hpp>
#include <fastdds/topic/DataReader.hpp>
#include <fastrtps/utils/IPLocator.h>
#include <gtest/gtest.h>


#include <fastrtps/Domain.h>

using namespace eprosima::fastdds;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

TestSubscriber::TestSubscriber()
    : mp_participant(nullptr)
    , mp_subscriber(nullptr)
    , m_bInitialized(false)
    , m_subListener(this)
{
}

bool TestSubscriber::init(
        const std::string& topicName,
        int domain,
        eprosima::fastrtps::rtps::TopicKind_t topic_kind,
        eprosima::fastrtps::TopicDataType* type,
        const eprosima::fastrtps::types::TypeObject* type_object,
        const eprosima::fastrtps::types::TypeIdentifier* type_identifier,
        const eprosima::fastrtps::types::TypeInformation* type_info,
        const std::string& name,
        const eprosima::fastrtps::DataRepresentationQosPolicy* dataRepresentationQos,
        const eprosima::fastrtps::TypeConsistencyEnforcementQosPolicy* typeConsistencyQos)
{
    m_Name = name;
    m_Type = type;
    ParticipantAttributes PParam;
    PParam.rtps.builtin.domainId = domain;
    PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
    PParam.rtps.builtin.leaseDuration_announcementperiod = Duration_t(1, 0);
    PParam.rtps.setName(m_Name.c_str());

    mp_participant = DomainParticipantFactory::get_instance()->create_participant(PParam);
    if(mp_participant==nullptr)
    {
        return false;
    }

    //CREATE THE SUBSCRIBER
    SubscriberAttributes Rparam;
    Rparam.topic.topicKind = topic_kind;
    Rparam.topic.topicDataType = m_Type != nullptr ? m_Type->getName() : nullptr;
    Rparam.topic.auto_fill_xtypes = false;

    //REGISTER THE TYPE
    if (m_Type != nullptr)
    {
        mp_participant->register_type(m_Type);
    }

    Rparam.topic.topicName = topicName;
    if (type_object != nullptr)
    {
        Rparam.topic.type = *type_object;
    }
    if (type_identifier != nullptr)
    {
        Rparam.topic.type_id = *type_identifier;
    }
    if (type_info != nullptr)
    {
        Rparam.topic.type_information = *type_info;
    }

    if (typeConsistencyQos != nullptr)
    {
        Rparam.qos.type_consistency = *typeConsistencyQos;
    }
    if (dataRepresentationQos != nullptr)
    {
        Rparam.qos.representation = *dataRepresentationQos;
    }

    mp_subscriber = mp_participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, Rparam, nullptr);

    if (mp_subscriber == nullptr)
    {
        return false;
    }

    reader_ = mp_subscriber->create_datareader(Rparam.topic, Rparam.qos, &m_subListener);

    if (m_Type != nullptr)
    {
        m_Data = m_Type->createData();
    }

    m_bInitialized = true;

    return true;
}

TestSubscriber::~TestSubscriber()
{
    if (m_Type != nullptr)
    {
        m_Type->deleteData(m_Data);
    }
}

TestSubscriber::SubListener::SubListener(TestSubscriber* parent)
    : mParent(parent)
    , n_matched(0)
    , n_samples(0)
{
}

void TestSubscriber::waitDiscovery(bool expectMatch, int maxWait)
{
    std::unique_lock<std::mutex> lock(m_mDiscovery);

    if(m_subListener.n_matched == 0)
        m_cvDiscovery.wait_for(lock, std::chrono::seconds(maxWait));

    if (expectMatch)
    {
        ASSERT_GE(m_subListener.n_matched, 1);
    }
    else
    {
        ASSERT_EQ(m_subListener.n_matched, 0);
    }
}

void TestSubscriber::matched(bool unmatched)
{
    std::unique_lock<std::mutex> lock(m_mDiscovery);
    if (unmatched)
    {
        --m_subListener.n_matched;
    }
    else
    {
        ++m_subListener.n_matched;
    }
    if(m_subListener.n_matched >= 1)
        m_cvDiscovery.notify_one();
}

void TestSubscriber::SubListener::on_subscription_matched(
        eprosima::fastdds::DataReader*,
        eprosima::fastrtps::rtps::MatchingInfo& info)
{
    if(info.status == MATCHED_MATCHING)
    {
        mParent->matched();
        std::cout << mParent->m_Name << " matched."<<std::endl;
    }
    else
    {
        mParent->matched(true);
        std::cout << mParent->m_Name << " unmatched."<<std::endl;
    }
}

void TestSubscriber::SubListener::on_data_available(
        eprosima::fastdds::DataReader* reader)
{
    if (reader->take_next_sample(mParent->m_Data, &m_info))
    {
        if (m_info.sampleKind == ALIVE)
        {
            ++n_samples;
            mParent->cv_.notify_one();
            //std::cout << mParent->m_Name << " received a total of " << n_samples << " samples." << std::endl;
        }
    }
}

void TestSubscriber::run()
{
    std::cout << m_Name << " running..." << std::endl;
    std::cin.ignore();
}
