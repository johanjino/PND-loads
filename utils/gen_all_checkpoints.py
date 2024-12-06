import subprocess
import os
import sys
import time

spec_path = "/work/muke/spec2017/"
benches = ["600.perlbench_s", "605.mcf_s", "619.lbm_s",
           "625.x264_s", "631.deepsjeng_s",
           "641.leela_s", "657.xz_s",
           "620.omnetpp_s", "602.gcc_s", "623.xalancbmk_s",
           "644.nab_s"] #"638.imagick_s"]
procs = []

for bench in benches:
    os.chdir(spec_path+"benchspec/CPU/"+bench+"/run/run_peak_refspeed_mytest-64.0000")
    specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
    commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if line.startswith(b".")]
    for run in range(len(commands)):
        p = subprocess.Popen("python3 /work/muke/PND-Loads/utils/gen_checkpoints.py "+bench+str(run), shell=True)
        procs.append(p)

active_procs = procs.copy()

while active_procs:
    for proc in active_procs[:]:
        code = proc.poll()
        if code is not None:
            active_procs.remove(proc)
            if code != 0:
                print("Crash: ", proc.args)
    time.sleep(60*5)
