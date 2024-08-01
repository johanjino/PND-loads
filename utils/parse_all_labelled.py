import subprocess
import os
import sys

spec_dir = "/work/muke/spec2017/benchspec/CPU/"
benches = ["600.perlbench_s", "605.mcf_s", "619.lbm_s",
           "625.x264_s", "631.deepsjeng_s",
           "641.leela_s", "657.xz_s", 
           "620.omnetpp_s", "602.gcc_s"]
           #"638.imagick_s", "644.nab_s"]
addr_file_type = sys.argv[1]

if len(sys.argv) > 2:
    sub_benches = []
    for bench in sys.argv[2:]:
        if bench in benches:
            sub_benches.append(bench)
        else:
            print("Unknown benchmark: ", bench)
            exit(1)
    benches = sub_benches

for bench in benches:

    name = bench.split(".")[1].split("_")[0]

    os.chdir(spec_dir+bench+"/run/run_peak_refspeed_mytest-64.0000")

    binary = name+"_s_peak.mytest-64"
    if bench == "602.gcc_s":
        binary = "sgcc_peak.mytest-64"

    labelled_binary = "../labelled/run_peak_refspeed_mytest-64.0000/"+binary

    p = subprocess.Popen("python3 /work/muke/PND-Loads/utils/parse_labelled_binary.py "+labelled_binary+" "+binary+" "+addr_file_type, shell=True)
    p.wait()
