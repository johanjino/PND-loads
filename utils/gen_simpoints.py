import subprocess
from subprocess import Popen
import sys
import os

clean = 'clean' in sys.argv
run_gem5 = 'gem5' in sys.argv
gen_bbvs = 'bbvs' in sys.argv
gen_simpoints = 'simpoint' in sys.argv
tag = sys.argv[-1]

spec = ["600.perlbench_s", "602.gcc_s", "605.mcf_s",
        "607.cactuBSSN_s", "619.lbm_s", "620.omnetpp_s",
        "623.xalancbmk_s", "625.x264_s", "631.deepsjeng_s",
        "638.imagick_s", "641.leela_s", "644.nab_s",
        "648.exchange2_s", "657.xz_s"]
spec_path = "/home/l50031074/spec2017/"
valgrind = "/home/l50031074/valgrind-new/bin/valgrind"
simpoint = "/home/l50031074/SimPoint/bin/simpoint"
gem5 = "/home/l50031074/PND-Loads/gem5/"

if clean:
    runcpu = subprocess.run([spec_path+"bin/runcpu", "--action", "clobber", "--config",
        "myconfig", *spec, "--define", "bits=64",
        "--define", "build_ncpus=100", "--tune", "peak"])
    runcpu = subprocess.run([spec_path+"bin/runcpu", "--action", "runsetup", "--config",
                    "myconfig", *spec, "--define", "bits=64",
        "--define", "build_ncpus=100", "--tune", "peak"])

benches = {}
bbvs = {}
for bench in spec:
    os.chdir(spec_path+"benchspec/CPU/"+bench+"/run/run_peak_refspeed_mytest-64.0000")
    specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
    commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if not line.startswith(b"#") and b"specinvoke" not in line and len(line) > 0]
    for c, command in enumerate(commands):
        bench_name = bench+"."+str(c)
        benches[bench_name] = command
        if gen_bbvs:
            valgrind_run = Popen([valgrind, "--tool=exp-bbv", "--bb-out-file=/work/luke_simpoints/bbvs/bb.out."+bench_name, command], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            bbvs[valgrind_run] = bench_name

    running_bbvs = {bbv: True for bbv in bbvs}

simpoints = {}
if gen_simpoints and gen_bbvs:
    while len(list(filter(lambda bbv: running_bbvs[bbv], running_bbvs))) > 0:
        for bbv in running_bbvs:
            if Popen.poll(bbv):
                running_bbvs[bbv] = False
                bench = bbvs[bbv]
                simpoints[Popen([simpoint, "-loadFVFile", "/work/luke_simpoints/bbvs/bb.out."+bench,
                                 "-k", "search", "-maxK", "30", "-saveSimpoints",
                                 "/work/luke_simpoints/simpoints/"+bench+".simpts", "-saveSimpointWeights",
                                 "/work/luke_simpoints/simpoints/"+bench+".weights"])] = bench
    running_simpoints = {sim: True for sim in simpoints}
elif gen_simpoints:
    for bench in benches:
        if "imagick" in bench: continue
        simpoints[Popen([simpoint, "-loadFVFile", "/work/luke_simpoints/bbvs/bb.out."+bench,
                            "-k", "search", "-maxK", "30", "-saveSimpoints",
                            "/work/luke_simpoints/simpoints/"+bench+".simpts", "-saveSimpointWeights",
                            "/work/luke_simpoints/simpoints/"+bench+".weights"])] = bench
if run_gem5 and gen_simpoints:
    while len(list(filter(lambda sim: running_simpoints[sim], running_simpoints))) > 0:
        for sim in simpoints:
            if Popen.poll(sim):
                running_simpoints[sim] = False
                bench = simpoints[sim]
                command = benches[bench]
                original_bench = bench.split(".")[0]+"."+bench.split(".")[1]
                os.chdir(spec_path+"benchspec/CPU/"+original_bench+"/run/run_peak_refspeed_mytest-64.0000")
                Popen(gem5+"build/ARM/gem5.fast "+gem5+"configs/example/se.py --cpu-type=AtomicSimpleCPU --caches --take-simpoint-checkpoint=/work/luke_simpoints/simpoints/"+bench+".simpts,/work/luke_simpoints/simpoints/"+bench+".weights,100000000,10000000 -c "+command.split()[0]+"--options=\""+' '.join(command.split()[1:])+"\"", shell=True)
elif run_gem5:
    gem5s = []
    for bench in benches:
        command = benches[bench]
        original_bench = bench.split(".")[0]+"."+bench.split(".")[1]
        os.chdir(spec_path+"benchspec/CPU/"+original_bench+"/run/run_peak_refspeed_mytest-64.0000")
        print(gem5+"build/ARM/gem5.fast --outdir=../only_pna/"+'.'.join(bench.split(".")[1:])+" "+gem5+"configs/example/se.py --cpu-type=NonCachingSimpleCPU --caches --take-simpoint-checkpoint=/work/luke_simpoints/simpoints/"+bench+".simpts,/work/luke_simpoints/simpoints/"+bench+".weights,100000000,10000000 -c ../only_pna/"+command.split()[0][3:]+" --options=\""+' '.join(command.split()[1:])+"\" --mem-size=50GB")
#        gem5s.append(g)
#    for g in gem5s:
#        Popen.wait(g)
