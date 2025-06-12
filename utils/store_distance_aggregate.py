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
    for run in ["0", "1", "2"]:
    # for run in ["0"]:

        aggregated_loads = {}
        raw_results_dir = "/work/johan/results/store_distance/empty/a14/" + bench.split(".")[1].rstrip("_s") + "." + run + "/raw/"
        try:
            os.chdir(raw_results_dir)
            print(bench.split(".")[1].rstrip("_s") + "." + run, end=" ")
        except FileNotFoundError:
            continue
        
        run_count+=1


        #pass over each .out and aggregate weighted values
        for dirname in os.listdir("."):
            if os.path.isdir(dirname) and dirname[0].isdigit() and dirname.split('.')[1] == 'out':
                stats_file = os.path.join(dirname, "stats.txt")
                chkpt_number = int(dirname.split('.')[0])
                cpt_dir = "/mnt/data/checkpoints/"+bench+"/checkpoints."+run
                for cpt in os.listdir(cpt_dir):
                    if cpt.startswith("cpt.") and int(cpt.split('_')[1]) == (chkpt_number-1):
                        weight = float(cpt.split('_')[5])
                f = open(stats_file)
                lines = f.readlines()
                if len(lines) == 0:
                    print("Checkpoint "+str(chkpt_number)+" has empty stats file!")
                    continue
                    # exit(1)
                loads = {}
                ignores = re.compile(r'^---|^$')
                count = 0
                for line in lines:
                    #ignore empty lines and lines starting with "---"
                    #regex would catch empty lines but we want to count number of begin/end markers for correctness checking
                    if len(line.strip()) == 0: continue
                    if not ignores.match(line):
                        if re.search(r'[a-zA-Z]', line):
                            continue

                        parts = line.strip().split()
                        load_id, exec_count = parts[0].split("-")
                        exec_count = float(exec_count) *weight
                        cycle_data = parts[1:]

                        total_exec_count = 0.0
                        cycle_map = {}

                        for item in cycle_data:
                            if ':' in item:
                                cycle, c = item.split(':')
                                cycle = int(cycle)
                                c = float(c) *weight
                                cycle_map[cycle] = c
                                total_exec_count += c
                        
                        assert abs(exec_count-total_exec_count) < 1e-8, f"{exec_count} vs {total_exec_count}"

                        # assert load_id not in loads
                        loads[load_id] = {
                            "exec_count": exec_count,
                            "cycles": cycle_map
                        }
                    else:
                        count+=1
                f.close()

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

        for threshold in [30]:
            # print("Threshold = ", threshold)
            good_loads = filter_loads_by_threshold(aggregated_loads, threshold)
            print(len(good_loads))
            # directory = "/work/johan/PND-Loads/addr_files/store_distance_" + str(threshold)
            directory = "/work/johan/PND-Loads/addr_files/sd_30_full"
            if not os.path.exists(directory):
                os.makedirs(directory)
            filename = directory + "/" + bench +"_"+run
            with open(filename, 'w') as f:
                for load_id in sorted(good_loads):
                    f.write(load_id + '\n')

    # print(good_loads)
    # print(len(aggregated_loads))
    # print(len(good_loads))
    # total = 0
    # for load in good_loads:
    # #     print(aggregated_loads[load])
    #     total += aggregated_loads[load]["exec_count"]
    # print(total)
    # try:
    #     for key, val in aggregated_loads["4461628"]["cycles"].items():
    #         print(key, val)
    # except:
    #     pass
    # 28513503
    # 29838643




# -----------------------------------------------------------------------------------------------------



# import os
# import collections
# import glob
# import sys
# import re

# # Store Distance
# benches = [
#     "600.perlbench_s", 
#     # "605.mcf_s",
#     # "619.lbm_s",
#     "625.x264_s",
#     # "631.deepsjeng_s",
#     # "641.leela_s",
#     "657.xz_s", 
#     # "620.omnetpp_s", # checkpoint 8 empty?
#     # "602.gcc_s", # binary missing lol
#     "623.xalancbmk_s",
#     # "644.nab_s"
#     ] #"638.imagick_s"]

# for bench in benches:
#     print(bench.split(".")[1].rstrip("_s"))

#     base_path = "/work/johan/results/store_distance/train/m4-0"
#     pattern = os.path.join(base_path, "*", "raw", "*", "stats.txt")

#     found_count = 0
#     aggregated_loads = {}
#     for filepath in glob.glob(pattern):
#         if bench.split(".")[1].rstrip("_s") in filepath:
#             found_count+=1
#             f = open(filepath)
#             lines = f.readlines()
#             loads = {}
#             ignores = re.compile(r'^---|^$')
#             for line in lines:
#                 if len(line.strip()) == 0: continue
#                 if not ignores.match(line):
#                     if re.search(r'[a-zA-Z]', line):
#                         continue

#                     parts = line.strip().split()
#                     load_id, exec_count = parts[0].split("-")
#                     exec_count = float(exec_count)
#                     cycle_data = parts[1:]

#                     total_exec_count = 0.0
#                     cycle_map = {}

#                     for item in cycle_data:
#                         if ':' in item:
#                             cycle, c = item.split(':')
#                             cycle = int(cycle)
#                             c = float(c)
#                             cycle_map[cycle] = c
#                             total_exec_count += c
                    
#                     assert abs(exec_count-total_exec_count) < 1e-8, f"{exec_count} vs {total_exec_count}"

#                     loads[load_id] = {
#                         "exec_count": exec_count,
#                         "cycles": cycle_map
#                     }
#             f.close()

#             #collecting this passing over to clobber warmup values
#             for load in loads:
#                 if load not in aggregated_loads:
#                     aggregated_loads[load] = loads[load]
#                 else:
#                     aggregated_loads[load]["exec_count"] += loads[load]["exec_count"]
#                     for cycle, count in loads[load]["cycles"].items():
#                         if cycle in aggregated_loads[load]["cycles"]:
#                             aggregated_loads[load]["cycles"][cycle] += count
#                         else:
#                             aggregated_loads[load]["cycles"][cycle] = count
    
#     print("Found ", found_count, " train files")
            


#     def filter_loads_by_threshold(loads, threshold):
#         result = []

#         for load_id, data in loads.items():
#             exec_total = data["exec_count"]
#             cycles = data["cycles"]

#             # Find if a dominant cycle exists (>95% of total)
#             dominant_cycle = None
#             for cycle, count in cycles.items():
#                 if count / exec_total > 0.95:
#                     dominant_cycle = cycle
#                     break

#             if dominant_cycle is not None:
#                 compare_cycle = dominant_cycle
#             # elif 1001 in cycles and cycles[1001] / exec_total > 0.1:
#                 # compare_cycle = 1001
#             else:
#                 compare_cycle = min(cycles.keys())
                
#             if compare_cycle > threshold: #and int(compare_cycle)!=1001:
#                 result.append(load_id)

#         return result

#     # def filter_loads_by_threshold(loads, threshold):
#     #     result = []

#     #     for load_id, data in loads.items():
#     #         exec_total = data["exec_count"]
#     #         cycles = data["cycles"]

#     #         # Calculate the total count of cycles greater than the threshold
#     #         count_above_threshold = sum(count for cycle, count in cycles.items() if cycle > threshold and cycle!=1001)

#     #         if count_above_threshold / exec_total >= 0.95:
#     #             result.append(load_id)

#     #     return result




#     # for key in aggregated_loads:
#         # print(aggregated_loads[key])


#     good_loads = filter_loads_by_threshold(aggregated_loads, 200)
#     print(len(good_loads))
#     directory = "/work/johan/PND-Loads/addr_files/store_distance_train_2001"
#     if not os.path.exists(directory):
#         os.makedirs(directory)
#     filename = directory + "/" + bench
#     with open(filename, 'w') as f:
#         for load_id in sorted(good_loads):
#             f.write(load_id + '\n')

#     # print(good_loads)
#     # print(len(aggregated_loads))
#     # print(len(good_loads))
#     # total = 0
#     # for load in good_loads:
#     # #     print(aggregated_loads[load])
#     #     total += aggregated_loads[load]["exec_count"]
#     # print(total)
#     # try:
#     #     for key, val in aggregated_loads["4461628"]["cycles"].items():
#     #         print(key, val)
#     # except:
#     #     pass
#     # 28513503
#     # 29838643