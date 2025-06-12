import os
import collections
import sys
import re
import subprocess

# Store Distance
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


def get_source_location(address, binary_path):
    """
    Uses llvm-symbolizer to get the source file, line, and column for a given address.

    Args:
        address (int): The address of the instruction.
        binary_path (str): The path to the executable binary.

    Returns:
        list of ste: A string representing the source location (e.g., "av.c:50:2"), or empty on error.
    """
    address_hex = f"0x{address:x}"
    cmd = [
        "llvm-symbolizer",
        f"--obj={binary_path}",
        # "--no-inlines",  # Add --no-inlines
        # "--basenames",  # Add --basenames
        "--relativenames", # Add --relativenames
        "-a",
        address_hex,
    ]
    try:
        process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        stdout, stderr = process.communicate()
        results = []
        if process.returncode == 0 and stderr == "": # changed the condition to check if the return code is 0 and stderr is empty
            # The output of llvm-symbolizer is in the format "file.c:line:column"
            for data in stdout.split("\n"):
                match = re.search(r"([^:]+:[^:]+:[^:]+)", data) # refined regex
                if match:
                    # print(match)
                    results.append(match.group(1))
                else:
                    continue
            return results
        else:
            print(f"Error running llvm-symbolizer for address {address_hex}:")
            print(f"Command: {cmd}")
            print(f"Stdout: {stdout}")
            print(f"Stderr: {stderr}")
            return results
    except Exception as e:
        print(f"Exception running llvm-symbolizer for address {address_hex}: {e}")
        return results

def process_load_map(loads, binary_path):
    """
    Processes the load map, converts addresses to source locations, and formats the output string.

    Args:
        loads (dict): The load map.
        binary_path (str): The path to the executable binary.

    Returns:
        full string to store: A dictionary with load_id as key and string of  load and store locations with counts.
    """
    final_string = ""
    for load_id, load_data in loads.items():
        load_location = get_source_location(load_id, binary_path)
        if not load_location:
            print(f"Could not get source location for load_id: {load_id}")
            exit()

        load_output_strings = []
        for load in load_location:
            load_output_strings.append(f"{load} {load_data['exec_count']}")
        
        store_output_string = ""
        for store_id, alias_count in load_data['alias_map'].items():
            store_location = get_source_location(store_id, binary_path)
            if not store_location:
                print(f"Could not get source location for store_id: {store_id}")
                exit()
            for store_loc in store_location:
                store_output_string += f" {store_loc} {alias_count}"
        
        for load_string in load_output_strings:
            final_string +=  load_string + " " + store_output_string + "\n"
    return final_string


for bench in benches:

    # bench = "623.xalancbmk_s"
    # run = "0"
    run_count = 0

    aggregated_loads = {}

    for run in ["0", "1", "2", "3", "4"]:


        raw_results_dir = "/work/johan/results/train/alias_profile/a14/" + bench.split(".")[1].rstrip("_s") + "." + run
        results_spec_dir = "/work/johan/spec2017/benchspec/CPU/" + bench + "/run/"
        name_binary = bench.split(".")[1].split("_")[0]+"_s_peak.withPGO-64"
        if bench == "602.gcc_s":
            name = "sgcc_peak.withPGO-64"
            name_binary = "sgcc_peak.withPGO-64"
        binary = "/work/johan/spec2017/benchspec/CPU/"+bench+"/run/run_peak_refspeed_withPGO-64.0000/"+name_binary
        
        try:
            os.chdir(raw_results_dir)
            print(bench.split(".")[1].rstrip("_s") + "." + run)
        except FileNotFoundError:
            continue
        
        run_count+=1


        #pass over each .out and aggregate weighted values
        aggregated_loads = {}


        stats_file = os.path.join(raw_results_dir, "stats.txt")
        f = open(stats_file)
        lines = f.readlines()
        f.close()

        loads = {}
        ignores = re.compile(r'^---|^$')
        for line in lines:
            #ignore empty lines and lines starting with "---"
            #regex would catch empty lines but we want to count number of begin/end markers for correctness checking
            if len(line.strip()) == 0: continue
            if not ignores.match(line):
                if re.search(r'[a-zA-Z]', line):
                    continue

                parts = line.strip().split()
                load_id, exec_count = int(parts[0]), parts[1]
                exec_count = int(exec_count)
                alias = parts[2:]
                alias_map = {}
                for item in alias:
                    if ':' in item:
                        store, alias_count = item.split(':')
                        store = int(store)
                        alias_count = int(alias_count)
                        alias_map[store] = alias_count

                loads[load_id] = {
                    "exec_count": exec_count,
                    "alias_map": alias_map
                }

        #collecting this passing over to clobber warmup values
        for load in loads:
            if load not in aggregated_loads:
                aggregated_loads[load] = loads[load]
            else:
                aggregated_loads[load]["exec_count"] += loads[load]["exec_count"]
                for cycle, count in loads[load]["cycles"].items():
                    if cycle in aggregated_loads[load]["cycles"]:
                        aggregated_loads[load]["cycles"][cycle] += count
                    else:
                        aggregated_loads[load]["cycles"][cycle] = count
        

    output_profile_info = process_load_map(aggregated_loads, binary)

    filename = "/work/johan/spec2017/benchspec/CPU/"+bench+"/run/"+"AliasPGOInfoShift4Train"
    with open(filename, 'w') as f:
        f.write(output_profile_info)

    print("Found", run_count)

   



