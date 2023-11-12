# simulate the design for ten millon cycles
# select the rare branches
import os
import re
import numpy as np
import time


# conquest_dut.v and conquest_tb.v is required
assert(os.path.isfile("conc_run.vvp") and os.path.isfile("branch_id"))
length = 0
cycle = 0

branch = set()
with open("branch_id", "r") as fin:
    for line in fin:
        branch.add(line[:-1])

# read length
with open('conquest_tb.v', 'r') as fin:
    for line in fin:
        m = re.search('reg  \[(\d+):0\] _conc_ram.*;', line)
        if (m):
            length = int(m.group(1)) + 1

# read cycle
with open('Makefile.inc', 'r') as fin:
    for line in fin:
        m = re.search('UNROLL_CYC.*= (\d+)', line)
        if (m):
            cycle = int(m.group(1)) + 1

print("LENGTH: {} CYCLE: {}\n".format(length, cycle))

t0 = time.clock()
np.random.seed(0)
# compile and run
for i in range(int(10000000/(cycle - 1))):
#for i in range(1):
    with open("data.mem", "w") as fout:
        for j in range(cycle):
            a = np.random.randint(2, size=length).tolist()
            fout.write("".join(map(str, a)))
            fout.write("\n")
    os.system("vvp conc_run.vvp >temp")
    with open("temp", "r") as fin:
        for line in fin:
            m = re.search(';A (\d+)', line)
            if (m):
                b = m.group(1)
                if (b in branch):
                    print('COVERED {} in iteration {}\n'.format(b, i))
                    branch.remove(b)
                if (len(branch) == 0):
                    print("Done with {} iterations\n".format(i))
                    break
    if (len(branch) == 0):
        break

print("{} branches covered".format(20 - len(branch)))
print("{0:.2f} seconds time".format(time.clock() - t0))
