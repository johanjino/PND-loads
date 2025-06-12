import psutil
import os

spec_dir = "/work/johan/spec2017/benchspec/CPU/"
benches = ["600.perlbench_s", "605.mcf_s", "619.lbm_s",
           "625.x264_s", "631.deepsjeng_s",
           "641.leela_s", "657.xz_s", 
           "620.omnetpp_s", "602.gcc_s", "623.xalancbmk_s",
            "644.nab_s"] #"638.imagick_s"]

processes = []
for bench in benches:
    env = os.environ.copy()
    dump_file = spec_dir+bench+"/run/"+"InstructionsToInstrument2.txt"
    env["InstructionsToInstrument"] = dump_file
    os.chdir("/work/johan/spec2017/")
    if not os.path.exists(spec_dir+bench+"/run/"):
        raise FileNotFoundError

    if os.path.exists(dump_file):
        os.remove(dump_file)
        print("Deleted file...")

    while psutil.virtual_memory().percent > 60 and psutil.cpu_percent(interval=1) > 90: time.sleep(30)
    # processes.append(psutil.Popen(["./bin/runcpu --action runsetup --rebuild  --config myconfig --tune peak "+bench], env=env))
    processes.append(psutil.Popen(["./bin/runcpu", "--action", "runsetup", "--rebuild", "--config", "x86config", "--tune", "peak", bench], env=env))
    # processes.append(psutil.Popen(["python3", "-c", "import os; print('Process 1:', os.getenv('InstructionsToInstrument'))"], env=env))

for p in processes:
    p.wait()


for bench in benches:
    dump_file = spec_dir+bench+"/run/"+"InstructionsToInstrument2.txt"
    # Read and deduplicate lines
    with open(dump_file, 'r') as f:
        unique_lines = set(line.strip() for line in f if line.strip())

    # Overwrite the file with deduplicated content
    with open(dump_file, 'w') as f:
        for line in unique_lines:
            f.write(line + '\n')


