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
#include <iostream>

namespace wsl_csma {

Define_Module(MAC);

MAC::MAC()
    : cSimpleModule()
{

}

MAC::~MAC()
{

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
}

void MAC::handleMessage(cMessage *msg)
{
    // check the type of the message
    // if the received packet is from Packet Generator
    if (AppMessage * appMsg = check_and_cast<AppMessage *>(msg))
    {
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
}

} //namespace
