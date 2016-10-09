import os
import math
import argparse
from common import COMMON_CONFIG
from template import EXPERIMENT1, EXPERIMENT2

def generateConfigFiles():

    
    for dist in range(1, 41):
        # create configuration file
        fp = open("exp1_config_dist_{}.ini".format(dist), "w")
        fp.write(COMMON_CONFIG)
        fp.write('\n')
        fp.write(EXPERIMENT1.format(dist, dist))
        fp.close()
        
    for num in range(2, 22, 2):
        # create configuration file
        fp = open("exp2_config_nodenum_{}.ini".format(num), "w")
        fp.write(COMMON_CONFIG)
        fp.write('\n')
        fp.write(EXPERIMENT2.format(num, num))
        fp.close()    
        
def runSimulationOne():
    for dist in range(1, 41):
        for i in range(1,11):
            cmd = "../src/COSC418-WSN-Simulator -c ChannelPacketLossRateSimulation -u Cmdenv -n .:../src --sim-time-limit=1000s exp1_config_dist_{}.ini".format(dist)
            os.system(cmd)
        
        os.system('python3.5 log_in_script.py -S one > {}'. format('testDistance' + str(dist)+'.txt'))
        os.system('python3.5 cleanUp.py')
        
def runSimulationTwo():
    Total = len(range(2, 22, 2)) * len(range(1,2))
    count = 0;
    for num in range(2, 22, 2):
        for i in range(1,11):
            cmd = "../src/COSC418-WSN-Simulator -c MACPacketLossRateSimulation -u Cmdenv -n .:../src --sim-time-limit=1000s exp2_config_nodenum_{}.ini".format(num)
            os.system(cmd)
            count += 1
        
        os.system('python3.5 log_in_script.py -S two > {}'. format('testNum' + str(num)+'.txt'))
        os.system('python3.5 cleanUp.py')         
        print('Finished {}%'.format((count/Total)*100))
        


    
    
if __name__ == "__main__":

    
    parser = argparse.ArgumentParser(description = 'Script to run simulations')
    parser.add_argument('-S','--sim', help = 'Run Simulation tasks, choose one or two!', default = 'one')
    args = parser.parse_args()
    testCase = args.sim
    

    generateConfigFiles()
    
    if testCase == 'one':
        runSimulationOne()
    elif testCase == 'two':
        runSimulationTwo()
    else:
        print('Please type one or two')    