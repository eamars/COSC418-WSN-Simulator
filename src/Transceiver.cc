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

#include "Transceiver.h"
#include "MacMessage_m.h"
#include "CSRequestMessage_m.h"
#include "CSResponseMessage_m.h"
#include "TransmissionRequestMessage_m.h"
#include "TransmissionConfirmMessage_m.h"
#include "SignalStartMessage_m.h"
#include "SignalStopMessage_m.h"

namespace wsl_csma {

Define_Module(Transceiver);

Transceiver::Transceiver()
    : cSimpleModule()
{

}

Transceiver::~Transceiver()
{

}

void Transceiver::initialize()
{
    // take parameters
    txPowerDBm = par("txPowerDBm");
    bitRate = par("bitRate");
    csThreshDBm = par("csThreshDBm");
    noisePowerDBm = par("noisePowerDBm");
    turnaroundTime = par("turnaroundTime");
    csTime = par("csTime");

    // initialize internal variable
    transceiverState = RX;
}

void Transceiver::handleMessage(cMessage *msg)
{
    // DEBUG::channel is free
    if (dynamic_cast<CSRequestMessage *>(msg))
    {
        delete msg;

        CSResponseMessage *csMsg = new CSResponseMessage("free");
        csMsg->setBusyChannel(false);

        send(csMsg, "gate1$o");

    }

    // State Machine
    switch (transceiverState)
    {
        case RX:
        {
            // phase 1
            // when TransmissionRequest arrived
            if (dynamic_cast<TransmissionRequestMessage *>(msg))
            {
                // decapsulate the mac message out of TransmissionRequest mesage
                TransmissionRequestMessage *trMsg = static_cast<TransmissionRequestMessage *>(msg);
                MacMessage *macMsg = static_cast<MacMessage *>(trMsg->decapsulate());

                // dispose TransmissionRequest message
                delete trMsg;

                // change the internal state to TX
                transceiverState = TX;

                // wait for the TurnaroundTime
                scheduleAt(simTime() + turnaroundTime, macMsg);
            }

            break;
        }

        case TX:
        {
            // when TransmissionRequest arrived, it response with status set to statusBusy
            if (dynamic_cast<TransmissionRequestMessage *>(msg))
            {
                // drop the request packet
                delete msg;

                // response with statusBusy
                TransmissionConfirmMessage * tcMsg = new TransmissionConfirmMessage("statusBusy");
                tcMsg->setStatus("statusBusy");

                send(tcMsg, "gate1$o");
            }

            // phase 2
            // when mac message is received from itself after the turnaround time
            else if (dynamic_cast<MacMessage *>(msg))
            {
                MacMessage *macMsg = static_cast<MacMessage *>(msg);

                // retrieve the packet length in bits from the mac packet
                int64_t packet_length = macMsg->getBitLength();

                // send a SignalStart message to the channel
                SignalStartMessage *startMsg = new SignalStartMessage;

                // copy critical information
                int nodeIdentifier = getParentModule()->par("nodeIdentifier");
                int nodeXPosition = getParentModule()->par("nodeXPosition");
                int nodeYPosition = getParentModule()->par("nodeYPosition");

                startMsg->setIdentifier(nodeIdentifier);
                startMsg->setTransmitPowerDBm(txPowerDBm);
                startMsg->setPositionX(nodeXPosition);
                startMsg->setPositionY(nodeYPosition);
                startMsg->setCollidedFlag(false);

                // encapsulate the mac message
                startMsg->encapsulate(macMsg);
                macMsg = nullptr;

                // send the message to the channel
                send(startMsg, "gate2$o");

                // wait for the end of the packet transmission
                scheduleAt(simTime() + packet_length / bitRate, new cMessage("PHASE_3"));
            }

            else
            {
                // phase 3
                // when the SignalStop message is received
                if (strcmp(msg->getName(), "PHASE_3") == 0)
                {
                    // dispose the message
                    delete msg;

                    SignalStopMessage *stopMsg = new SignalStopMessage;

                    // set identifier
                    int nodeIdentifier = getParentModule()->par("nodeIdentifier");
                    stopMsg->setIdentifier(nodeIdentifier);

                    // send the message to the channel
                    send(stopMsg, "gate2$o");

                    // wait for another turnaround time
                    scheduleAt(simTime() + turnaroundTime, new cMessage("PHASE_4"));
                }

                // phase 4
                // when PHASE 4 message is received
                else if (strcmp(msg->getName(), "PHASE_4") == 0)
                {
                    // dispose the message
                    delete msg;

                    // change the internal state to receive state
                    transceiverState = RX;

                    // send a message of type TransmissionConfirm to MAC module with status set to statusOK
                    // response with statusBusy
                    TransmissionConfirmMessage * tcMsg = new TransmissionConfirmMessage("statusOK");
                    tcMsg->setStatus("statusOK");

                    send(tcMsg, "gate1$o");
                }

                // other unknown message
                else
                {
                    delete msg;
                }
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

} //namespace
