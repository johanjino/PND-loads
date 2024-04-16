import subprocess
import os

spec_dir = "/sim_home/luke/spec2017/benchspec/CPU/"
"""
benches = ["600.perlbench_s", "605.mcf_s", "619.lbm_s",
           "623.xalancbmk_s", "625.x264_s", "631.deepsjeng_s",
           "641.leela_s", "657.xz_s", "607.cactuBSSN_s",
           "648.exchange2_s", 
           "620.omnetpp_s"]
"""
benches = ["607.cactuBSSN_s", "648.exchange2_s"]

processes = []
for bench in benches:
    os.chdir(spec_dir+bench+"/run/run_peak_refspeed_mytest-64.0000")

    processes.append(subprocess.Popen("python3 /sim_home/luke/PND-Loads/utils/spec_automation.py pnd", shell=True))

    processes.append(subprocess.Popen("python3 /sim_home/luke/PND-Loads/utils/spec_automation.py base", shell=True))

for p in processes:
    p.wait()
