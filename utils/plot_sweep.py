from typing import DefaultDict
import matplotlib.pyplot as plt
import os
from subprocess import Popen
from matplotlib.lines import Line2D
import sys
import math

class CPIValues:
    def __init__(self, base_cpi=None, pnd_cpi=None, diff=None):
        self.base_cpi = base_cpi
        self.pnd_cpi = pnd_cpi
        self.diff = diff

home = "/work/home_moved/l50031074"
results_dir = sys.argv[1]
benchmark_names = ["perlbench.0", "perlbench.1", "perlbench.2", "mcf.0", "lbm.0", "xalancbmk.0", "x264.0", "x264.1", "x264.2", "deepsjeng.0", "leela.0", "nab.0", "xz.0", "xz.1"]
weights = {"perlbench.0": 1/3, "perlbench.1": 1/3, "perlbench.2": 1/3, "xz.0": 0.5, "xz.1": 0.5, "x264.0": 1/3, "x264.1": 1/3, "x264.2": 1/3}
SSIT_range = [32, 128, 352, 567, 800, 1024]
LFST_range = [32, 128, 512, 1024]
clear_period_ratio_range = [122, 244, 488, 976]
lfst_map = {bench: {lfst:{ssit:CPIValues() for ssit in SSIT_range} for lfst in LFST_range} for bench in benchmark_names}
clear_map = {bench: {clear:{ssit:CPIValues() for ssit in SSIT_range} for clear in clear_period_ratio_range} for bench in benchmark_names}

def parse_results(results_dir):
    for d in os.listdir(results_dir):
        if 'png' in d: continue
        os.chdir(results_dir+"/"+d)
        Popen.wait(Popen("python3 "+home+"/PND-Loads/utils/cpi_cmp.py", shell=True))
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
            elif current_benchmark and line.strip().split(":")[0] == "CPI":
                result.diff = float(line.strip().split(":")[1])

def plot_benchmark(benchmark, results_map, results_dir, label, title):
    _, ax = plt.subplots(2,1,figsize=(10,8))
    abs_ax = ax[0]
    diff_ax = ax[1]

    lines = []

    for param in results_map.keys():
        ssits = list(results_map[param].keys())
        for ssit in ssits:
            if results_map[param][ssit].base_cpi is None or results_map[param][ssit].pnd_cpi is None: print("a ssit value was not populated!"); exit(1)
        base_cpis = [results_map[param][ssit].base_cpi for ssit in ssits]
        pnd_cpis = [results_map[param][ssit].pnd_cpi for ssit in ssits]
        diff_cpis = [results_map[param][ssit].diff for ssit in ssits]
        line, = abs_ax.plot(ssits, base_cpis, label=f'{label}={param}', marker='+', alpha=0.7)
        abs_ax.plot(ssits, pnd_cpis, label=f'{label}={param}', marker='o', color=line.get_color(), linestyle='--')
        lines.append(line)
        diff_ax.plot(ssits, diff_cpis, label=f'{label}={param}')
    legends = [Line2D([0], [0], color=line.get_color(), lw=2, label=line.get_label()) for line in lines]
    abs_ax.set_xlabel('SSIT Size')
    abs_ax.set_ylabel('CPI')
    abs_ax.set_title(title+ " - Absolute CPI")
    abs_ax.legend(handles=legends)
    diff_ax.set_xlabel('SSIT Size')
    diff_ax.set_ylabel('CPI % Diff')
    diff_ax.set_title(title+" - % Diff CPI")
    plt.tight_layout()
    plt.savefig(results_dir+"/"+benchmark+".png", dpi=1200)

def plot_average(results_map, param_range, results_dir, label, title):
    average_map = {param:{ssit:CPIValues(1,1,0) for ssit in SSIT_range} for param in param_range}
    for bench in results_map:
        for param in results_map[bench]:
            for ssit in SSIT_range:
                average_map[param][ssit].base_cpi *= results_map[bench][param][ssit].base_cpi*weights.get(bench, 1)
                average_map[param][ssit].pnd_cpi *= results_map[bench][param][ssit].pnd_cpi*weights.get(bench, 1)
                average_map[param][ssit].diff += results_map[bench][param][ssit].diff*weights.get(bench, 1)
    for param in average_map:
        for ssit in average_map[param]:
            #len(benchmark_names) - 5 because we want to treat different runs of the same workload as one data point
            average_map[param][ssit].base_cpi = math.pow(average_map[param][ssit].base_cpi, 1 / (len(benchmark_names)-5))
            average_map[param][ssit].pnd_cpi = math.pow(average_map[param][ssit].pnd_cpi, 1 / (len(benchmark_names)-5))
            average_map[param][ssit].diff = average_map[param][ssit].diff / (len(benchmark_names)-5)

    plot_benchmark("average", average_map, results_dir, label, title)

parse_results(results_dir+"/lfst_ssit_search")
parse_results(results_dir+"/clear_period_ssit_search")

for benchmark in benchmark_names:
    plot_benchmark(benchmark, lfst_map[benchmark], results_dir+"/lfst_ssit_search", "LFST Size", benchmark+": SSIT vs CPI Over LFST (Clear Period Ratio = 244)")
    plot_benchmark(benchmark, clear_map[benchmark], results_dir+"/clear_period_ssit_search", "Clear Period Ratio", benchmark+": SSIT vs CPI Over Clear Period Ratio (LFST = SSIT)")
plot_average(lfst_map, LFST_range, results_dir+"/lfst_ssit_search", "LFST Size", "Average CPI Across Spec As LFST/SSIT Vary")
plot_average(clear_map, clear_period_ratio_range, results_dir+"/clear_period_ssit_search", "Clear Period Ratio", "Average CPI Across Spec As Clear Period Ratio/SSIT Vary")
