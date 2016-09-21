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

#include "MAC.h"
#include "AppMessage_m.h"
#include "MacMessage_m.h"
#include "CSRequestMessage_m.h"
#include "CSResponseMessage_m.h"
#include <iostream>

namespace wsl_csma {

Define_Module(MAC);

MAC::MAC()
    : cSimpleModule()
{

}

MAC::~MAC()
{
    // dump all packets in the queue
    while (macBuffer.size() > 0)
    {
        AppMessage *appMsg = macBuffer.front();
        macBuffer.pop_front();
        delete appMsg;
    }
}

void MAC::initialize()
{
    // take parameters
    bufferSize = par("bufferSize");
    maxBackoffs = par("maxBackoffs");
    backoffDistribution = par("backoffDistribution");

    // local variables
    // initialize the local packet buffer
    macBuffer.resize(bufferSize);
    macBuffer.clear();

    // reset the backoff counter
    backoffCounter = 0;

    // reset the MAC state
    MACState = IDLE;
}

void MAC::handleMessage(cMessage *msg)
{
    // check the type of the message
    // if the received packet is from Packet Generator
    if (dynamic_cast<AppMessage *>(msg))
    {
        AppMessage *appMsg = static_cast<AppMessage *>(msg);

        // put packet into the macBuffer
        // if the buffer is full, then drop the packet
        if (macBuffer.size() == (size_t) bufferSize)
        {
            delete appMsg;
        }

        // otherwise add packet into the buffer
        else
        {
            macBuffer.push_back(appMsg);
        }
    }

    // if the received packet is from Transceiver
    else if (dynamic_cast<CSResponseMessage *>(msg))
    {
        CSResponseMessage *csMsg = static_cast<CSResponseMessage *>(msg);

        // the channel is busy
        if (csMsg->getBusyChannel())
        {
            // increase the backoffCounter
            backoffCounter++;

            // std::cout << "Retry:" << backoffCounter << std::endl;

            // test if the counter has reached the maximum value
            if (backoffCounter < maxBackoffs)
            {
                // wait for a random time
                double interval = exponential(backoffDistribution);

                // schedule next event
                // send a dummy packet to itself
                scheduleAt(simTime() + interval, new cMessage("CSMA_FAILED"));

                // wait for response
                MACState = CARRIER_SENSE_WAIT;
            }
            else
            {
                // cancel the schedule for current packet transmission
                macBuffer.pop_front();

                // cancel the current transmission
                MACState = IDLE;
            }


        }

        // otherwise transmit immediately
        else
        {
            MACState = TRANSMIT;
        }

        // destroy the response
        delete msg;
    }

    // other packets
    else
    {
        // dummy packet for next carrier sensing procedure
        if (strcmp(msg->getName(), "CSMA_FAILED") == 0)
        {
            MACState = CARRIER_SENSE_RETRY;
        }

        // destroy the received packet
        delete msg;
    }

    // CSMA procedure
    switch (MACState)
    {
        // the MAC module will stay in IDLE state if there is no packet in the macBuffer
        case IDLE:
        {
            // if there are packets in the macBuffer, then starts the MAC process.
            if (!macBuffer.empty())
            {
                // reset the backoff counter
                backoffCounter = 0;

                // start the carrier sensing procedure
                // send a message of type CSRequest to the Transceiver
                CSRequestMessage *csMsg = new CSRequestMessage;

                send(csMsg, "gate2$o");

                // advance to next state
                MACState = CARRIER_SENSE_WAIT;
            }
            break;
        }
        case CARRIER_SENSE_RETRY:
        {
            // start the carrier sensing procedure
            // send a message of type CSRequest to the Transceiver
            CSRequestMessage *csMsg = new CSRequestMessage;

            send(csMsg, "gate2$o");

            // advance to next state
            MACState = CARRIER_SENSE_WAIT;

            break;
        }
        case CARRIER_SENSE_WAIT:
        {
            // the process will trap here until CSResponse is received
            break;
        }
        case TRANSMIT:
        {
            // CSResponse is received and the channel is clear to transmit
            // we already know there will be packet in the queue

            // extract the oldest message from buffer
            AppMessage *appMsg = macBuffer.front();
            macBuffer.pop_front();

            // encapsulate it into a message mmsg of type MacMessage
            MacMessage *mmsg = new MacMessage;
            mmsg->encapsulate(appMsg);

            // nullify the pointer
            appMsg = nullptr;

            // transmit the MacMessage
            send(mmsg, "gate2$o");

            // back to the IDLE state
            MACState = IDLE;

            break;
        }
        default:
        {
            break;
        }
    }


}

} //namespace
