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
#include "TransmissionIndicationMessage_m.h"
#include "SignalStartMessage_m.h"
#include "SignalStopMessage_m.h"
#include "global.h"
#include "misc.h"

#include <iostream>
#include <cmath>

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
    // TODO: implement carrier sensing
    if (dynamic_cast<CSRequestMessage *>(msg))
    {
        delete msg;

        CSResponseMessage *csMsg = new CSResponseMessage("free");
        csMsg->setBusyChannel(false);

        send(csMsg, "gate1$o");

        return;
    }

    // Receive Path
    // process all the SignalStart and SignalStop message coming from the Channel, independent of
    // whether it is in the transmit or receive state
    if (dynamic_cast<SignalStartMessage *>(msg))
    {
        SignalStartMessage *startMsg = static_cast<SignalStartMessage *>(msg);

        // if at the time of arrival of the SignalStart message, the currentTransmission
        // list was not empty, then toggle the collidedFlag of newly received and all stored
        // in the currentTransmission list
        if (!currentTransmissions.empty())
        {
            startMsg->setCollidedFlag(true);
            markAllCollided();
        }

        // add SignalStart message to the currentTransmissions list
        updateCurrentTransmissions(startMsg);

        // remove the original start message
        delete msg;

        return;
    }

    if (dynamic_cast<SignalStopMessage *>(msg))
    {
        SignalStopMessage *stopMsg = static_cast<SignalStopMessage *>(msg);

        // when SignalStop message is received
        SignalStartMessage *startMsg = updateCurrentTransmissions(stopMsg);

        // we don't need SignalStop message anymore
        delete stopMsg;

        // if the collided flag is marked true, then drop the message and do no
        // further action
        if (startMsg->getCollidedFlag())
        {

        }

        // not collied
        else
        {
            // extract mac packet
            MacMessage *mpkt = static_cast<MacMessage *>(startMsg->decapsulate());

            // calculate the euclidean distance between two nodes
            int nodeXPosition = getParentModule()->par("nodeXPosition");
            int nodeYPosition = getParentModule()->par("nodeYPosition");

            int otherXPosition = startMsg->getPositionX();
            int otherYPosition = startMsg->getPositionY();

            double dist = sqrt((nodeXPosition - otherXPosition) * (nodeXPosition - otherXPosition) +
                    (nodeYPosition - otherYPosition) * (nodeYPosition - otherYPosition));

            // take account of path loss
            // if the distance is less than the reference distance d0, then there will be
            // no packet loss
            const double dist0 = 1.0;

            double path_loss_ratio = 1.0;

            // no packet loss
            if (dist < dist0)
            {
                path_loss_ratio = 1;
            }

            // loss channel
            else
            {
                path_loss_ratio = pow(dist, pathLossExponent);
            }

            // convert the path loss ratio into decibal
            // using dB = 10 * log10(path_loss_ratio)
            double path_loss_db = ratio_to_db(path_loss_ratio);

            // calculate the received power in dBm
            // using R_db = transmitPower - path_loss_dbm
            double received_power_db = startMsg->getTransmitPowerDBm() - path_loss_db;

            // get bit rate in dbm
            double bit_rate_db = ratio_to_db(bitRate);

            // hence we can calculate signal to noise ratio
            // using snr = receivedPower - (noisePower + bitRate)
            double snr_db = received_power_db - (noisePowerDBm + bit_rate_db);

            // calculate signal to noise ratio in normal domain
            double snr = db_to_ratio(snr_db);

            // calculate bit error rate
            // ref: http://stackoverflow.com/a/18786808/4444357
            double bit_error_rate = cef(sqrt(2 * snr));

            // calculate packet error rate
            // using PER = 1 - (1 - BER)^n
            // ref: https://en.wikipedia.org/wiki/Bit_error_rate
            int64_t packet_length = mpkt->getBitLength();
            double packet_error_rate = 1 - pow((1 - bit_error_rate), packet_length);

            // draw a uniformly distributed random number u between 0 and 1, if u < PER, then
            // top any further processing and drop the packet
            double u = uniform(0, 1);

            if (u < packet_error_rate)
            {
                std::cout << "Transceiver::packet dropped" << std::endl;
                delete mpkt;
            }
            else
            {
                // encapsulate into the message and deliver to higher layer
                TransmissionIndicationMessage * tiMsg = new TransmissionIndicationMessage;
                tiMsg->encapsulate(mpkt);

                send(tiMsg, "gate1$o");

                // dispose pointer
                mpkt = nullptr;
            }
        }

        // dispose the message
        delete startMsg;

        return;
    }

    // State Machine
    switch (transceiverState)
    {
        case RX:
        {
            // Transmit Path
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
            else
            {
                delete msg;
            }

            break;
        }

        case TX:
        {
            // Transmit Path
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

            // Transmit Path
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
                // Transmit Path
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

                // Transmit Path
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

void Transceiver::updateCurrentTransmissions(SignalStartMessage *startMsg)
{
    // traverse the list to see if packet with same identifier exists
    for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); it++)
    {
        // compare the identifier
        if ((*it)->getIdentifier() == startMsg->getIdentifier())
        {
            // TODO: throw an error
            std::cout << "Transceiver::updateCurrentTransmissions<SignalStartMessage *>::ERROR" << std::endl;

            return;
        }
    }

    // if not found, then append at the end (create a deep copy)
    currentTransmissions.push_back(new SignalStartMessage(*startMsg));
}
SignalStartMessage * Transceiver::updateCurrentTransmissions(SignalStopMessage *stopMsg)
{
    for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); it++)
    {
        // compare the identifier
        if ((*it)->getIdentifier() == stopMsg->getIdentifier())
        {
            // get the message from list
            SignalStartMessage *startMsg = *it;

            // remove the message from list
            currentTransmissions.erase(it);

            return startMsg;
        }
    }

    // TODO: throw an error
    std::cout << "Transceiver::updateCurrentTransmissions<SignalStopMessage *>::ERROR" << std::endl;

    return NULL;
}

void Transceiver::markAllCollided()
{
    for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); it++)
    {
        (*it)->setCollidedFlag(true);
    }
}

} //namespace
