import matplotlib.pyplot as plt
import os
import numpy as np
import argparse
import sys

benchmark_names = ["perlbench.0", "perlbench.1", "perlbench.2", "gcc.0", "gcc.1", "gcc.2", "mcf.0", "lbm.0", "omnetpp.0", "xalancbmk.0", "x264.0", "x264.1", "x264.2", "deepsjeng.0", "leela.0", "nab.0", "xz.0", "xz.1"]

parser = argparse.ArgumentParser(prog='plot spec', description='plot graphs')

parser.add_argument('--run-type', type=str, required=True)
parser.add_argument('--addr-types', type=str, required=True)
parser.add_argument('--cpu-models', type=str, required=True)
parser.add_argument('--field', type=str, required=True)
args = parser.parse_args()

run_type = args.run_type.split(',')[0]
addr_type = args.addr_types.split(',')[0]
cpu_model = args.cpu_models.split(',')[0]
field = args.field.split(',')[0]
if field != "lookups" and field != "violations" and field != "collisions":
    print("invalid field")
    exit(1)
field = field[0].upper()+field[1:]

model_dir = "/work/muke/PND-Loads/cpu_models/"
benches = {}
pnd_results = {b:{} for b in benchmark_names}
base_results = {b:{} for b in benchmark_names}
results_dir = "/work/muke/results/"+run_type+"/"+addr_type+"/"+cpu_model
stats = open(results_dir+"/differences", "r")
for line in stats:
    if line.strip()[:-1] in benchmark_names:
        current_benchmark = line.strip()[:-1] #cut off ':'
        continue
    if len(line.strip()) == 0: continue
    name, value = line.strip().split(': ')
    if ' '.join(name.split()[0:2]) == "PND "+field:
        pnd_results[current_benchmark][cpu_model] = float(value)
    if ' '.join(name.split()[0:2]) == "Base "+field:
        base_results[current_benchmark][cpu_model] = float(value)

percent_diff = []
base_values = []
pnd_values = []
for name in benchmark_names:
    base = float(base_results[name][cpu_model])
    pnd = float(pnd_results[name][cpu_model])
    base_values.append(base)
    pnd_values.append(pnd)
    percent_diff.append((pnd - base)/base * 100)

fig, ax = plt.subplots()
fig.set_size_inches(12,12,forward=True)
renderer = fig.canvas.get_renderer()

# Set every other bar to a different color
color_sequence = ['#1f77b4', '#ff7f0e'] * 5
bar_width = 0.4
bar_gap = 0.2

positions1 = np.arange(len(benchmark_names))

ax.bar(positions1, base_values, width=bar_width, label='Base')
pnd_bars = ax.bar(positions1+bar_width, pnd_values, width=bar_width, label='PND')

if field == "Lookups": 
    mcf_adjustment = 0.000025
    mcf_adjustment = 25
    adjustment = 30
else: 
    mcf_adjustment = 150
    adjustment = 105
for i, value in enumerate(percent_diff):
    if benchmark_names[i] == "mcf.0": adj = mcf_adjustment
    else: adj = adjustment
    text = ax.text(positions1[i] + bar_width+0.05, pnd_values[i], str(round(value,1))+"%", ha='center', va='top', fontsize=12, rotation=90)
    bbox = text.get_window_extent(renderer=renderer)
    bbox_data = ax.transData.inverted().transform_bbox(bbox)
    text_height = bbox_data.height
    text.set_y(pnd_values[i] + text_height)


measurment = 'Lookups per KiloInst' if field == 'Lookups' else 'Violations per MegaInst'
ax.set_ylabel(measurment, fontsize=18)
ax.legend(labels=['Unlabelled Run', 'Labelled Run'], fontsize=16)
title = 'MDP Lookup Reduction' if field == 'Lookups' else 'Memory Order Violation Difference'
ax.set_title(title, fontsize=18)
ax.set_xticks(positions1, benchmark_names)
ax.set_ylim(0, max(max(pnd_values),max(base_values))+adjustment)
plt.tick_params(axis='x',labelsize=14, rotation=60)
plt.tick_params(axis='y',labelsize=16)
plt.tight_layout()
plt.savefig(results_dir+"/../../graphs/"+field+".png", dpi=600)
