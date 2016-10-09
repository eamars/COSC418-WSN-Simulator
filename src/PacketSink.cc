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

#include "PacketSink.h"
#include "AppMessage_m.h"
#include <iostream>

namespace wsl_csma {

Define_Module(PacketSink);

PacketSink::PacketSink()
    : cSimpleModule()
{

}

PacketSink::~PacketSink()
{
    //Number of Packets transmitted VS number of Packets received
    //PacketSink filename doesnt make sense to be
    //used here
    //std::string filename = "Data_Received.txt";
    FILE * filePointerToWrite = fopen("Data_Received.txt", "a");
    if (filePointerToWrite == NULL) return;

    // retrieve node position in the network (from parents)
    int nodeXPosition = getParentModule()->par("nodeXPosition");
    int nodeYPosition = getParentModule()->par("nodeYPosition");

    // retrieve node identifier from parent
    int nodeIdentifier = getParentModule()->par("nodeIdentifier");


    fprintf(filePointerToWrite, "Receiver Module #%d\n", nodeIdentifier);
    fprintf(filePointerToWrite, "NumOfMessage Received         Position(X.Y)\n");
    fprintf(filePointerToWrite, "%d,                           (%d,%d)\n",
            numOfPacketsReceived, nodeXPosition, nodeYPosition);
}

void PacketSink::initialize()
{
    numOfPacketsReceived = 0;
    // take parameters
    filename = par("filename").str();
}

void PacketSink::handleMessage(cMessage *msg)
{
    numOfPacketsReceived++;

    if (dynamic_cast<AppMessage *>(msg))
    {
        delete msg;
    }

    else
    {
        delete msg;
    }
}

} //namespace
