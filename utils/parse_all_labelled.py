import subprocess
import os
import sys

spec_dir = "/work/johan/spec2017/benchspec/CPU/"
# benches = [
#     # "600.perlbench_s", 
#     "605.mcf_s",
#     "619.lbm_s",
#     # "625.x264_s",
#     "631.deepsjeng_s",
#     "641.leela_s",
#     "657.xz_s", 
#     # "620.omnetpp_s",
#     # "602.gcc_s",
#     # "623.xalancbmk_s",
#     "644.nab_s"
#     ] #"638.imagick_s"]

benches = [
    "600.perlbench_s", 
    "605.mcf_s",
    "619.lbm_s",
    "625.x264_s",
    "631.deepsjeng_s",
    "641.leela_s",
    "657.xz_s", 
    "620.omnetpp_s",
    "602.gcc_s",
    "623.xalancbmk_s",
    "644.nab_s"
    ] #"638.imagick_s"]

addr_file_type = sys.argv[1]
os.makedirs("/work/johan/PND-Loads/addr_files/"+addr_file_type, exist_ok=True)

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
    print(bench)
    name = bench.split(".")[1].split("_")[0]+"_s_peak.withPGO-64"
    name_binary = bench.split(".")[1].split("_")[0]+"_s_peak.withPGObinary-64"
    if bench == "602.gcc_s":
        name = "sgcc_peak.withPGO-64"
        name_binary = "sgcc_peak.withPGObinary-64"

    # os.chdir(spec_dir+bench+"/run/run_peak_refspeed_withPGO-64.0000")

    binary = spec_dir+bench+"/run/run_peak_refspeed_withPGObinary-64.0000/"+name_binary
    # binary = "/work/muke/spec2017/benchspec/CPU/"+bench+"/run/run_peak_refspeed_mytest-64.0000/"+"xz_s_peak.mytest-64"
    # labelled_binary = spec_dir+bench+"/run/labelled/run_peak_refspeed_withPGO-64.0000/"+name
    labelled_binary = spec_dir+bench+"/run/run_peak_refspeed_withPGO-64.0000/"+name

    p = subprocess.Popen("python3 /work/johan/PND-Loads/utils/parse_labelled_binary.py "+labelled_binary+" "+binary+" "+addr_file_type, shell=True)
    p.wait()




# spec_dir = "/work/muke/spec2017/benchspec/CPU/"
# benches = [
#     # "600.perlbench_s", 
#     # "605.mcf_s",
#     # "619.lbm_s",
#     # "625.x264_s",
#     # "631.deepsjeng_s",
#     # "641.leela_s",
#     "657.xz_s", 
#     # "620.omnetpp_s",
#     # "602.gcc_s",
#     # "623.xalancbmk_s",
#     # "644.nab_s"
#     ] #"638.imagick_s"]

# addr_file_type = sys.argv[1]
# os.makedirs("/work/johan/PND-Loads/addr_files/"+addr_file_type, exist_ok=True)

# if len(sys.argv) > 2:
#     sub_benches = []
#     for bench in sys.argv[2:]:
#         if bench in benches:
#             sub_benches.append(bench)
#         else:
#             print("Unknown benchmark: ", bench)
#             exit(1)
#     benches = sub_benches

# for bench in benches:

#     name = bench.split(".")[1].split("_")[0]+"_s_peak.mytest-64"
#     if bench == "602.gcc_s":
#         name = "sgcc_peak.mytest-64"

#     os.chdir(spec_dir+bench+"/run/run_peak_refspeed_mytest-64.0000")

#     binary = spec_dir+bench+"/run/run_peak_refspeed_mytest-64.0000/"+name
#     labelled_binary = spec_dir+bench+"/run/stack-only/run_peak_refspeed_mytest-64.0000/"+name

#     p = subprocess.Popen("python3 /work/johan/PND-Loads/utils/parse_labelled_binary.py "+labelled_binary+" "+binary+" "+addr_file_type, shell=True)
#     p.wait()
