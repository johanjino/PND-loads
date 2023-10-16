import subprocess
import os

spec_dir = "/work/luke_simpoints/spec2017/benchspec/CPU/"
benches = ["600.perlbench_s", "605.mcf_s", "619.lbm_s",
           "623.xalancbmk_s", "625.x264_s", "631.deepsjeng_s",
           "641.leela_s", "644.nab_s", "657.xz_s"]

processes = []
for bench in benches:
    try:
        os.chdir(spec_dir+bench+"/run/only_pna/run_peak_refspeed_mytest-64.0000")
    except FileNotFoundError:
        os.chdir(spec_dir+bench+"/run/only_pna/run_peak_refspeed_mytest-64.0001")

    processes.append(subprocess.Popen("python3 ~/spec_automation.py", shell=True))

    try:
        os.chdir(spec_dir+bench+"/run/run_peak_refspeed_mytest-64.0000")
    except FileNotFoundError:
        os.chdir(spec_dir+bench+"/run/run_peak_refspeed_mytest-64.0001")

    processes.append(subprocess.Popen("python3 ~/spec_automation.py", shell=True))

for p in processes:
    p.wait()
