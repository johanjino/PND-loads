import subprocess
from subprocess import Popen
import sys
import os

clean = 'clean' in sys.argv
run_gem5 = 'gem5' in sys.argv
gen_bbvs = 'bbvs' in sys.argv
gen_simpoints = 'simpoint' in sys.argv

spec = ["600.perlbench_s", "602.gcc_s", "605.mcf_s",
        "607.cactuBSSN_s", "619.lbm_s", "620.omnetpp_s",
        "623.xalancbmk_s", "625.x264_s", "631.deepsjeng_s",
        "638.imagick_s", "641.leela_s", "644.nab_s",
        "648.exchange2_s", "657.xz_s"]
spec_path = "/home/l50031074/spec2017/"
gem5 = "/home/l50031074/PND-Loads/gem5/"

if clean:
    runcpu = subprocess.run([spec_path+"bin/runcpu", "--action", "clobber", "--config",
        "myconfig", *spec, "--define", "bits=64",
        "--define", "build_ncpus=100", "--tune", "peak"])
    runcpu = subprocess.run([spec_path+"bin/runcpu", "--action", "runsetup", "--config",
                    "myconfig", *spec, "--define", "bits=64",
        "--define", "build_ncpus=100", "--tune", "peak"])

for bench in spec:
    os.chdir(spec_path+"benchspec/CPU/"+bench+"/run/run_peak_refspeed_mytest-64.0000")
    specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
    commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if not line.startswith(b"#")]
    for c, command in enumerate(commands):
        print(gem5+"build/ARM/gem5.fast --outdir=checkpoints."+str(c)+" "+gem5+"configs/example/se.py --cpu-type=NonCachingSimpleCPU --take-simpoint-checkpoint=/work/luke_simpoints/simpoints/"+bench+".simpts,/work/luke_simpoints/simpoints/"+bench+".weights,100000000,10000000 -c "+command.split()[0]+" --options=\""+' '.join(command.split()[1:])+"\" --mem-size=50GB")
