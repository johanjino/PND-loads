import matplotlib.pyplot as plt
import numpy as np

all_benches = ["perlbench.0", "perlbench.1", "perlbench.2", "mcf.0", "lbm.0", "xalancbmk.0", "x264.0", "x264.1", "x264.2", "deepsjeng.0", "leela.0", "nab.0", "xz.0", "xz.1"]
benchmark_names = ["x264.0", "x264.1", "x264.2"]
benches = {}
field = "CPI"
results = {b:{} for b in benchmark_names}
#files = ['differences-small-base', 'differences-small-big-cache-window', 'differences-large-base', 'differences-large-small-ssit']
files = ['differences-large-base', 'differences-large-big-ssit', 'differences-extra-large-base', 'differences-extra-large-small-ssit']
for f in files:
    stats = open("./"+f)
    for line in stats:
        if line.strip()[:-1] in all_benches:
            current_benchmark = line.strip()[:-1] #cut off ':'
            continue
        if len(line.strip()) == 0: continue
        if current_benchmark.split('.')[0] != 'x264': continue
        name, value = line.strip().split(': ')
        if name == field:
            results[current_benchmark][f] = float(value)

fig, ax = plt.subplots()
fig.set_size_inches(20,12,forward=True)
bar_width = 0.125
for c, f in enumerate(files):
    bars = ax.bar(np.arange(len(benchmark_names))+(bar_width*c), [results[name][f] for name in benchmark_names], width=bar_width)
    for bar in bars:
        value = bar.get_height()
        if abs(round(value,1)) > 0.1:
            ax.text(bar.get_x() + bar.get_width() / 2, value, str(round(value,1))+"%", ha='center', va='bottom' if value >=0 else 'top')

ax.legend(labels=['Large, Base', 'Large, Big MDP', 'Extra Large, Base', 'Extra Large, Small MDP'], fontsize=16)
ax.set_xlabel('Benchmark', fontsize=18)
plt.xticks(np.arange(len(benchmark_names)) + bar_width / 2, benchmark_names)
ax.set_ylabel('CPI Percent Change', fontsize=18)
ax.set_title('CPI Difference Over CPU Paramteres', fontsize=24)
plt.show()
