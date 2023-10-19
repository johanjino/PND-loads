import matplotlib.pyplot as plt
import numpy as np
import os

def plot_diff(field):
    values = []
    percent_diff = []
    names = []
    for name in benchmark_names:
        base = benches[name+"_base"]
        modified = benches[name+"_modified"]
        values.append(base[field])
        values.append(modified[field])
        percent_diff.append((modified[field] - base[field]) / base[field] * 100)
        names.append(name+"_base")
        names.append(name+"_modified")
    percent_diff.append(0)

    fig, ax = plt.subplots()

    # Set every other bar to a different color
    color_sequence = ['#1f77b4', '#ff7f0e'] * 5
    ax.bar(names, values, color=color_sequence)

    # Set the y-limit to start at the smallest value in the data
    #ax.set_ylim(bottom=min(values))

    # # Set the y-axis to logarithmic scale
    # ax.set_yscale('log')

    # # Round y-axis ticks up to next order of magnitude
    # y_max = np.ceil(np.log10(max(values)))
    # ax.set_yticks(10 ** np.arange(0, y_max+1))

    # Add labels and title
    ax.set_xlabel('Benchmark')
    ax.set_ylabel('CPI')
    ax.set_title('CPI')

    # Add percentage difference text
    percent_diff = iter(percent_diff)
    for i in range(len(values)):
        if i % 2 == 1:
            ax.text(i, values[i], f"{next(percent_diff):.1f}%", ha='center', va='bottom')

    plt.show()

benchmark_names = ["leela", "mcf", "perlbench.0", "perlbench.1", "perlbench.2", "x264.0", "x264.1", "x264.2", "xalancbmk", "nab", "xz.0", "xz.1", "deeps"]
benches = {}
for results_file in os.listdir("."):
    if results_file == "cpi_differences": continue
    if os.path.isdir(results_file): continue
    with open(results_file, "r") as stats:
        results = {}
        for line in stats:
            if "Adjusted Mem Order Violation Events" in line:
                name = "Adjusted Mem Order Violation Events"
                value = float(line.strip().split(" ")[-1])
            elif "Lookup reduction" in line:
                name = "Lookup Reduction"
                value = float(line.strip().split(' ')[-1])*100
                results[name] = value
            else:
                name, value = line.strip().split(' ')
                results[name] = float(value)
    bench_name = results_file.split("_")[0]
    if results_file.split(".")[1].isdigit():
        bench_name += "."+results_file.split(".")[1]
    if "base" in results_file.split("_")[1]:
        bench_name += "_base"
    else:
        bench_name += "_modified"
    benches[bench_name] = results


fig, ax = plt.subplots()
ax.bar(benchmark_names, [benches[name+"_modified"]["Lookup Reduction"] for name in benchmark_names])
ax.set_xlabel('Benchmark')
ax.set_ylabel('Percent Reduction')
ax.set_title('Store Set Lookup Reduction')
plt.show()
