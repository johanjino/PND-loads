import subprocess
import sys
from collections import defaultdict, Counter

binary = sys.argv[1]
count = sys.argv[2]
gem5_cmd = ' '.join(sys.argv[3:])

gem5 = subprocess.run(gem5_cmd, shell=True, capture_output=True, text=True)

addr_info = {}
violation_count = defaultdict(int)
function_count = defaultdict(int)

for line in gem5.stdout:
    if "Violation" in line:
        addr = line.split(":")[1]
        violation_count[addr] += 1
        addr2line = subprocess.run("addr2line -f -e "+binary+" "+addr, shell=True, capture_output=True, text=True)
        func = addr2line.stdout.split("\n")[0]
        line_number = int(addr2line.stdout.split("\n")[1].split(":")[1])
        addr_info[addr] = (func, line_number)
        function_count[func] += 1

largest_insts = Counter(violation_count).most_common(int(count))
largest_funcs = Counter(function_count).most_common(int(count))

print(count+" most violating loads:")
for i, c in largest_insts:
    func, line_number = addr_info[i]
    print(func, line_number, c)

print()
print(count+" most violating functions:")
for f, c in largest_funcs:
    print(f, c)
