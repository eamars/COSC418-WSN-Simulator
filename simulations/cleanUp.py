import sys
import os

LOG_FILES = ['Data_Dropped_MAC.txt', 'Data_Generate.txt', 'Data_Received.txt', 'Data_Transmit.txt', 'Message_PacketSink.txt']

def main():
    for fileName in LOG_FILES:
        os.system('rm {}'.format(fileName))
        print('Removed {}'.format(fileName))
    print('Log files all removed!')
    

if __name__ == "__main__":
    main()
    