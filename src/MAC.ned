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

//
// TODO auto-generated module
//
simple MAC
{
	parameters:
		// buffer size for higher layer packets
		int bufferSize = default(10);
		
		// maximum backoff counts
		int maxBackoffs = default(5);
		
		// distribution for random backoff period
		double backoffDistribution = default(0.5);
		
	gates:
		inout gate1;
		inout gate2;
}
