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

package wsl_csma;

import wsl_csma.PacketSink;
import wsl_csma.MAC;
import wsl_csma.Transceiver;

// Example structure
// module Host
// {
//    types:
//        ...
//    parameters:
//        ...
//    gates:
//        ...
//    submodules:
//        ...
//    connections:
//        ...
// }


module ReceiverNode
{
	parameters:
		int nodeIdentifier = default(-1);
    	int nodeXPosition;
    	int nodeYPosition;
	gates:
		inout gate;
	submodules:
		PacketSinkModule: PacketSink {
            @display("p=113,66");
        }
        MACModule: MAC {
            @display("p=113,190");
        }
        TransceiverModule: Transceiver {
            @display("p=113,307");
        }
        
        connections allowunconnected:
    	// internal connections
        PacketSinkModule.gate <--> MACModule.gate1;
        MACModule.gate2 <--> TransceiverModule.gate1;
        
        // interface
        TransceiverModule.gate2 <--> gate;
}