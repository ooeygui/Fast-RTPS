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
 * @file EDPServer.h
 *
 */

#ifndef EDPSERVER_H_
#define EDPSERVER_H_
#ifndef DOXYGEN_SHOULD_SKIP_THIS_PUBLIC

#include <fastrtps/rtps/common/CacheChange.h>

#include <fastrtps/rtps/builtin/data/ParticipantProxyData.h>

#include "EDPSimple.h"

#include <set>

namespace eprosima {
namespace fastrtps{
namespace rtps {

class EDPServerPUBListener;
class EDPServerSUBListener;

/**
 * Class EDPServer, implements the Endpoint Discovery Protocol for server participants
 * Inherits from EDPSimple class.
 *@ingroup DISCOVERY_MODULE
 */
class EDPServer : public EDPSimple
{
    friend class EDPServerPUBListener;
    friend class EDPServerSUBListener;

    typedef std::set<InstanceHandle_t> key_list;

    //! Keys to wipe out from WriterHistory because its related Participants have been removed
    key_list _PUBdemises, _SUBdemises;

    //! TRANSIENT or TRANSIENT_LOCAL durability;
    DurabilityKind_t _durability;

    public:

    /**
     * Constructor.
     * @param p Pointer to the PDP
     * @param part Pointer to the RTPSParticipantImpl
     */
    EDPServer(
        PDP* p,
        RTPSParticipantImpl* part,
        DurabilityKind_t durability_kind)
        : EDPSimple(p, part)
        , _durability(durability_kind)
    {
    }

    ~EDPServer() override {}

    /**
     * This method generates the corresponding change in the subscription writer and send it
     * to all known remote endpoints.
     * @param rdata Pointer to the ReaderProxyData object.
     * @return true if correct.
     */
    bool processLocalReaderProxyData(
        RTPSReader* reader,
        ReaderProxyData* rdata) override;
    /**
     * This method generates the corresponding change in the publciations writer and send it
     * to all known remote endpoints.
     * @param wdata Pointer to the WriterProxyData object.
     * @return true if correct.
     */
    bool processLocalWriterProxyData(
        RTPSWriter* writer,
        WriterProxyData* wdata) override;
    /**
     * This methods generates the change disposing of the local Reader and calls the unpairing
     * and removal methods of the base class.
     * @param R Pointer to the RTPSReader object.
     * @return True if correct.
     */
    bool removeLocalReader(RTPSReader*R) override;
    /**
     * This methods generates the change disposing of the local Writer and calls the unpairing
     * and removal methods of the base class.
     * @param W Pointer to the RTPSWriter object.
     * @return True if correct.
     */
    bool removeLocalWriter(RTPSWriter*W) override;

    /**
    * Some History data is flag for defer removal till every client
    * acknowledges reception
    * @return True if trimming must be done
    */
    inline bool pendingHistoryCleaning()
    {
        return !(_PUBdemises.empty() && _SUBdemises.empty());
    }

    //! Callback to remove unnecesary WriterHistory info
    bool trimPUBWriterHistory()
    {
        return trimWriterHistory<std::vector<WriterProxyData*>>(_PUBdemises,
            *publications_writer_.first, *publications_writer_.second, &ParticipantProxyData::m_writers);
    }

    bool trimSUBWriterHistory()
    {
        return trimWriterHistory<std::vector<ReaderProxyData*>>(_SUBdemises,
            *subscriptions_writer_.first, *subscriptions_writer_.second, &ParticipantProxyData::m_readers);
    }

    protected:

    /**
        * Add participant CacheChange_ts from reader to writer
        * @return True if successfully modified WriterHistory
        */
    bool addPublisherFromHistory( CacheChange_t& c)
    {
        return addEndpointFromHistory(*publications_writer_.first, *publications_writer_.second, c);
    }

    bool addSubscriberFromHistory( CacheChange_t& c)
    {
        return addEndpointFromHistory(*subscriptions_writer_.first, *subscriptions_writer_.second, c);
    }

    /**
    * Trigger the participant CacheChange_t removal system
    * @return True if successfully modified WriterHistory
    */
    void removePublisherFromHistory(const InstanceHandle_t&);
    void removeSubscriberFromHistory(const InstanceHandle_t&);

    private:

    /**
    * Callback to remove unnecesary WriterHistory info common implementation
    * @return True if trim is finished
    */
    template<class ProxyCont> 
    bool trimWriterHistory(
        key_list& _demises,
        StatefulWriter& writer,
        WriterHistory& history,
        ProxyCont ParticipantProxyData::* pCont);

    //! addPublisherFromHistory and addSubscriberFromHistory common implementation
    bool addEndpointFromHistory(
        StatefulWriter& writer,
        WriterHistory& history,
        CacheChange_t& c);

    /**
     * Create local SEDP Endpoints based on the DiscoveryAttributes.
     * @return True if correct.
     */
    virtual bool createSEDPEndpoints() override;

};

// Default configuration values for EDP entities.
 extern const Duration_t edp_heartbeat_period;
 extern const Duration_t edp_nack_response_delay;
 extern const Duration_t edp_nack_supression_duration;
 extern const Duration_t edp_heartbeat_response_delay;

 extern const int32_t edp_initial_reserved_caches;

}
} /* namespace rtps */
} /* namespace eprosima */

#endif
#endif /* EDPSERVER_H_ */
