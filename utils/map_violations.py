import subprocess
import sys
from collections import defaultdict, Counter

count = sys.argv[1]
gem5_output = open(sys.argv[2])
lines = gem5_output.readlines()
gem5_output.close()

addr_info = {}
violation_count = {}
total_count = defaultdict(int)
function_count = defaultdict(int)

for line in lines:
    if "Violation:" in line:
        addrs = [i.split()[0] for i in line.split(":")[-1].split(',')]
        load = addrs[0]
        try:
            store = addrs[1].strip()
        except:
            continue
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
