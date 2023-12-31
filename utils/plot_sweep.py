from typing import DefaultDict
import matplotlib.pyplot as plt
import os
from subprocess import Popen
import sys
import math

class CPIValues:
    def __init__(self, base_cpi=None, pnd_cpi=None):
        self.base_cpi = base_cpi
        self.pnd_cpi = pnd_cpi

results_dir = sys.argv[1]
benchmark_names = ["perlbench.0", "perlbench.1", "perlbench.2", "mcf.0", "lbm.0", "xalancbmk.0", "x264.0", "x264.1", "x264.2", "deepsjeng.0", "leela.0", "nab.0", "xz.0", "xz.1"]
weights = {"perlbench.0": 0.5, "perlbench.1": 0.5, "xz.0": 0.5, "xz.1": 0.5, "x264.0": 1/3, "x264.1": 1/3, "x264.2": 1/3}
SSIT_range = [32, 128, 352, 567, 800, 1024]
LFST_range = [32, 128, 1024]
clear_period_ratio_range = [122, 244, 488, 976]
lfst_map = {bench: {lfst:{ssit:CPIValues() for ssit in SSIT_range} for lfst in LFST_range} for bench in benchmark_names}
clear_map = {bench: {clear:{ssit:CPIValues() for ssit in SSIT_range} for clear in clear_period_ratio_range} for bench in benchmark_names}

def parse_results(results_dir):
    for d in os.listdir(results_dir):
        if 'png' in d: continue
        os.chdir(results_dir+"/"+d)
        Popen.wait(Popen("python3 ~/PND-Loads/utils/cpi_cmp.py", shell=True))
        ssit_size, lfst_size, clear_period = [int(i) for i in d.split('_')]
        results = open(results_dir+"/"+d+"/differences").readlines()
        current_benchmark = None
        for line in results:
            if line.strip()[:-1] in benchmark_names:
                current_benchmark = line.strip()[:-1]
                if results_dir.split("/")[-1] == "lfst_ssit_search":
                    result = lfst_map[current_benchmark][lfst_size][ssit_size]
                elif results_dir.split("/")[-1] == "clear_period_ssit_search":
                    result = clear_map[current_benchmark][clear_period][ssit_size]
                else:
                    print("wrong path")
                    exit(1)
            elif current_benchmark and line.strip().split(":")[0] == "Base CPI":
                result.base_cpi = float(line.strip().split(":")[1])
            elif current_benchmark and line.strip().split(":")[0] == "PND CPI":
                result.pnd_cpi = float(line.strip().split(":")[1])

def plot_benchmark(benchmark, results_map, results_dir, label, title):
    fig, ax = plt.subplots()

    for param in results_map.keys():
        ssits = list(results_map[param].keys())
        for ssit in ssits:
            if results_map[param][ssit].base_cpi is None or results_map[param][ssit].pnd_cpi is None: print("a ssit value was not populated!"); exit(1)
        base_cpis = [results_map[param][ssit].base_cpi for ssit in ssits]
        pnd_cpis = [results_map[param][ssit].pnd_cpi for ssit in ssits]
        ax.plot(ssits, base_cpis, label=f'{label}={param}', marker='+')
        ax.plot(ssits, pnd_cpis, label=f'{label}={param}', marker='+')
    ax.set_xlabel('SSIT Size')
    ax.set_ylabel('CPI')
    ax.set_title(title)
    ax.legend()
    plt.savefig(results_dir+"/"+benchmark+".png", dpi=1200)

def plot_average(results_map, label, title):
    fig, ax = plt.subplots()

    average_map = {lfst:{ssit:CPIValues(1,1) for ssit in SSIT_range} for lfst in LFST_range}
    for bench in results_map:
        for param in results_map[bench]:
            for ssit in SSIT_range:
                average_map[param][ssit].base_cpi *= results_map[bench][param][ssit].base_cpi*weights.get(bench, 1)
                average_map[param][ssit].pnd_cpi *= results_map[bench][param][ssit].pnd_cpi*weights.get(bench, 1)
    for param in average_map:
        for ssit in average_map[param]:
            average_map[param][ssit].base_cpi = math.pow(average_map[param][ssit].base_cpi, 1 / len(benchmark_names))
            average_map[param][ssit].pnd_cpi = math.pow(average_map[param][ssit].pnd_cpi, 1 / len(benchmark_names))

    plot_benchmark("average", average_map, results_dir+"/lfst_ssit_search", label, title)

parse_results(results_dir+"/lfst_ssit_search")
#parse_results(results_dir+"/clear_period_ssit_search")

#benchmark = "x264.2"
#plot_benchmark(benchmark, lfst_map[benchmark], "LFST", benchmark+": SSIT vs CPI Difference Over LFST (Clear Period Ratio = 244)")
for benchmark in benchmark_names:
    plot_benchmark(benchmark, lfst_map[benchmark], results_dir+"/lfst_ssit_search", "LFST Size", benchmark+": SSIT vs CPI Difference Over LFST (Clear Period Ratio = 244)")
    #plot_benchmark(benchmark, lfst_map[benchmark], results_dir+"/clear_period_ssit_search", "Clear Period Ratio", benchmark+": SSIT vs CPI Difference Over Clear Period Ratio (LFST = SSIT)")
plot_average(lfst_map, "LFST Size", "Average CPI Difference Across Spec As LFST/SSIT Vary")
# plot_average(clear_map, "Clear Period Ratio", "Average CPI Difference Across Spec As Clear Period Ratio/SSIT Vary")
