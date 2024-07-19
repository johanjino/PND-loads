import subprocess
import os
import sys

addr_file_type = sys.argv[1]
cpu_model = sys.argv[2]
addr_file_dir = "/work/muke/PND-Loads/addr_files/"
chkpt_dir = "/work/muke/checkpoints/"
results_dir = "/work/muke/PND-Loads/results/"+addr_file_type+"/"+cpu_model+"/"
benches = ["600.perlbench_s", "605.mcf_s", "619.lbm_s",
           "623.xalancbmk_s", "625.x264_s", "631.deepsjeng_s",
           "641.leela_s", "657.xz_s", "602.gcc_s",
           "620.omnetpp_s"]

#run checkpoints
processes = []
for bench in benches:
    os.chdir(chkpt_dir+bench)

    #TODO: check return codes
    processes.append(subprocess.Popen("python3 /work/muke/PND-Loads/utils/spec_automation.py "+addr_file_dir+addr_file_type+"/ "+cpu_model, shell=True))

    processes.append(subprocess.Popen("python3 /work/muke/PND-Loads/utils/spec_automation.py /work/muke/PND-Loads/addr_files/base "+cpu_model, shell=True))

for p in processes:
    p.wait()

#aggregate stats
for bench in benches:
    name = bench.split(".")[1].split("_")[0]

    for i in (0,1,2):
        if os.path.exists(results_dir+name+'.'+str(i)):
            raw_results_dir = results_dir+name+'.'+str(i)+"/raw/"
            os.chdir(raw_results_dir)
            p = subprocess.Popen("python3 /work/muke/PND-Loads/utils/aggregate_stats.py", shell=True)
            p.wait()
            subprocess.Popen("cp results.txt ../", shell=True)

os.chdir(results_dir)
subprocess.Popen("python3 /sim_home/luke/PND-Loads/utils/cpi_cmp.py")
