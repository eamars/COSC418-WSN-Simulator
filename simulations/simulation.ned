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

package wsl_csma.simulations;

import wsl_csma.TransmitterNode;
import wsl_csma.ReceiverNode;
import wsl_csma.Channel;

network Simulation
{
	parameters:
		int numTransmitters = default(1);
	submodules:
		TransmitterNodeModule[numTransmitters] : TransmitterNode;
		ReceiverNodeModule : ReceiverNode;
		ChannelModule : Channel;
	connections allowunconnected:
		// connect receiver to the channel
		ChannelModule.gate++ <--> ReceiverNodeModule.gate;
		
		// connect transmitters to the channel
		for i=0..numTransmitters - 1 
		{
			ChannelModule.gate++ <--> TransmitterNodeModule[i].gate;
		}
}