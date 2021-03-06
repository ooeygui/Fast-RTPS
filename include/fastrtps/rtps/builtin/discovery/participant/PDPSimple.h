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
 * @file PDPSimple.h
 *
 */

#ifndef PDPSIMPLE_H_
#define PDPSIMPLE_H_
#ifndef DOXYGEN_SHOULD_SKIP_THIS_PUBLIC


#include "PDP.h"


namespace eprosima {
namespace fastrtps{
namespace rtps {

class StatelessWriter;
class StatelessReader;

/**
 * Class PDPSimple that implements the SimpleRTPSParticipantDiscoveryProtocol as defined in the RTPS specification.
 *@ingroup DISCOVERY_MODULE
 */
class PDPSimple : public PDP
{
    public:
    /**
     * Constructor
     * @param builtin Pointer to the BuiltinProcols object.
     */
    PDPSimple(BuiltinProtocols* builtin);
    ~PDPSimple();

    void initializeParticipantProxyData(ParticipantProxyData* participant_data) override;

    /**
     * Initialize the PDP.
     * @param part Pointer to the RTPSParticipant.
     * @return True on success
     */
    bool initPDP(RTPSParticipantImpl* part) override;

    /**
     * Creates an initializes a new participant proxy from a DATA(p) raw info
     * @param ParticipantProxyData from DATA msg deserialization
     * @param CacheChange_t from DATA msg
     * @return new ParticipantProxyData * or nullptr on failure
     */
    ParticipantProxyData* createParticipantProxyData(
        const ParticipantProxyData&,
        const CacheChange_t&) override;

    /**
     * Some PDP classes require EDP matching with update PDP DATAs like EDPStatic
     * @return true if EDP endpoinst must be match
     */
    bool updateInfoMatchesEDP() override;

    /**
     * Force the sending of our local DPD to all remote RTPSParticipants and multicast Locators.
     * @param new_change If true a new change (with new seqNum) is created and sent; if false the last change is re-sent
     * @param dispose Sets change kind to NOT_ALIVE_DISPOSED_UNREGISTERED
     */
    void announceParticipantState(
        bool new_change,
        bool dispose = false,
        WriteParams& wparams = WriteParams::WRITE_PARAM_DEFAULT) override;

    /**
     * This method assigns remote endpoints to the builtin endpoints defined in this protocol. It also calls the corresponding methods in EDP and WLP.
     * @param pdata Pointer to the RTPSParticipantProxyData object.
     */
    void assignRemoteEndpoints(ParticipantProxyData* pdata) override;

    void removeRemoteEndpoints(ParticipantProxyData * pdata) override;

    void notifyAboveRemoteEndpoints(const ParticipantProxyData& pdata) override;

    /**
     * Activate a new Remote Endpoint that has been statically discovered.
     * @param pguid GUID_t of the participant.
     * @param userDefinedId User Defined ID.
     * @param kind Kind of endpoint.
     */
    bool newRemoteEndpointStaticallyDiscovered(
        const GUID_t& pguid,
        int16_t userDefinedId,
        EndpointKind_t kind);

    private:

    /**
     * Create the SPDP Writer and Reader
     * @return True if correct.
     */
    bool createPDPEndpoints() override;

};

}
} /* namespace rtps */
} /* namespace eprosima */
#endif
#endif /* PDPSIMPLE_H_ */
