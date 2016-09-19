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

namespace wsl_csma {

Define_Module(Transceiver);

Transceiver::Transceiver()
    : cSimpleModule()
{

}

Transceiver::~Transceiver()
{
    // take parameters
    txPowerDBm = par("txPowerDBm");
    bitRate = par("bitRate");
    csThreshDBm = par("csThreshDBm");
    noisePowerDBm = par("noisePowerDBm");
    turnaroundTime = par("turnaroundTime");
    csTime = par("csTime");

    // initialize internal variable
    transceiverState = IDLE;
}

void Transceiver::initialize()
{
    // TODO - Generated method body
}

void Transceiver::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

} //namespace
