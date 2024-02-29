import matplotlib.pyplot as plt
import matplotlib
import numpy as np

benchmark_names = ["perlbench.0", "perlbench.1", "perlbench.2", "mcf.0", "lbm.0", "xalancbmk.0", "x264.0", "x264.1", "x264.2", "deepsjeng.0", "leela.0", "nab.0", "xz.0", "xz.1"]
#benchmark_names = ["x264.0", "x264.1", "x264.2"]
benches = {}
field = "CPI"
results = {b:{} for b in benchmark_names}
#files = ['differences-small-base', 'differences-small-big-cache-big-window', 'differences-large-base', 'differences-large-small-ssit']
files = ['differences-small', 'differences-large', 'differences-extra-large']
#files = [i+'-oracle' for i in ['differences-small', 'differences-large', 'differences-extra-large']]
for f in files:
    stats = open("./"+f)
    for line in stats:
        if line.strip()[:-1] in benchmark_names:
            current_benchmark = line.strip()[:-1] #cut off ':'
            continue
        if len(line.strip()) == 0: continue
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

ax.legend(labels=['Small Model', 'Large Model', 'Extra Large Model'], fontsize=22, loc='center left')
ax.set_xlabel('Benchmark', fontsize=28)
plt.xticks(np.arange(len(benchmark_names)) + bar_width / 2, benchmark_names)
ax.set_ylabel('Percent Change', fontsize=28)
ax.set_title('CPI Difference', fontsize=32)
plt.savefig('/home/muke/Documents/papers/PND-ARCS/figures/cpi.png', dpi=600)
