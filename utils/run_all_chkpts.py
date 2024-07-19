import subprocess
import os
import sys

addr_file = sys.argv[1]
addr_file_dir = "/work/muke/PND-Loads/addr_files/"
spec_dir = "/work/muke/spec2017/benchspec/CPU/"
benches = ["600.perlbench_s", "605.mcf_s", "619.lbm_s",
           "623.xalancbmk_s", "625.x264_s", "631.deepsjeng_s",
           "641.leela_s", "657.xz_s", "602.gcc_s",
           "620.omnetpp_s"]


processes = []
for bench in benches:
    os.chdir(spec_dir+bench+"/run/run_peak_refspeed_mytest-64.0000")

    processes.append(subprocess.Popen("python3 /work/muke/PND-Loads/utils/spec_automation.py "+addr_file_dir+addr_file, shell=True))

    processes.append(subprocess.Popen("python3 /work/muke/PND-Loads/utils/spec_automation.py "+addr_file_dir+"empty", shell=True))

for p in processes:
    p.wait()

for
