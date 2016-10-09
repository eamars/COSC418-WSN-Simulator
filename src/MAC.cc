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
#include "TransmissionRequestMessage_m.h"
#include "TransmissionConfirmMessage_m.h"
#include "TransmissionIndicationMessage_m.h"
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


    //std::string filename = "Data_Dropped_MAC.txt";
    FILE * filePointerToWrite = fopen("Data_Dropped_MAC.txt", "a");
    if (filePointerToWrite == NULL) return;

    // retrieve node position in the network (from parents)
    int nodeXPosition = getParentModule()->par("nodeXPosition");
    int nodeYPosition = getParentModule()->par("nodeYPosition");

    // retrieve node identifier from parent
    int nodeIdentifier = getParentModule()->par("nodeIdentifier");


    fprintf(filePointerToWrite, "TransceiverNode #           NumOfMessage Dropped-OverFlow          numOfPacketsDroppedTimeOut          Position(X.Y)\n");
    fprintf(filePointerToWrite, "%d,                         %d,                                    %d,                           %d,%d\n",
           nodeIdentifier, numOfPacketsDroppedOverFlow, numOfPacketsDroppedTimeOut, nodeXPosition, nodeYPosition);

    fclose(filePointerToWrite);
}

void MAC::initialize()
{
    numOfPacketsDroppedOverFlow = 0;
    numOfPacketsDroppedTimeOut = 0;
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
            numOfPacketsDroppedOverFlow++;
            delete appMsg;
        }

        // otherwise add packet into the buffer
        else
        {
            macBuffer.push_back(appMsg);
        }
    }

    // received carrier sensing response packet
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
                // schedule next event
                // send a dummy packet to itself
                scheduleAt(simTime() + backoffDistribution, new cMessage("CSMA_FAILED"));

                // wait for response
                MACState = CARRIER_SENSE_WAIT;
            }
            else
            {
                // cancel the schedule for current packet transmission
                AppMessage *appMsg = macBuffer.front();
                macBuffer.pop_front();
                numOfPacketsDroppedTimeOut++;
                delete appMsg;

                // cancel the current transmission
                MACState = IDLE;
            }


        }

        // otherwise transmit immediately
        else
        {
            MACState = TRANSMIT_START;
        }

        // destroy the response
        delete msg;
    }

    // received transmission confirm packet
    else if (dynamic_cast<TransmissionConfirmMessage *>(msg))
    {
        TransmissionConfirmMessage *tcMsg = static_cast<TransmissionConfirmMessage *>(msg);

        // status busy
        if (strcmp(tcMsg->getStatus(), "statusBusy") == 0)
        {
            // TODO: check
            // reset it's state to IDLE
            MACState = IDLE;
        }

        // status ok
        else if (strcmp(tcMsg->getStatus(), "statusOK") == 0)
        {
            // set the state to transmit done, we can pop off the packet at the front
            MACState = TRANSMIT_DONE;
        }

        // destroy the packet
        delete msg;
    }

    // received transmission indication message
    else if (dynamic_cast<TransmissionIndicationMessage *>(msg))
    {
        TransmissionIndicationMessage *tiMsg = static_cast<TransmissionIndicationMessage *>(msg);

        // decapsulate the message and pass to higher layer
        MacMessage *macMsg = static_cast<MacMessage *>(tiMsg->decapsulate());

        // decapsulate the appmessage
        AppMessage *appMsg = static_cast<AppMessage *>(macMsg->decapsulate());

        // pass it to higher layer
        send(appMsg, "gate1$o");

        delete macMsg;
        delete tiMsg;
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
        case TRANSMIT_START:
        {
            // CSResponse is received and the channel is clear to transmit
            // we already know there will be packet in the queue

            // extract the oldest message from buffer (create a deep copy)
            AppMessage *appMsg = new AppMessage(*macBuffer.front());

            // encapsulate it into a message mmsg of type MacMessage
            MacMessage *mmsg = new MacMessage;
            mmsg->encapsulate(appMsg);

            // encapsulate again into TransmissionRequest packet
            TransmissionRequestMessage *trMsg = new TransmissionRequestMessage;
            trMsg->encapsulate(mmsg);

            // nullify the pointers
            appMsg = nullptr;
            mmsg = nullptr;

            // transmit the MacMessage
            send(trMsg, "gate2$o");

            // wait for transmission confirm
            MACState = TRANSMIT_WAIT;

            break;
        }
        case TRANSMIT_WAIT:
        {
            // the process will trap here until TransmissionConfirm is received
            break;
        }
        case TRANSMIT_DONE:
        {
            AppMessage *appMsg = macBuffer.front();
            macBuffer.pop_front();

            delete appMsg;

            // advance to IDLE state
            MACState = IDLE;
        }

        default:
        {
            break;
        }
    }


}

} //namespace
