import os
import sys
import numpy as np
import math
import matplotlib.pyplot as plt
import pprint
def main(op, type, freq):
    output=[]
    is_rand=0
    is_ext4=0
    File=["data/"+type+"_"+op+"_seq", "data/"+type+"_"+op+"_random"]
    for fl in File:
        with open(fl) as f:
            content = f.readlines()
            cycles=[]
            #output.append(is_rand, 0, 0)
            for entry in content:
                if entry=='\n':
                    cycles=np.array(cycles)
                    avg=mean(cycles)
                    output.append((is_ext4, is_rand, size, avg))
                    cycles=[]
                else:
                    size=int(entry.split(",")[0])
                    cycle=float(entry.split(",")[1])
                    cycles.append(cycle)
        is_rand=1

    is_rand=0
    is_ext4=1
    File=["data/"+type+"_"+op+"_seq_ext4", "data/"+type+"_"+op+"_random_ext4"]
    for fl in File:
        with open(fl) as f:
            content = f.readlines()
            cycles=[]
            #output.append(is_rand, 0, 0)
            for entry in content:
                if entry=='\n':
                    cycles=np.array(cycles)
                    avg=mean(cycles)
                    output.append((is_ext4, is_rand, size, avg))
                    cycles=[]
                else:
                    size=int(entry.split(",")[0])
                    cycle=float(entry.split(",")[1])
                    cycles.append(cycle)
        is_rand=1

    fig=plt.figure(op + " latency for " + type + " files")
    plt.subplot(111)
    plt.grid(True)
    plt.title(op + " latency for " + type + " files")
    plt.ylabel('latency in nano seconds')
    plt.xlabel('file size in bytes')
    for e1 in [0,1]:
        for e2 in [0,1]:
            x=[]
            y=[]
            for entry in output:
                if e1 == entry[0] and e2 == entry[1]:
                    #x.append(math.log(entry[1],2))
                    x.append(entry[2])
                    y.append(entry[3]/freq)

            if e1==0 and e2==0:
                s="nova/sequential " + op
            elif e1==0 and e2==1:
                s="nova/random " + op
            elif e1==1 and e2==0:
                s="ext4/sequential " + op
            else:
                s="ext4/random " + op
            print len(x)
            plt.plot(x, y, linestyle='--', marker='o', label=s)

    plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05),fancybox=True, shadow=True, ncol=5)
    plt.show()

def mean(entries):
    return np.mean(entries, axis=0)

if __name__=="__main__":
    print "Usage: python graph.py <read/write> <small/big> <freq>"
    main(str(sys.argv[1]), str(sys.argv[2]), float(sys.argv[3]))
