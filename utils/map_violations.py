import subprocess
import sys
from collections import defaultdict, Counter

binary = sys.argv[1]
count = sys.argv[2]
gem5_output = open(sys.argv[3]).readlines()

addr_info = {}
violation_count = {}
total_count = defaultdict(int)
function_count = defaultdict(int)

for line in gem5_output:
    if "Violation:" in line:
        addrs = line.split(":")[-1] 
        load = addrs.split(",")[0]
        store = addrs.split(",")[1].strip()
        if load in violation_count:
            if store in violation_count[load]:
                violation_count[load][store] += 1
            else:
                violation_count[load][store] = 1
        else:
            violation_count[load] = {}
            violation_count[load][store] = 1
        total_count[load] += 1

largest_insts = Counter(total_count).most_common(int(count))

print(count+" most violating loads:")
for i, c in largest_insts:
    print(hex(int(i)), hex(int(max(violation_count[i]))), total_count[i])
