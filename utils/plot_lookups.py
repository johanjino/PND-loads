import matplotlib.pyplot as plt
import numpy as np
import os
import sys

benchmark_names = ["perlbench.0", "perlbench.1", "perlbench.2", "mcf.0", "lbm.0", "xalancbmk.0", "x264.0", "x264.1", "x264.2", "deepsjeng.0", "leela.0", "nab.0", "xz.0", "xz.1"]

differences = ["differences-extra-large"]
sizes = ["Large"]
for c, difference in enumerate(differences):
    base_lookups = {}
    pnd_lookups = {}
    with open(difference, "r") as stats:
        results = {}
        for line in stats:
            if line.strip()[:-1] in benchmark_names:
                current_benchmark = line.strip()[:-1] #cut off ':'
                continue
            if len(line.strip()) == 0: continue
            name, value = line.strip().split(': ')
            if name == 'Base LookupPerKInst':
                base_lookups[current_benchmark] = value
            if name == 'PND LookupPerKInst':
                pnd_lookups[current_benchmark] = value

    percent_diff = []
    base_values = []
    pnd_values = []
    for name in benchmark_names:
        base = float(base_lookups[name])
        pnd = float(pnd_lookups[name])
        base_values.append(base)
        pnd_values.append(pnd)
        percent_diff.append((pnd - base)/base * 100)

    fig, ax = plt.subplots()
    fig.set_size_inches(12,10,forward=True)

    # Set every other bar to a different color
    color_sequence = ['#1f77b4', '#ff7f0e'] * 5
    bar_width = 0.4
    bar_gap = 0.2

    positions1 = np.arange(len(benchmark_names))

    ax.bar(positions1, base_values, width=bar_width, label='Base')
    pnd_bars = ax.bar(positions1+bar_width, pnd_values, width=bar_width, label='PND')

    # Set the y-limit to start at the smallest value in the data
    #ax.set_ylim(bottom=min(values))

    # # Set the y-axis to logarithmic scale
    #ax.set_yscale('log')

    # # Round y-axis ticks up to next order of magnitude
    # y_max = np.ceil(np.log10(max(values)))
    # ax.set_yticks(10 ** np.arange(0, y_max+1))

    # Add labels and title

    # for i, bar in enumerate(pnd_bars):
    #     value = percent_diff[i]
    #     ax.text(bar.get_x() + bar.get_width() / 2, value, str(round(value,3))+"%", ha='center', va='bottom')

    for i, value in enumerate(percent_diff):
        ax.text(positions1[i] + bar_width+0.03, pnd_values[i]+0.5, str(round(value,1))+"%", ha='center', va='bottom')

    ax.set_ylabel('Lookups per KiloInst', fontsize=18)
    ax.legend(labels=['Unlabelled Run', 'Labelled Run'], fontsize=16)
    ax.set_title('MDP Lookup Reduction', fontsize=18)
    ax.set_xticks(positions1, benchmark_names)
    plt.tick_params(axis='x',labelsize=14, rotation=60)
    plt.tick_params(axis='y',labelsize=12)
    plt.tight_layout()
    plt.savefig("/home/muke/Documents/papers/PND-ARCS/figures/lookups-small.png", dpi=600)
