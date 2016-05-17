import os
import sys
import numpy as np
import math
import matplotlib.pyplot as plt
import pprint
def main(type, freq):
    if type == "small":
        File=["small_write_seq","small_write_random"]
    else:
        File=["big_write_seq","big_write_random"]

    output=[]
    outputs=[0,1]
    is_seq=0
    for fl in File:
        with open(fl) as f:
            content = f.readlines()
            cycles=[]
            #output.append(is_seq,0,0)
            for entry in content:
                if entry=='\n':
                    cycles=np.array(cycles)
                    avg=mean(cycles)
                    output.append((is_seq,size,avg))
                    cycles=[]
                else:
                    size=int(entry.split(",")[0])
                    cycle=float(entry.split(",")[1])
                    cycles.append(cycle)
        is_seq=1

    xaxis=[]
    yaxis=[]
    for e in outputs:
        x=[]
        y=[]
        for entry in output:
            if e == entry[0]:
                #x.append(math.log(entry[1],2))
                x.append(entry[1])
                y.append(entry[2]/freq)
        xaxis.append(x)
        yaxis.append(y)
        print len(x)

    i=0
    fig=plt.figure()
    plt.subplot(111)
    plt.grid(True)
    plt.title("Write Latency for " + type + " files")
    plt.ylabel('Latency in nano seconds')
    plt.xlabel('File size in bytes')
    while i<len(xaxis):
        if i==0:
            s="Sequential"
        else:
            s="Random"
        plt.plot(xaxis[i],yaxis[i], linestyle='--', marker='o', label=(s+" writes"))
        i+=1
    plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05),fancybox=True, shadow=True, ncol=5)
    plt.show()

def mean(entries):
    return np.mean(entries, axis=0)

if __name__=="__main__":
    main(str(sys.argv[1]), float(sys.argv[2]))
