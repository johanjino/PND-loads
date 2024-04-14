import subprocess
from subprocess import Popen
import sys
import os

bench = sys.argv[1]
run = int(bench[-1])
bench = bench[:-1]
chkpt = str(int(sys.argv[2])+1)
offset = sys.argv[3]
spec_path = "/sim_home/luke/spec2017/"
gem5 = "/sim_home/luke/PND-Loads/gem5/"

os.chdir(spec_path+"benchspec/CPU/"+bench+"/run/run_peak_refspeed_mytest-64.0000")
specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
Popen.wait(specinvoke)
commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if line.startswith(b".")]
command = commands[run]
p = Popen(gem5+"build/ARM/gem5.fast --outdir=checkpoints."+str(run)+".adjusted "+gem5+"configs/example/se.py --cpu-type=NonCachingSimpleCPU --restore-simpoint-checkpoint -r "+chkpt+" --checkpoint-dir checkpoints."+str(run)+" --take-simpoint-checkpoint=/sim_home/luke/simpoints/"+bench+"."+str(run)+".simpts.adjusted_no_offset,/sim_home/luke/simpoints/"+bench+"."+str(run)+".weights.adjusted_no_offset,100000000,10000000 -c "+command.split()[0]+" --options=\""+' '.join(command.split()[1:])+"\" --mem-size=50GB", shell=True)
p.wait()
