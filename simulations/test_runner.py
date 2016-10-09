import threading
import queue
import os
import sys
from common import COMMON_CONFIG
from template import EXPERIMENT1, EXPERIMENT2
from subprocess import Popen, PIPE


# total number of workers (same as CPU numbers)
NUM_WORKERS = 8

def worker(args):
    task_queue = args

    # get thread id
    thread_id = threading.current_thread().name
    print(thread_id, "starts")

    while True:
        task = task_queue.get()

        # stop the worker if queue is empty
        if task is None:
            break
        # print received task
        print(thread_id, "receives", task)

        # print approximate number of tasks left in the queue
        print(thread_id, task_queue.qsize(), "tasks left")

        # execute the incoming command as a subprocess
        p = Popen(task, stdout=PIPE)

        # wait for the execution
        p.wait()

        # fetch the output
        out = p.stdout.read()

        # print the output
        print(thread_id, out)

    print(thread_id, "exits")

def task1():
    tasks = []
    for dist in range(1, 2):
        #for iterarion in range(1, 11):
        # create configuration file
        fp = open("exp1_config_dist_{}.ini".format(dist), "w")
        fp.write(COMMON_CONFIG)
        fp.write('\n')
        fp.write(EXPERIMENT1.format(dist, dist))
        fp.close()
    
        # create executable command
        cmd = "../src/COSC418-WSN-Simulator -c ChannelPacketLossRateSimulation -u Cmdenv -n .:../src --sim-time-limit=1000s exp1_config_dist_{}.ini".format(dist)
    
        tasks.append(cmd.split())
        #os.system('python3.5 log_in_script.py -S one > {}'. format('testDistance' + str(dist)+'.txt'))
        #os.system('python3.5 cleanUp.py')
    return tasks

def task2():
    tasks = []
    for num in range(2, 22, 2):
        # create configuration file
        fp = open("exp2_config_nodenum_{}.ini".format(num), "w")
        fp.write(COMMON_CONFIG)
        fp.write('\n')
        fp.write(EXPERIMENT2.format(num, num))
        fp.close()

        # create executable command
        cmd = "../src/COSC418-WSN-Simulator -c MACPacketLossRateSimulation -u Cmdenv -n .:../src --sim-time-limit=1000s exp2_config_nodenum_{}.ini".format(num)
        tasks.append(cmd.split())
        #os.system('python3.5 log_in_script.py -S two > {}'. format('testNum' + str(num)+'.txt'))
        #os.system('python3.5 cleanUp.py')        

    return tasks

def main():
    # Create a FIFO queue
    task_queue = queue.Queue()

    # Create a thread pool
    thread_pool = []

    # Initialize workers
    for n in range(NUM_WORKERS):
        thread = threading.Thread(target=worker, args = [task_queue])
        thread.start()
        thread_pool.append(thread)

    # Allocate tasks
    tasks = task1()# + task2()
    for task in tasks:
        task_queue.put(task)

    # finish tasks
    for n in range(NUM_WORKERS):
        task_queue.put(None)

    # wait until all threads terminated
    for thread in thread_pool:
        thread.join()

if __name__ == "__main__":
    #os.system('rm testDistance*')
    #os.system('rm testNum*')
    main()
