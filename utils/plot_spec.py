import matplotlib.pyplot as plt
import numpy as np

benchmark_names = ["perlbench.0", "perlbench.1", "perlbench.2", "mcf.0", "lbm.0", "xalancbmk.0", "x264.0", "x264.1", "x264.2", "deepsjeng.0", "leela.0", "nab.0", "xz.0", "xz.1"]
benches = {}
field = "CPI"
with open("./differences", "r") as stats:
    results = {}
    for line in stats:
        if line.strip()[:-1] in benchmark_names:
            current_benchmark = line.strip()[:-1] #cut off ':'
            continue
        if len(line.strip()) == 0: continue
        name, value = line.strip().split(': ')
        if name == field:
            results[current_benchmark] = float(value)

fig, ax = plt.subplots()
bars = ax.bar(benchmark_names, [results[name] for name in benchmark_names])

for bar in bars:
    value = bar.get_height()
    ax.text(bar.get_x() + bar.get_width() / 2, value, str(round(value,3))+"%", ha='center', va='bottom' if value >=0 else 'top')

ax.set_xlabel('Benchmark', fontsize=18)
ax.set_ylabel('Percent Change', fontsize=18)
ax.set_title('CPI Difference - Workstation, 32 SSIT Entries', fontsize=24)
plt.show()
