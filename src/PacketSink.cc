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
#include "global.h"
#include "misc.h"

#include <iostream>
#include <cmath>
#include <ctgmath>
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


    fprintf(filePointerToWrite, "ReceiverNode #           NumOfMessage Received         Position(X.Y)\n");
    fprintf(filePointerToWrite, "%d,                      %d,                           %d,%d\n",
            nodeIdentifier, numOfPacketsReceived, nodeXPosition, nodeYPosition);

    fclose(filePointerToWrite);

    //Start writing our message into log files
    FILE * MessageLogFilePointer = fopen("Message_PacketSink.txt", "a");

    for(int i = 0; i < numOfPacketsReceived; i++){
        if(numOfPacketsReceived >= circBuffSize) break;
        AppMessage *appMsg = sinkBuffer[i];
        //we can perfect the formatting later...
            fprintf(MessageLogFilePointer, "%d,                  %d,       %f,        %d,        %d\n",
                   numOfPacketsReceived, appMsg->msgSize, SIMTIME_DBL(appMsg->timeStamp), appMsg->senderId, appMsg->sequenceNumber);
    }
    fclose(MessageLogFilePointer);


}

void PacketSink::initialize()
{
    //save memory for heavy-loaded testing
    circBuffSize = 10000;
    writeIndex = 0;
    numOfPacketsReceived = 0;
    FILE * MessageLogFilePointer = fopen("Message_PacketSink.txt", "a");
    if (MessageLogFilePointer != NULL)
    {

            fprintf(MessageLogFilePointer, "NumOfMessage Received      msgSize      timeStamp      SenderID      sequenceNumber\n");
            fclose(MessageLogFilePointer);
    }
    // take parameters
    filename = par("filename").str();
}

void PacketSink::handleMessage(cMessage *msg)
{

    numOfPacketsReceived++;
    if (dynamic_cast<AppMessage *>(msg))
    {
        //File pointer CAN NOT be handled here
        AppMessage *appMsg = static_cast<AppMessage *>(msg);

        if (writeIndex < circBuffSize){
            sinkBuffer[writeIndex] = appMsg;
        }
        else{
            //Circulate back to the start
            writeIndex = 0;
            }

        writeIndex++;
        delete msg;
    }

    else
    {
        //It should never reach here
        delete msg;
    }
}

} //namespace
