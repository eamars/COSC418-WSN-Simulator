import sys
import os
import argparse

LOG_FILES = ['Data_Dropped_MAC.txt', 'Data_Generate.txt', 'Data_Received.txt', 'Data_Transmit.txt', 'Message_PacketSink.txt']



if __name__ == "__main__":
    
    
    parser = argparse.ArgumentParser(description = 'Script to clean txt files')
    parser.add_argument('-C','--cln', help = 'clean packet sink message? y/n', default = 'n')
    args = parser.parse_args()
    testCase = args.cln
    

    
    if testCase == 'n':
        for fileName in LOG_FILES[:-1]:
            os.system('rm {}'.format(fileName))
            print('Removed {}'.format(fileName))
        print('Log files all removed!')
    elif testCase == 'y':
        for fileName in LOG_FILES:
            os.system('rm {}'.format(fileName))
            print('Removed {}'.format(fileName))
        print('Log files all removed!')
    else:
        print('Please type y or n')
        
       
    