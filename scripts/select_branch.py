# simulate the design for ten millon cycles
# select the rare branches
import os
import re
import numpy as np
from collections import defaultdict
import subprocess
import time
from multiprocessing import Pool
import shutil
import random
length = 0
cycle = 0

def f(path, seed):
    np.random.seed(seed)
    if os.path.exists(path):
        shutil.rmtree(path)
    os.mkdir(path)
    os.chdir(path)

    branches = defaultdict(int)
    # compile and run
    for i in range(5000):
        with open("data.mem", "w") as fout:
            for j in range(cycle):
                fout.write(str(j%2))
                a = np.random.randint(2, size=(length-1)).tolist()
                fout.write("".join(map(str, a)))
                fout.write("\n")
        proc = subprocess.Popen(["vvp", "../../conc_run.vvp"], stdout=subprocess.PIPE)
        (out, err) = proc.communicate()
        for line in out.decode().split('\n')[:-1]:
            spt = line.split()
            if spt[0] == ';A':
                branches[spt[1]] += 1

    with open("temp", "w") as fout:
        #for b in (sorted(branches, key=lambda x: branches[x]))[:20]:
        for b in branches:
            fout.write("{}: {}\n".format(b, branches[b]))

if __name__ == '__main__':
    # conquest_dut.v and conquest_tb.v is required
    assert(os.path.isfile("../conc_run.vvp"))

    # read length
    with open('../conquest_tb.v', 'r') as fin:
        for line in fin:
            m = re.search('reg  \[(\d+):0\] _conc_ram.*;', line)
            if (m):
                length = int(m.group(1)) + 1

    # read cycle
    with open('../Makefile.inc', 'r') as fin:
        for line in fin:
            m = re.search('UNROLL_CYC.*= (\d+)', line)
            if (m):
                cycle = int(m.group(1)) + 1
    p = Pool(20)
    dirs = [("dir" + str(i), random.randint(1, 10000000)) for i in range(1, 21)]
    p.starmap(f, dirs)
