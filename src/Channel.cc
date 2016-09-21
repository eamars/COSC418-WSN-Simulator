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

#include <Channel.h>
#include <iostream>
#include "SignalMessage_m.h"

namespace wsl_csma {

Define_Module(Channel);

void Channel::initialize()
{
    // since we are using inout gate, each gate have input and output
    // we need to take half number of the gates from the counter
    numGates = gateCount() / 2;
}

void Channel::handleMessage(cMessage *msg)
{
    // take messages of type SignalStart or SignalEnd from any Transceiver and sends copies
    // of these to each attached transceiver (including the sending one)
    if (check_and_cast<SignalMessage *>(msg))
    {
        // broadcast the message
        for (int i = 0; i < numGates; i++)
        {
            // create a deep copy of the message
            cMessage * newMsg = new cMessage(*msg);

            // distribute to all stations that connect to the same channel
            send(newMsg, "gate", i);
        }
    }

}

} //namespace
