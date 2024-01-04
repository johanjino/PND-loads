import os
import subprocess
from subprocess import Popen

spec = ["600.perlbench_s", "602.gcc_s", "605.mcf_s",
        "607.cactuBSSN_s", "619.lbm_s", "620.omnetpp_s",
        "623.xalancbmk_s", "625.x264_s", "631.deepsjeng_s",
        "638.imagick_s", "641.leela_s", "644.nab_s",
        "648.exchange2_s", "657.xz_s"]
spec_path = "/home/l50031074/spec2017/"
valgrind = "/home/l50031074/valgrind-new/bin/valgrind"
simpoint = "/home/l50031074/SimPoint/bin/simpoint"
procs = []
bench_names = []

benches = {}
bbvs = {}
for bench in spec:
    os.chdir(spec_path+"benchspec/CPU/"+bench+"/run/run_peak_refspeed_mytest-64.0000")
    specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
    commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if not line.startswith(b"#") and b"specinvoke" not in line and len(line) > 0]
    for c, command in enumerate(commands):
        bench_name = bench+"."+str(c)
        bench_names.append(bench_name)
        p = Popen([valgrind, "--tool=exp-bbv", "--bb-out-file=/work/luke_simpoints/bbvs/bb.out."+bench_name, command], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        procs.append(p)

for p in procs:
    Popen.wait(p)

for bench in bench_names:
    Popen([simpoint, "-loadFVFile", "/work/luke_simpoints/bbvs/bb.out."+bench,
            "-k", "search", "-maxK", "30", "-saveSimpoints",
            "/work/luke_simpoints/simpoints/"+bench+".simpts", "-saveSimpointWeights",
            "/work/luke_simpoints/simpoints/"+bench+".weights"])
