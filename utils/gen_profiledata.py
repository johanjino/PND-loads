import os
import psutil
import subprocess
import time
import re

def setfilepath(new_file_path: str):
    os.chdir("/work/johan/PND-Loads/")

    # C++ source file path
    cpp_file = '/work/johan/PND-Loads/InstrumentationFunctionsAliasHints.cpp'

    # Regular expression pattern to match dynamic paths
    pattern = r'/work/johan/spec2017/benchspec/CPU/[^/]+/run/AliasHintsPGOFile.txt'

    # Read the C++ source file
    with open(cpp_file, 'r') as file:
        cpp_code = file.read()

    # Replace matched file paths dynamically
    modified_code = re.sub(pattern, new_file_path, cpp_code)

    # Write the modified code back to the same C++ file (overwrite the original)
    with open(cpp_file, 'w') as file:
        file.write(modified_code)
    
    os.system("./johan.sh compile_inst")

run_dir = "run_peak_refspeed_instrumentedforPGO-64.0000"
spec_dir = "/work/johan/spec2017/benchspec/CPU/"
benches = [
    "600.perlbench_s",
    "605.mcf_s",
    "619.lbm_s",
    # "625.x264_s",
    "631.deepsjeng_s",
    "641.leela_s",
    "657.xz_s", 
    "620.omnetpp_s",
    # "602.gcc_s", #Profile Empty
    "623.xalancbmk_s", #Too much mem during profiling
    "644.nab_s"
    ] #"638.imagick_s"]

processes = []

try:
    for bench in benches:
        env = os.environ.copy()
        dump_file = spec_dir+bench+"/run/"+"AliasHintsPGOFile.txt"
        setfilepath(dump_file)

        os.chdir("/work/johan/spec2017/")
        if not os.path.exists(spec_dir+bench+"/run/"):
            raise FileNotFoundError

        if os.path.exists(dump_file):
            os.remove(dump_file)
            print("Deleted file...")
        
        dump_file = spec_dir+bench+"/run/"+"InstructionsToInstrument.txt"
        env["InstructionsToInstrument"] = dump_file

        while psutil.virtual_memory().percent > 60 and psutil.cpu_percent() > 90: time.sleep(30)
        p = psutil.Popen(["./bin/runcpu", "--action", "runsetup", "--rebuild", "--config", "myinstrconfig", "--tune", "peak", bench], env=env)
        p.wait()

    print("\n\n\nFinished all instrumentation compilations")
    print(".........................................")
    print("Starting instrumentation....\n\n\n")

    for bench in benches:
        env = os.environ.copy()
        dump_file = spec_dir+bench+"/run/"+"AliasHintsPGOFile.txt"  

        env["AliasHintsPGOFile"] = dump_file

        if not os.path.exists(spec_dir+bench+"/run/"+run_dir):
            raise FileNotFoundError

        os.chdir(spec_dir+bench+"/run/"+run_dir)

        specinvoke = subprocess.run(["/work/johan/spec2017/bin/specinvoke", "-n"], stdout=subprocess.PIPE)
        commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if not line.startswith(b"#") and not b"specinvoke" in line]

        run_commands = [command.split('>')[0] for command in commands if command!=""] 

        # Join commands using '&&' to ensure they run sequentially in the same shell process
        command_str = " && ".join(run_commands)
        print()
        print(command_str)
        print()

        while psutil.virtual_memory().percent > 60 and psutil.cpu_percent() > 90: time.sleep(30)
        processes.append(psutil.Popen(command_str,  shell=True, env=env))
    
    for p in processes:
        p.wait()

except KeyboardInterrupt:
    print("\nKeyboardInterrupt detected! Terminating all processes...")
    
    # Kill all started processes
    for p in processes:
        try:
            p.terminate()  # Send SIGTERM
            p.wait(timeout=5)  # Wait for process to terminate
        except psutil.NoSuchProcess:
            pass  # Process already exited
        except psutil.TimeoutExpired:
            p.kill()  # Force kill if it didn't terminate in time

    print("All processes terminated. Exiting.")