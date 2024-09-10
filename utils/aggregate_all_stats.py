import subprocess
import os
import sys

addr_file_type = sys.argv[1]
cpu_model = sys.argv[2]
run_pnd = True
run_base = True if sys.argv[3] == 'with_base' else False
if addr_file_type == "base":
    run_base = True
    run_pnd = False
if run_base: print("Running with base model")
addr_file_dir = "/work/muke/PND-Loads/addr_files/"
chkpt_dir = "/work/muke/checkpoints/"
results_dir = "/work/muke/PND-Loads/results/"+addr_file_type+"/"+cpu_model+"/"
base_results_dir = "/work/muke/PND-Loads/results/base/"+cpu_model+"/"
benches = ["600.perlbench_s", "605.mcf_s", "619.lbm_s",
           "623.xalancbmk_s", "625.x264_s", "631.deepsjeng_s",
           "641.leela_s", "657.xz_s", "602.gcc_s",
           "620.omnetpp_s", "644.nab_s"] #"638.imagick_s"]

for bench in benches:
    name = bench.split(".")[1].split("_")[0]

    for i in (0,1,2):
        if os.path.exists(results_dir+name+'.'+str(i)):
            raw_results_dir = results_dir+name+'.'+str(i)+"/raw/"
            os.chdir(raw_results_dir)
            p = subprocess.Popen("python3 /work/muke/pnd-loads/utils/aggregate_stats.py "+bench+" "+str(i), shell=True)
            p.wait()
            subprocess.Popen("cp results.txt ../", shell=True)
