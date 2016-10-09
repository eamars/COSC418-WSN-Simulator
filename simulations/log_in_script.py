import math
import argparse

LOG_FILES = ['Data_Dropped_MAC.txt', 'Data_Generate.txt', 'Data_Received.txt', 'Data_Transmit.txt']


def parseNumAndPosition(lineString):
    ret = []  
    for char in lineString.split(','):
        ret.append(int(char))
    return ret
            

def returnNumAndPosition(lineArray):
    returnList = []
    numIndex = 1

    while numIndex < len(lineArray):
        line = lineArray[numIndex]
        #print(line)
        ret = parseNumAndPosition(line)
        if(len(ret) >= 4):
            returnList.append(ret)
        numIndex += 2
        
    return returnList
        
        

def taskSimulationOne(generatedPacketFile, receivedPacketFile):
    packetGeneratorCollection = []
    packetSinkCollection = []
    transmitters = []
    transmittedPackets = 0
    receivedPackets = 0
    
    with open(generatedPacketFile) as g_fp:
        lines = g_fp.read().splitlines()
        #list of transmitters data
        packetGeneratorCollection = returnNumAndPosition(lines)
    
    g_fp.close()

    with open(receivedPacketFile) as r_fp:
        lines = r_fp.read().splitlines()
        #list of transmitters data
        packetSinkCollection = returnNumAndPosition(lines)
        
    r_fp.close() 
    
    #Get numReceivedPackets, assume we have one receiver
    receivedPackets = packetSinkCollection[0][1]
    distance = math.sqrt((packetGeneratorCollection[0][2] - packetSinkCollection[0][2])**2 + 
                         (packetGeneratorCollection[0][3] - packetSinkCollection[0][3])**2)

    for transmitter in packetGeneratorCollection:
        transmittedPackets += transmitter[1]
        if transmitter[0] not in transmitters:
            transmitters.append(transmitter[0])
        
    lossRate = ((transmittedPackets - receivedPackets) * 100) / transmittedPackets 
    
    print('TransmitterNodes #{}   totalGeneratedPackets: {}   totalReceivedPackets: {}   PacketLoss Rate: {}%   Distance: {}\n'
              .format(transmitters, transmittedPackets, receivedPackets, lossRate, distance))



def taskSimulationTwo(generatedPacketFile, MAC_DroppedPacketFile, tranmittedPacketFile, receivedPacketFile):
 
    #Part one
    #Packets loss at MAC buffer
    packetGeneratorCollection = []
    MAC_Collection = []    
    totalNumGenerated = 0
    generators = []
    totalNumDroppedOverFlow = 0
    totalNumDroppedTimeOut = 0
    MACs = []
    with open(generatedPacketFile) as g_fp:
        lines = g_fp.read().splitlines()
        #list of transmitters data
        packetGeneratorCollection = returnNumAndPosition(lines)
        
    g_fp.close()
    
    with open(MAC_DroppedPacketFile) as MAC_fp:
        lines = MAC_fp.read().splitlines()
        #list of transmitters data
        MAC_Collection = returnNumAndPosition(lines)
            
    MAC_fp.close()  
    
    for generator in packetGeneratorCollection:
        totalNumGenerated += generator[1]
        if generator[0] not in generators:
            generators.append(generator[0])
    
    for MAC in MAC_Collection:
        totalNumDroppedOverFlow += MAC[1]
        totalNumDroppedTimeOut += MAC[2]
        if MAC[0] not in MACs:
            MACs.append(MAC[0])
    
    MacDropRate = ((totalNumDroppedOverFlow + totalNumDroppedTimeOut)* 100) / totalNumGenerated
    
    
    #Part two
    #Packets lost in channel
    trancesiverCollection = []
    receiverCollection = []  
    totalNumTransmitted= 0
    tranceivers = []
    totalNumReceived = 0
    receivers = []
    
    with open(tranmittedPacketFile) as t_fp:
        #The first item is a receiver...
        lines = t_fp.read().splitlines()[2:]
        #list of transmitters data
        trancesiverCollection = returnNumAndPosition(lines)
            
    t_fp.close()
        
    with open(receivedPacketFile) as r_fp:
        
        lines = r_fp.read().splitlines()
        #list of transmitters data
        receiverCollection = returnNumAndPosition(lines)
                
    r_fp.close()
    

    for transceiver in trancesiverCollection:
        totalNumTransmitted += transceiver[1]
        if transceiver[0] not in tranceivers:
            tranceivers.append(transceiver[0])
        
    for receiver in receiverCollection:
        totalNumReceived += receiver[1]
        if receiver[0] not in receivers:
            receivers.append(receiver[0])
        
    ChannelLossRate = ((totalNumTransmitted - totalNumReceived)*100)/totalNumTransmitted
    
    
    print('TransmitterNodes #{}   OverFlowDroppedPackets: {}   TimeOutDropppedPackets: {}   TotalGeneratedPackets: {}   MAC DropRate: {}%\n'
          .format(generators, totalNumDroppedOverFlow, totalNumDroppedTimeOut, totalNumGenerated, MacDropRate))
    print('TransmitterNodes #{}   ReceiverNode #{}   TotalTransmittedPackets: {}   TotalReceivedPackets: {}   Channel LossRate: {}%\n'
          .format(tranceivers, receivers, totalNumTransmitted, totalNumReceived, ChannelLossRate))
    
    






if __name__ == "__main__":
    
    #They all write to the same file
    #use cleanUp.py to remove them    
    parser = argparse.ArgumentParser(description = 'Script to run simulations')
    parser.add_argument('-S','--sim', help = 'Run Simulation tasks, choose one or two!', default = 'one')
    args = parser.parse_args()
    testCase = args.sim
    
    if testCase == 'one':
        taskSimulationOne(LOG_FILES[1], LOG_FILES[2])
    elif testCase == 'two':
        taskSimulationTwo(LOG_FILES[1], LOG_FILES[0], LOG_FILES[3], LOG_FILES[2])
    else:
        print('Please type one or two')
    
    
    





