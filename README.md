COSC418 Project Note
====================
General idea: investigate the performance of a simple CSMA protocol over a wireless channel. 

Brief
-----
Each transmiter node contains:
- Packet generator: generate higher layer message with interarrival time (configurable).
- MAC module: accept message from its higher layer. The module would also buffer those packets in a queue with given size (configurable). The excess packet will be dropped. The module will interact with transceiver for packet receiving, transmitting and carrier-sense operations. On the other hand, the module would pass received message to higher layer after processing those packets.
- The transceiver module will transmit packet over the channel and help with CSMA operations. The transceiver would be capable of determining whether those packets are received correctly. Those packets will be dropped if checksum is incorrect.


Details
-------
- TransmiterNode
	- Packet Generator.
	- MAC module.
	- Transceiver.
	- ```nodeIdentifier``` (ID for each individual node, configurable in each module).
	- ```nodeXposition``` and ```nodeYPosition```. Those parameters describes the geographical position of the node in 2D space. 

- ReceiverNode
	- Transceiver.
	- MAC module.
	- Packet Sink.
	- ```nodeIdentifier``` (ID for each individual node, configurable in each module).
	- ```nodeXposition``` and ```nodeYPosition```. Those parameters describes the geographical position of the node in 2D space.

- Packet Generator
	- Stream of application layer message.
	- Process message type called ```AppMessage```.
	- ```iatDistribution``` describes the intergeneration time drawn from a random distribution.
	- ```messageSize``` indicates the size of generated message in bytes.
	- ```seqno``` indicates the sequence number.
	- The ```AppMessage``` class contains:
		- ```timeStamp```: simulation time at which the message has been generated.
		- ```senderId```: the node identification of the TransmitterNode.
		- ```sequenceNumber```: the current value of the seqno maintained in Packet Generator. The number increased afterward.
		- ```msgSize```: indicates the total mesage size in bytes, taken from ```messageSize``` from Packet Generator.

- Packet Sink
	- Accept message from local MAC module.
	- Prints texual information about the message to a log file, records other statistics and dispose the message.
	- Requires filename of log file as parameter.
	- The information that is required to write to the log file:
		- The time as which PacketSink ahas received the message.
		- The fields from the ```AppMessage```, i.e. the ```timeStamp```, ```senderId```, ```sequenceNumber``` and ```msgSize```.

- Transceiver
	- Take packets from the MAC and models a transmission of this packet with given transmit power and transceiver turnaround operations.
	- On the receiving side, it tracks packet transmissions from all stations, check for collisions and calcular SNR. It converts the SNR into a bit error rate p and packet error rate P. It drops the packet with probability P.
	- Module parameters:
		- ```txPowerDBm``` specifies the radiated power in dBm^2.
		- ```bitRate``` specifies the transmission bitrate in bits/s.
		- ```csThreshDBm``` specifies the observed signal power above which a carrier-sensing station will indicate the medium as busy.
		- ```noisePowerDBm``` specifies the noise power. 
		- ```turnaroundTime``` specifies the time required to turning the transceiver from the transmit state to the receiver state or vice versa. 
		- ```csTime``` specifies the time required to carry out a cerrier sense operation. 
	- Internal Variables:
		- ```transceiverState``` stores two different values: a receive state and a transmit state. The receive state is the default state. The transmit state is only entered upon request from the ```MAC``` module and consists of three distinct stages:
			- After getting the request from the ```MAC``` the ```Transceiver``` changes to the transmit state, then wait for an amount of time given as the ```turnaroundTime```, modeling the transceiver turnaround time. 
			- Then actual packet transmission is handled between ```Transceiver``` and ```Channel```.
			- After packet transmission we wait for another ```turnaroundTime``` before switching back to the receive state. 
		- ```currentTransmissions``` maintains a list of currently active transmissions and for each tranmission records important features like the identity of the sender and the received power.
	- Carrier-Sensing:
		- The ```MAC``` module starts the process by sending a message of type ```CSRequest``` to the ```Transceiver```. When the ```Transceiver``` is in the receive state, it performs the following steps:
			- It first calculates the current signal power observed on the channel.
			- Then it waits for a time corresponding to the ```csTime``` parameter and finally
			- It sends a message of ```CSResponse``` back to the local ```MAC``` module. The ```CSResponse``` message has one Boolean field called ```busyChannel```. This field is set to TRUE when the current signal power obserbed in the first step exceeds the value of the parameter ```csThresshDBm```, otherwise it is set to FALSE.
	- To calculate the current signal power, I should traverse the list of current transmissions (```CurrentTransmissions```) and add up the received power of all ongoing transmissions in the normal domain, not in the dB domain, and finally convert the sum to dBm. This models a setup where the powers of all involved signals simply add up.
	