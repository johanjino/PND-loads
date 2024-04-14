import subprocess
import os
import sys

results_dir = sys.argv[1]

spec_dir = "/sim_home/luke/spec2017/benchspec/CPU/"
benches = ["600.perlbench_s", "605.mcf_s", "619.lbm_s",
           "623.xalancbmk_s", "625.x264_s", "631.deepsjeng_s",
           "641.leela_s", "657.xz_s", "607.cactuBSSN_s",
           "648.exchange2_s", "620.omnetpp_s"]

for bench in benches:

    name = bench.split(".")[1].split("_")[0]

    os.chdir(spec_dir+bench+"/run/run_peak_refspeed_mytest-64.0000")

    for f in os.listdir(os.getcwd()):
        try:
            is_res_dir = os.path.isdir(f) and f.split(".")[1].isdigit() and f.split(".")[0] == "pnd"
        except IndexError:
            continue
        if is_res_dir:
            os.chdir(f)
            p = subprocess.Popen("python3 /sim_home/luke/PND-Loads/utils/aggregate_stats.py", shell=True)
            p.wait()
            subprocess.Popen("cp results.txt "+results_dir+"/"+name+"_pnd."+f.split(".")[1]+".txt", shell=True)
            os.chdir("..")

    for f in os.listdir(os.getcwd()):
        try:
            is_res_dir = os.path.isdir(f) and f.split(".")[1].isdigit() and f.split(".")[0] == "base"
        except IndexError:
            continue
        if is_res_dir:
            os.chdir(f)
            p = subprocess.Popen("python3 /sim_home/luke/PND-Loads/utils/aggregate_stats.py", shell=True)
            p.wait()
            subprocess.Popen("cp results.txt "+results_dir+"/"+name+"_base."+f.split(".")[1]+".txt", shell=True)
            os.chdir("..")
            
os.chdir(results_dir)
subprocess.Popen("python3 /sim_home/luke/PND-Loads/utils/cpi_cmp.py")
