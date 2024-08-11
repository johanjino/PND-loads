import os
import argparse
import subprocess

addr_file_dir = "/work/muke/PND-Loads/addr_files/"
cpu_model_dir = "/work/muke/PND-Loads/cpu_models/"
gem5_dir = "/work/muke/PND-Loads/dev-gem5/"

parser = argparse.ArgumentParser(prog='run_models', description='run over multiple addr files and cpu models')

parser.add_argument('--addr-types', type=str, required=True)
parser.add_argument('--cpu-models', type=str, required=True)
args = parser.parse_args()

addr_types = args.addr_types.split(',')
cpu_models = args.cpu_models.split(',')
if addr_types[0] == 'all': addr_types = [addr_type for addr_type in os.listdir(addr_file_dir)]
elif addr_types[0] == 'all-with-base': addr_types = [addr_type for addr_type in os.listdir(addr_file_dir)] + ['base']
if cpu_models[0] == 'all': cpu_models = [model.split('.')[0] for model in os.listdir(cpu_moddel_dir)]

os.chdir(gem5_dir)

#run all models on base first to generate comparison results
if 'base' in addr_types:
    for model in cpu_models:
        cp = subprocess.Popen("cp "+cpu_model_dir+model+".py src/cpu/o3/BaseO3CPU.py", shell=True)
        if type(cp) != subprocess.CompletedProcess: cp.wait()
        scons = subprocess.Popen("scons build/ARM/gem5.fast -j 28 --with-lto", shell=True)
        scons.wait()
        run = subprocess.Popen("python3 /work/muke/PND-Loads/utils/run_all_chkpts.py base "+model+" with_base" , shell=True)
        run.wait()

addr_types = [a for a in addr_types if a != 'base']

for model in cpu_models:
    cp = subprocess.Popen("cp "+cpu_model_dir+model+".py src/cpu/o3/BaseO3CPU.py", shell=True)
    if type(cp) != subprocess.CompletedProcess: cp.wait()
    scons = subprocess.Popen("scons build/ARM/gem5.fast -j 28 --with-lto", shell=True)
    scons.wait()
    for addr_type in addr_types:
       run = subprocess.Popen("python3 /work/muke/PND-Loads/utils/run_all_chkpts.py "+addr_type+" "+model+" without_base", shell=True)
       run.wait()
