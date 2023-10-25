import sys
import os
import subprocess
from subprocess import Popen

#run from base spec dir
base_dir = os.getcwd()
spec_path = "/home/l50031074/spec2017/"
gem5 = "/home/l50031074/PND-Loads/gem5/"
benchmark = base_dir.split("/")[6]
specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if not line.startswith(b"#")]
scaled_up = True

# iterate over all items in the current directory
for out_dir in os.listdir(base_dir):
    if os.path.isdir(out_dir) and out_dir.startswith(benchmark.split(".")[1].split("_")[0]+".") and out_dir[-1].isdigit():
        command = commands[int(out_dir[-1])]
        cpt_number = 0
        for cpt_dir in os.listdir(os.path.join(base_dir, out_dir)):
            # if this cpt_dir is a directory and its name starts with 'cpt.'
            if os.path.isdir(os.path.join(base_dir, out_dir, cpt_dir)) and cpt_dir.startswith('cpt.'):
                cpt_number += 1
                O3outdir = out_dir+"/"+str(cpt_number)+".out"
                binary = spec_path+"benchspec/CPU/"+benchmark+"/run/run_peak_refspeed_mytest-64.0000/"+command.split()[0]
                run = gem5+"build/ARM/gem5.fast --outdir="+O3outdir+" "+gem5+"configs/example/se.py --cpu-type=DerivO3CPU --caches --restore-simpoint-checkpoint -r "+str(cpt_number)+" --checkpoint-dir "+out_dir+" --restore-with-cpu=NonCachingSimpleCPU --mem-size=50GB -c "+binary+" --options=\""+' '.join(command.split()[1:])+"\""
                if scaled_up:
                    run += " --l1d_size=128KiB --l1i_size=128KiB --l2_size=4MB"
                p = Popen(run, shell=True)
                Popen.wait(p)
