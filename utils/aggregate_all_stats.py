import subprocess
import os
import sys

results_dir = sys.argv[1]

spec_dir = "/work/luke_simpoints/spec2017/benchspec/CPU/"
benches = ["600.perlbench_s", "605.mcf_s", "619.lbm_s",
           "623.xalancbmk_s", "625.x264_s", "631.deepsjeng_s",
           "641.leela_s", "644.nab_s", "657.xz_s"]

for bench in benches:

    name = bench.split(".")[1].split("_")[0]

    try:
        os.chdir(spec_dir+bench+"/run/only_pna/run_peak_refspeed_mytest-64.0000")
    except FileNotFoundError:
        os.chdir(spec_dir+bench+"/run/only_pna/run_peak_refspeed_mytest-64.0001")

    for f in os.listdir(os.getcwd()):
        if os.path.isdir(f) and f.split(".")[0] == name and f.split(".")[1].isdigit():
            os.chdir(f)
            p = subprocess.Popen("python3 ~/PND-Loads/utils/aggregate_stats.py", shell=True)
            p.wait()
            subprocess.Popen("cp results.txt "+results_dir+"/"+name+"_pna."+f.split(".")[1]+".txt", shell=True)
            os.chdir("..")

    try:
        os.chdir(spec_dir+bench+"/run/run_peak_refspeed_mytest-64.0000")
    except FileNotFoundError:
        os.chdir(spec_dir+bench+"/run/run_peak_refspeed_mytest-64.0001")

    for f in os.listdir(os.getcwd()):
        if os.path.isdir(f) and f.split(".")[0] == name and f.split(".")[1].isdigit():
            os.chdir(f)
            p = subprocess.Popen("python3 ~/PND-Loads/utils/aggregate_stats.py", shell=True)
            p.wait()
            subprocess.Popen("cp results.txt "+results_dir+"/"+name+"_base."+f.split(".")[1]+".txt", shell=True)
            os.chdir("..")
