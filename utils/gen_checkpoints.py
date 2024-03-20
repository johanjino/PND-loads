import subprocess
from subprocess import Popen
import sys
import os

bench = sys.argv[1]
run = int(bench[-1])
bench = bench[:-1]
spec_path = "/sim_home/luke/spec2017/"
gem5 = "/sim_home/luke/PND-Loads/gem5/"

os.chdir(spec_path+"benchspec/CPU/"+bench+"/run/run_peak_refspeed_mytest-64.0000")
specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
Popen.wait(specinvoke)
commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if line.startswith(b".")]
command = commands[run]
print(gem5+"build/ARM/gem5.fast --outdir=checkpoints."+str(run)+" "+gem5+"configs/example/se.py --cpu-type=NonCachingSimpleCPU --take-simpoint-checkpoint=/sim_home/luke/simpoints/"+bench.split('.')[1]+"."+str(run)+".simpts,/sim_home/luke/simpoints/"+bench.split('.')[1]+"."+str(run)+".weights,100000000,10000000 -c "+command.split()[0]+" --options=\""+' '.join(command.split()[1:])+"\" --mem-size=50GB")
