//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __WSL_CSMA_MAC_H_
#define __WSL_CSMA_MAC_H_

#include <omnetpp.h>
#include <deque>
#include "AppMessage_m.h"

using namespace omnetpp;

namespace wsl_csma {

/**
 * TODO - Generated class
 */
class MAC : public cSimpleModule
{
public:
    MAC();
    ~MAC();

protected:
    typedef enum
    {
        IDLE = 0,
        CARRIER_SENSE_RETRY,
        CARRIER_SENSE_WAIT,
        TRANSMIT
    } MACState_t;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

protected:
    int bufferSize;
    int maxBackoffs;
    double backoffDistribution;

    // internal variables
    std::deque<AppMessage *> macBuffer;
    int backoffCounter;
    MACState_t MACState;
};

} //namespace

#endif
