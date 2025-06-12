import os
import collections
import sys
import re


# def filter_loads_by_threshold(loads, threshold):
#     result = []

#     for load_id, data in loads.items():
#         exec_total = data["exec_count"]
#         cycles = data["cycles"]

#         # Find if a dominant cycle exists (>95% of total)
#         dominant_cycle = None
#         for cycle, count in cycles.items():
#             if count / exec_total > 0.95:
#                 dominant_cycle = cycle
#                 break

#         if dominant_cycle is not None:
#             compare_cycle = dominant_cycle
#         elif 1001 in cycles and cycles[1001] / exec_total > 0.1:
#             compare_cycle = 1001
#         else:
#             compare_cycle = min(cycles.keys())
            
#         if compare_cycle > threshold and int(compare_cycle)!=1001:
#             result.append(load_id)

#         # if compare_cycle == 1001:
#         #     result.append(load_id)

#     return result

def filter_loads_by_threshold(loads, threshold):
    result = []

    for load_id, data in loads.items():
        exec_total = data["exec_count"]
        cycles = data["cycles"]

        # Calculate the total count of cycles greater than the threshold
        count_above_threshold = sum(count for cycle, count in cycles.items() if cycle > threshold)

        if count_above_threshold / exec_total >= 0.95:
            result.append(load_id)

    return result


# Store Distance
benches = [
    "600.perlbench_s", 
    "602.gcc_s",
    "605.mcf_s",
    "619.lbm_s",
    "620.omnetpp_s",
    "623.xalancbmk_s",
    "625.x264_s",
    "631.deepsjeng_s",
    "641.leela_s",
    "644.nab_s",
    "657.xz_s", 
    ] #"638.imagick_s"]

for bench in benches:

    # bench = "623.xalancbmk_s"
    # run = "0"

    aggregated_loads = {}
    run_count = 0
    for run in ["0", "1", "2", "3", "4"]:
        raw_results_dir = "/work/johan/results/train/store_distance/a14/" + bench.split(".")[1].rstrip("_s") + "." + run +"/"
        try:
            os.chdir(raw_results_dir)
            print(bench.split(".")[1].rstrip("_s") + "." + run, end=" ")
        except FileNotFoundError:
            continue
        
        run_count+=1

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
                load_id, exec_count = parts[0].split("-")
                exec_count = float(exec_count) #*weight
                cycle_data = parts[1:]

                total_exec_count = 0.0
                cycle_map = {}

                for item in cycle_data:
                    if ':' in item:
                        cycle, c = item.split(':')
                        cycle = int(cycle)
                        c = float(c) #*weight
                        cycle_map[cycle] = c
                        total_exec_count += c
                
                assert abs(exec_count-total_exec_count) < 1e-8, f"{exec_count} vs {total_exec_count}"

                assert load_id not in loads

                # assert load_id not in loads
                loads[load_id] = {
                    "exec_count": exec_count,
                    "cycles": cycle_map
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

    # print("Found", run_count)

    threshold = int(sys.argv[1])

    print("Threshold = ", threshold)
    good_loads = filter_loads_by_threshold(aggregated_loads, threshold)
    # print(len(good_loads))

    directory = "/work/johan/PND-Loads/addr_files/sd_train_"+ str(threshold) + "_full"
    if not os.path.exists(directory):
        os.makedirs(directory)
    # filename = directory + "/" + bench +"_"+run
    filename = directory + "/" + bench
    with open(filename, 'w') as f:
        for load_id in sorted(good_loads):
            f.write(load_id + '\n')