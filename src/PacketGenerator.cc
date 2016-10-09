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
    //Number of Packets transmitted VS number of Packets received
    //PacketSink filename doesnt make sense to be
    //used here
    //std::string filename = "Data_Generate.txt";
    FILE * filePointerToWrite = fopen("Data_Generate.txt", "a");
    if (filePointerToWrite == NULL) return;

    // retrieve node position in the network (from parents)
    int nodeXPosition = getParentModule()->par("nodeXPosition");
    int nodeYPosition = getParentModule()->par("nodeYPosition");

    // retrieve node identifier from parent
    int nodeIdentifier = getParentModule()->par("nodeIdentifier");

    fprintf(filePointerToWrite, "Transmitter Module #%d\n", nodeIdentifier);
    fprintf(filePointerToWrite, "NumOfMessage Generated        Position(X.Y)\n");
    fprintf(filePointerToWrite, "%d,                           (%d,%d)\n",
            numOfPacketsGenerated, nodeXPosition, nodeYPosition);

}

void PacketGenerator::initialize()
{
    numOfPacketsGenerated = 0;
    // take parameters
    iatDistribution = par("iatDistribution");
    messageSize = par("messageSize");

    // get TransmitterNode's identifier
    senderId = getParentModule()->par("nodeIdentifier");

    // schedule the next transmission
    scheduleAt(simTime() + iatDistribution, new cMessage("SCHEDULE"));

    // reset the sequence number
    seqno = 0;
}

void PacketGenerator::handleMessage(cMessage *msg)
{
    // check the type of the message
    if (dynamic_cast<AppMessage *>(msg))
    {
        // discard incoming AppMessage
        delete msg;
    }

    // other messages
    else
    {
        // scheduled transmission
        if (strcmp(msg->getName(), "SCHEDULE") == 0)
        {
            // create a new AppMessage
            AppMessage * appMsg = generateMessage();

            // send the message immediately
            send(appMsg, "gate$o");

            // schedule the next transmission
            scheduleAt(simTime() + iatDistribution, new cMessage("SCHEDULE"));
        }

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

    // generate message name
    char msgname[32];
    snprintf(msgname, 32, "sender=%d seq=%d ts=%f", senderId, sequenceNumber, timeStamp.dbl());

    // create a new instance of AppMessage
    AppMessage * msg = new AppMessage(msgname);

    // configure the message
    msg->setTimeStamp(timeStamp);
    msg->setSenderId(senderId);
    msg->setSequenceNumber(sequenceNumber);
    msg->setMsgSize(msgSize);

    numOfPacketsGenerated++;

    return msg;
}

} //namespace

