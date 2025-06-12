import subprocess
from subprocess import Popen
import sys
import os

bench = sys.argv[1]
run = int(bench[-1])
bench = bench[:-1]
spec_path = "/work/muke/spec2017/"
gem5 = "/work/muke/PND-Loads/gem5-gen/"
checkpoint_path = "/mnt/data/checkpoints/"+bench

os.chdir(spec_path+"benchspec/CPU/"+bench+"/run/run_peak_refspeed_mytest-64.0000")
specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if line.startswith(b".")]
command = commands[run].split('>')[0]
bench_name = bench+"."+str(run)
subprocess.run(gem5+"build/ARM/gem5.fast --outdir="+checkpoint_path+"/checkpoints."+str(run)+" "+gem5+"configs/deprecated/example/se.py --cpu-type=NonCachingSimpleCPU --take-simpoint-checkpoint=/work/muke/simpoints/"+bench_name+".simpts_first,/work/muke/simpoints/"+bench_name+".weights_first,100000000,10000000 -c "+command.split()[0]+" --options=\""+' '.join(command.split()[1:])+"\" --mem-size=50GB 2>&1 > "+bench_name+".out 2>&1", shell=True, check=True)
