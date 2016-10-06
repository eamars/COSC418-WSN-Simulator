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
		int numTransmitters;
		int radius;
		
		@display("bgb=0,0");
	submodules:
		TransmitterNodeModule[numTransmitters] : TransmitterNode
		{
			nodeIdentifier = index;	
			nodeXPosition = radius * cos(2 * 3.14159265358979323846 * index / (numTransmitters));
			nodeYPosition = radius * sin(2 * 3.14159265358979323846 * index / (numTransmitters));
			
			@display("p=$nodeXPosition,$nodeYPosition");
		};
		ReceiverNodeModule : ReceiverNode
		{
			nodeIdentifier = -1;
			nodeXPosition = 0;
			nodeYPosition = 0;
			
			@display("p=$nodeXPosition,$nodeYPosition");
		};
		WirelessChannel : Channel
		{
			@display("p=0,0");
		}
	connections allowunconnected:
		// connect receiver to the channel
		WirelessChannel.gate++ <--> ReceiverNodeModule.gate;
		
		// connect transmitters to the channel
		for i=0..numTransmitters - 1 
		{
			WirelessChannel.gate++ <--> TransmitterNodeModule[i].gate;
		}
}