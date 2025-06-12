import os
import psutil
import subprocess
import time
import sys

AliasProfile = "AliasPGOInfoShift4Train" #+ str(sys.argv[1])

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


processes = []

for bench in benches:
    env = os.environ.copy()

    os.chdir("/work/johan/spec2017/")
    if not os.path.exists(spec_dir+bench+"/run/"):
        raise FileNotFoundError
    
    PGOfile = spec_dir+bench+"/run/"+ AliasProfile
    SVFResultsFile = spec_dir+bench+"/run/"+"SVFResults5.txt"
    env["AliasHintsPGOFile"] = PGOfile
    env["SVFResultsFile"] = SVFResultsFile
    while psutil.virtual_memory().percent > 60 and psutil.cpu_percent() > 90: time.sleep(30)
    p = psutil.Popen(["./bin/runcpu", "--action", "runsetup", "--rebuild", "--config", "myconfig", "--tune", "peak", bench], env=env)
    # p = psutil.Popen(["./bin/runcpu", "--action", "runsetup", "--size", "train", "--rebuild", "--config", "myconfig", "--tune", "peak", bench], env=env)
    # print("./bin/runcpu", "--action", "runsetup", "--rebuild", "--config", "myconfig", "--tune", "peak", bench)
    processes.append(p)

for p in processes:
    p.wait()
    
print("Completed compiling benchmarks.....")