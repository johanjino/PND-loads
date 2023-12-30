from subprocess import Popen
import os

gem5 = "~/PND-Loads-old/gem5/"
cpu_config = gem5+"src/cpu/o3/BaseO3CPU.py"

SSIT_range = [32, 128, 352, 567, 800, 1024]
LFST_range = [32, 128, 1024]
clear_period_ratio_range = [122, 244, 488, 976]
standard_clear_period_ratio = 244

def update_config(sed_command):
    p = Popen(sed_command + cpu_config, shell=True)
    Popen.wait(p)

results_base_dir = "~/spec_results/lfst_ssit_search/"
for lfst_size in LFST_range:
    for ssit_size in SSIT_range:
        lfst_sed = r"sed -i 's/\(LFSTSize = Param.Unsigned(\)\([0-9]\+\)\(, \".*\"\)/\1"+str(lfst_size)+r"\3/' "
        update_config(lfst_sed)
        ssit_sed = r"sed -i 's/\(SSITSize = Param.Unsigned(\)\([0-9]\+\)\(, \".*\"\)/\1"+str(ssit_size)+r"\3/' "
        update_config(ssit_sed)
        clear_period_sed = r"sed -i 's/\(store_set_clear_period = Param.Unsigned(\)\([0-9]\+\)\(,.*\)/\1"+str(ssit_size*standard_clear_period_ratio)+r"\3/' "
        update_config(clear_period_sed)
        os.chdir(gem5)
        build = Popen("scons build/ARM/gem5.fast -j 100", shell=True)
        Popen.wait(build)
        os.chdir("~/PND-Loads/utils")
        run = Popen("python3 run_all_chkpts.py", shell=True)
        Popen.wait(run)
        results_dir = "_".join([str(i) for i in [ssit_size, lfst_size, standard_clear_period_ratio]])
        os.makedirs(results_base_dir+results_dir, exist_ok=True)
        os.chdir("~/PND-Loads/utils")
        aggregate = Popen("python3 aggregate_all_stats.py " + results_base_dir+results_dir, shell=True)
        Popen.wait(aggregate)

results_base_dir = "~/spec_results/clear_period_ssit_search/"
for clear_period_ratio in clear_period_ratio_range:
    for ssit_size in SSIT_range:
        lfst_size = ssit_size
        lfst_sed = r"sed -i 's/\(LFSTSize = Param.Unsigned(\)\([0-9]\+\)\(, \".*\"\)/\1"+str(lfst_size)+r"\3/' "
        update_config(lfst_sed)
        ssit_sed = r"sed -i 's/\(SSITSize = Param.Unsigned(\)\([0-9]\+\)\(, \".*\"\)/\1"+str(ssit_size)+r"\3/' "
        update_config(ssit_sed)
        clear_period_sed = r"sed -i 's/\(store_set_clear_period = Param.Unsigned(\)\([0-9]\+\)\(,.*\)/\1"+str(ssit_size*clear_period_ratio)+r"\3/' "
        update_config(clear_period_sed)
        os.chdir(gem5)
        build = Popen("scons build/ARM/gem5.fast -j 100", shell=True)
        Popen.wait(build)
        os.chdir("~/PND-Loads/utils")
        run = Popen("python3 run_all_chkpts.py", shell=True)
        Popen.wait(run)
        results_dir = "_".join([str(i) for i in [ssit_size, lfst_size, clear_period_ratio]])
        os.makedirs(results_base_dir+results_dir, exist_ok=True)
        os.chdir("~/PND-Loads/utils")
        aggregate = Popen("python3 aggregate_all_stats.py " + results_base_dir+results_dir, shell=True)
        Popen.wait(aggregate)
