import os
import sys
import subprocess
from subprocess import Popen
import psutil
import random
import time
import re


#run from base spec dir
base_dir = os.getcwd() # = /work/muke/checkpoints/benchmark
base_run = False
run_type = sys.argv[1]
addr_file_type = sys.argv[2]
if addr_file_type == "base": base_run = True
cpu_model = sys.argv[3]
spec_path = "/work/johan/spec2017/"
gem5 = "/work/johan/PND-Loads/gem5/"
results_dir = "/work/johan/results/"+run_type+"/"+addr_file_type+"/"+cpu_model+"/"
addr_file_dir = "/work/johan/PND-Loads/addr_files/"
# addr_file_dir = "/work/muke/PND-Loads/addr_files/"
benchmark = base_dir.split("/")[4]
run_dir = spec_path+"benchspec/CPU/"+benchmark+"/run/run_peak_train_withPGO-64.0000/"
os.chdir(run_dir)
specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if not line.startswith(b"#") and "specinvoke exit" not in line.decode() and len(line.decode())!=0]

os.chdir(base_dir)
random.seed(sum(ord(c) for c in base_dir))


def resources_available(max_cpu_percentage=80, max_mem_percent=50):
    cpu_util = psutil.cpu_percent(5)
    mem = psutil.virtual_memory()[2]
    return cpu_util <= max_cpu_percentage and mem <= max_mem_percent

procs = []

# iterate over all train input commands
for run_number in range(len(commands)):
    command = commands[run_number]
    benchmark_name = benchmark.split("_")[0].split(".")[1]
    out_dir = results_dir + benchmark_name + "." + str(run_number)
    if not os.path.exists(out_dir): os.makedirs(out_dir) #create the parent directories for gem5 stats dir if needed
    binary = run_dir+command.split()[0]
    os.chdir(run_dir)
    run = "ADDR_FILE=/work/johan/PND-Loads/addr_files/must_alias_count/"+ benchmark + " " + gem5 +"build/ARM/gem5.fast --outdir="+out_dir+" "+gem5+"configs/deprecated/example/se.py --cpu-type=NonCachingSimpleCPU --mem-size=50GB -c "+binary+" --options='"+" ".join(command.split()[1:]).split('>')[0].strip()+"'"
    
    while(not resources_available()):
        time.sleep(random.randint(1, 60))
    
    p = Popen(run, shell=True)
    procs.append(p)

for p in procs:
    code = Popen.wait(p)
    print("Reached!")
    if code is not None and code != 0: print(p.args); exit(1)