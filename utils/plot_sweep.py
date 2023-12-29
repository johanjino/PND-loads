import matplotlib.pyplot as plt
import os
import sys
import math

results_dir = sys.argv[1]
benchmark_names = ["perlbench.0", "perlbench.1", "perlbench.2", "mcf.0", "lbm.0", "xalancbmk.0", "x264.0", "x264.1", "x264.2", "deepsjeng.0", "leela.0", "nab.0", "xz.0", "xz.1"]
SSIT_range = [32, 128, 352, 567, 800, 1024]
LFST_range = [32, 128, 1024]
clear_period_ratio_range = [122, 244, 488, 976]
lfst_map = {bench: {lfst:{ssit:None for ssit in SSIT_range} for lfst in LFST_range} for bench in benchmark_names}
clear_map = {bench: {clear:{ssit:None for ssit in SSIT_range} for clear in clear_period_ratio_range} for bench in benchmark_names}

def parse_results(results_dir):
    for d in os.listdir(results_dir):
        ssit_size, lfst_size, clear_period = [int(i) for i in d.split('_')]
        results = open(d+"/differences").readlines()
        current_benchmark = None
        for line in results:
            if line.strip()[:-1] in benchmark_names:
                current_benchmark = line.strip()[:-1]
            elif current_benchmark and line.strip().split(":")[0] == "CPI":
                if results_dir.split("/")[-1] == "lfst_ssit_search":
                    lfst_map[current_benchmark][lfst_size][ssit_size] = float(line.strip().split(":")[1])
                elif results_dir.split("/")[-1] == "clear_period_ssit_search":
                    clear_map[current_benchmark][clear_period][ssit_size] = float(line.strip().split(":")[1])

def plot_benchmark(benchmark, results_map, label, title):
    fig, ax = plt.subplots()

    for param in results_map.keys():
        ssits = list(results_map[param].keys())
        for ssit in ssits:
            if results_map[param][ssit] is None: print("a ssit value was not populated!"); exit(1)
        cpis = [results_map[param][ssit] for ssit in ssits]
        ax.plot(ssits, cpis, label=f'{label}={param}')
    ax.set_xlabel('SSIT Size')
    ax.set_ylabel('CPI')
    ax.set_title(title)
    ax.legend()
    plt.show()
    #plt.savefig(results_dir+"/"+benchmark+".png")

def plot_average(results_map, label, title):
    fig, ax = plt.subplots()

    average_map = {param:{ssit:1 for ssit in SSIT_range} for param in results_map}
    for bench in results_map:
        for param in results_map[bench]:
            for ssit in SSIT_range:
                average_map[param][ssit] *= results_map[bench][param][ssit]
    for param in average_map:
        for ssit in average_map[param]:
            average_map[param][ssit] = math.pow(average_map[param][ssit], 1.0 / len(benchmark_names))

    plot_benchmark("Average", average_map, label, title)

        

parse_results(results_dir+"/lfst_ssit_search")
parse_results(results_dir+"/clear_period_ssit_search")

benchmark = benchmark_names[0]
plot_benchmark(benchmark, lfst_map[benchmark], "LFST", benchmark+": SSIT vs CPI Difference Over LFST (Clear Period Ratio = 244)")
# for benchmark in benchmark_names:
#     plot_benchmark(benchmark, lfst_map[benchmark], "LFST", benchmark+": SSIT vs CPI Difference Over LFST (Clear Period Ratio = 244)")
#     plot_benchmark(benchmark, lfst_map[benchmark], "Clear Period Ratio", benchmark+": SSIT vs CPI Difference Over Clear Period Ratio (LFST = SSIT)")
# plot_average(lfst_map, "LFST", "Average CPI Difference Across Spec As LFST/SSIT Vary")
# plot_average(clear_map, "Clear Period Ratio", "Average CPI Difference Across Spec As Clear Period Ratio/SSIT Vary")
