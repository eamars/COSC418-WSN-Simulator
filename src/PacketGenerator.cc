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

#include "PacketGenerator.h"
#include "AppMessage_m.h"
#include <iostream>

namespace wsl_csma {

Define_Module(PacketGenerator);

PacketGenerator::PacketGenerator()
    : cSimpleModule()
{

}

PacketGenerator::~PacketGenerator()
{

}

void PacketGenerator::initialize()
{
    // take parameters
    iatDistribution = par("iatDistribution");
    messageSize = par("messageSize");

    // reset the sequence number
    seqno = 0;

    // get TransmitterNode's identifier
    senderId = getParentModule()->par("nodeIdentifier");

    // create a new message
    AppMessage * msg = generateMessage();

    // schedule the message immediately
    scheduleAt(simTime(), msg);
}

void PacketGenerator::handleMessage(cMessage *msg)
{
    // check the type of the message
    if (check_and_cast<AppMessage *>(msg))
    {
        // schedule the next transmission
        // draw a random number
        double interval = exponential(iatDistribution);

        // create a new message
        AppMessage * newMsg = generateMessage();

        scheduleAt(simTime() + interval, newMsg);
    }

    else
    {
        delete msg;
    }
}

AppMessage * PacketGenerator::generateMessage()
{
    // get current simulation time
    simtime_t timeStamp = simTime();

    // get sequence id
    int sequenceNumber = seqno++;

    // get message size
    int msgSize = messageSize;

    // create a new instance of AppMessage
    AppMessage * msg = new AppMessage;

    // configure the message
    msg->setTimeStamp(timeStamp);
    msg->setSenderId(senderId);
    msg->setSequenceNumber(sequenceNumber);
    msg->setMsgSize(msgSize);

    return msg;
}

} //namespace

