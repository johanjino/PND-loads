import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import argparse
import os

benchmark_names = ["perlbench.0", "perlbench.1", "perlbench.2", "gcc.0", "gcc.1", "gcc.2", "mcf.0", "lbm.0", "omnetpp.0", "xalancbmk.0", "x264.0", "x264.1", "x264.2", "deepsjeng.0", "leela.0", "nab.0", "xz.0", "xz.1"]

parser = argparse.ArgumentParser(prog='plot spec', description='plot graphs')

parser.add_argument('--addr-types', type=str, required=True)
parser.add_argument('--cpu-models', type=str, required=True)
args = parser.parse_args()

addr_types = args.addr_types.split(',')
cpu_models = args.cpu_models.split(',')

model_dir = "/work/muke/PND-Loads/cpu_models/"
benches = {}
field = "CPI"
results = {b:{} for b in benchmark_names}
for model in cpu_models:
    results_dir = os.getcwd()+"/"+addr_types[0]+"/"+model
    stats = open(results_dir+"/differences", "r")
    for line in stats:
        if line.strip()[:-1] in benchmark_names:
            current_benchmark = line.strip()[:-1] #cut off ':'
            continue
        if len(line.strip()) == 0: continue
        name, value = line.strip().split(': ')
        if name == field:
            results[current_benchmark][model] = float(value)

fig, ax = plt.subplots()
fig.set_size_inches(12,12,forward=True)
bar_width = 0.25
for c, model in enumerate(cpu_models):
    bars = ax.bar(np.arange(len(benchmark_names))+(bar_width*c)-(bar_width/len(cpu_models)), [results[name][model] for name in benchmark_names], width=bar_width)
    for bar in bars:
        value = bar.get_height()
        if abs(round(value,1)) > 0.1:
            if value > 0: value += 0.025
            elif value < 0: value -= 0.025
            ax.text(bar.get_x() + bar.get_width() / 2+(bar_width/len(cpu_models)) - 0.05, value, str(round(value,1))+"%", ha='center', va='bottom' if value >=0 else 'top', rotation=90)

ax.legend(labels=cpu_models, fontsize=16)#, loc='center left')

for i in range(len(benchmark_names)):
    plt.axvline(x=i-(0.75*bar_width), color='grey', linestyle=':', linewidth=1)

plt.tick_params(axis='x',labelsize=14, rotation=60)
plt.tick_params(axis='y',labelsize=16)
plt.xticks(np.arange(len(benchmark_names)) + bar_width / 2, benchmark_names)
ax.set_ylabel('Percent Change', fontsize=18)
ax.set_title('CPI Difference - '+addr_types[0].replace('_',' '), fontsize=18)
plt.tight_layout()
plt.savefig(os.getcwd()+"/graphs/"+addr_types[0]+".png", dpi=300)
