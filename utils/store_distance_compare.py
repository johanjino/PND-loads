import os
import re
import sys
from collections import defaultdict

# ==============================================================================
# CORE ANALYSIS FUNCTIONS (Refactored from your code)
# ==============================================================================

THRESHOLD = 2

def filter_loads_by_threshold(loads, threshold):
    """
    Filters loads where at least 95% of its cycles are above the threshold.
    """
    result = []
    for load_id, data in loads.items():
        # Ensure exec_total is not zero to avoid division errors
        exec_total = data["exec_count"]
        if exec_total == 0:
            continue

        cycles = data["cycles"]
        
        # Calculate the total count of cycles greater than the threshold
        count_above_threshold = sum(count for cycle, count in cycles.items() if cycle > threshold)

        if (count_above_threshold / exec_total) >= 0.95:
            result.append(load_id)

    return result

def parse_stats_file(lines, weight=1.0):
    """
    Parses the lines of a stats.txt file and returns a dictionary of loads.
    """
    loads = {}
    ignores = re.compile(r'^---|^$')
    for line in lines:
        if len(line.strip()) == 0 or ignores.match(line) or re.search(r'[a-zA-Z]', line):
            continue

        parts = line.strip().split()
        load_id, exec_count_str = parts[0].split("-")
        exec_count = float(exec_count_str) * weight
        cycle_data = parts[1:]
        
        cycle_map = {}
        total_exec_from_cycles = 0.0
        for item in cycle_data:
            if ':' in item:
                cycle, c = item.split(':')
                cycle = int(cycle)
                weighted_c = float(c) * weight
                cycle_map[cycle] = weighted_c
                total_exec_from_cycles += weighted_c

        loads[load_id] = {
            "exec_count": exec_count,
            "cycles": cycle_map
        }
    return loads

def aggregate_run_data(aggregated_loads, new_loads):
    """
    Aggregates new load data into the main aggregated dictionary.
    """
    for load_id, data in new_loads.items():
        if load_id not in aggregated_loads:
            aggregated_loads[load_id] = data
        else:
            aggregated_loads[load_id]["exec_count"] += data["exec_count"]
            for cycle, count in data["cycles"].items():
                aggregated_loads[load_id]["cycles"][cycle] = aggregated_loads[load_id]["cycles"].get(cycle, 0) + count

def analyze_benchmark(base_dir, bench, runs, is_checkpoint_method):
    """
    Main analysis function to process a benchmark for a given method.
    
    Returns:
        - set: A set of "good" load IDs found.
        - set: A set of *all* load IDs encountered.
    """
    aggregated_loads = defaultdict(lambda: {"exec_count": 0, "cycles": defaultdict(float)})
    all_loads_encountered = set()

    for run in runs:
        bench_name = bench.split(".")[1].rstrip("_s")
        
        # Construct path based on whether it's the checkpoint method or not
        if is_checkpoint_method:
            run_dir = os.path.join(base_dir, f"{bench_name}.{run}", "raw")
        else:
            run_dir = os.path.join(base_dir, f"{bench_name}.{run}")

        if not os.path.isdir(run_dir):
            continue

        if is_checkpoint_method:
            # Logic for the ground truth (checkpoint aggregation)
            cpt_base_dir = f"/mnt/data/checkpoints/{bench}/checkpoints.{run}" # Placeholder path
            if not os.path.isdir(cpt_base_dir): continue

            for dirname in os.listdir(run_dir):
                if os.path.isdir(os.path.join(run_dir, dirname)) and dirname.endswith('.out'):
                    stats_file = os.path.join(run_dir, dirname, "stats.txt")
                    chkpt_number = int(dirname.split('.')[0])
                    weight = 0.0
                    
                    try:
                        # Find corresponding checkpoint weight
                        for cpt_file in os.listdir(cpt_base_dir):
                            if "cpt" in cpt_file:
                                cpt_number = int(cpt_file.split("_")[1])
                                if cpt_number == chkpt_number-1:
                                    weight = float(cpt_file.split('_')[5])
                                    break
                        
                        if weight==0:
                            raise IndexError

                        with open(stats_file, 'r') as f:
                            lines = f.readlines()
                        
                        if not lines: continue
                        
                        loads = parse_stats_file(lines, weight)
                        all_loads_encountered.update(loads.keys())
                        aggregate_run_data(aggregated_loads, loads)
                        
                    except (FileNotFoundError, IndexError):
                        continue
        else:
            # Logic for the non-aggregated method
            stats_file = os.path.join(run_dir, "stats.txt")
            try:
                with open(stats_file, 'r') as f:
                    lines = f.readlines()
                
                loads = parse_stats_file(lines) # Weight is 1.0 by default
                all_loads_encountered.update(loads.keys())
                aggregate_run_data(aggregated_loads, loads)
            except FileNotFoundError:
                continue
    good_loads = filter_loads_by_threshold(aggregated_loads, THRESHOLD)
    return set(good_loads), all_loads_encountered

# ==============================================================================
# MAIN COMPARISON SCRIPT
# ==============================================================================

def main():
    """
    Main function to run the comparison.
    """
    # --- CONFIGURATION ---
    # !!! IMPORTANT: Please verify these paths are correct for your system !!!
    GROUND_TRUTH_BASE_DIR = "/work/johan/results/store_distance/empty/a14"
    NON_AGGREGATED_BASE_DIR = "/work/johan/results/train/store_distance/a14"

    BENCHES = [
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
    ]

    print(f"Starting comparison with Threshold = {THRESHOLD}\n")

    # 1. Initialize data structures to hold the results for each category
    results = defaultdict(list)

    # --- Main Processing Loop ---
    for bench in BENCHES:
        print(f"--- Processing Benchmark: {bench} ---")

        # Get Ground Truth results
        try:
            gt_good_loads, gt_all_loads = analyze_benchmark(
                GROUND_TRUTH_BASE_DIR, bench, ["0", "1", "2"], is_checkpoint_method=True
            )
            print(f"  Ground Truth: Found {len(gt_good_loads)} good loads out of {len(gt_all_loads)} total.")
        except Exception as e:
            print(f"  ERROR processing Ground Truth for {bench}: {e}")
            continue

        # Get Non-Aggregated results
        try:
            test_good_loads, test_all_loads = analyze_benchmark(
                NON_AGGREGATED_BASE_DIR, bench, ["0", "1", "2", "3", "4"], is_checkpoint_method=False
            )
            print(f"  Non-Aggregated: Found {len(test_good_loads)} good loads out of {len(test_all_loads)} total.")
        except Exception as e:
            print(f"  ERROR processing Non-Aggregated for {bench}: {e}")
            continue
            
        # Perform the comparison using set logic
        total = len(gt_all_loads)
        if total == 0:
            print("  No loads found in ground truth dataset. Skipping.\n")
            continue

        gt_bad_loads = gt_all_loads.difference(gt_good_loads)
        test_bad_loads = test_all_loads.difference(test_good_loads)

        tp_set = gt_good_loads.intersection(test_good_loads)
        fp_set = gt_bad_loads.intersection(test_good_loads)
        fn_set = gt_good_loads.intersection(test_bad_loads)
        tn_set = gt_bad_loads.intersection(test_bad_loads)
        unknown_set = gt_all_loads.difference(test_all_loads)

        # 4. Calculate ratios and store them in the desired format
        # The value is the percentage (ratio * 100)
        results["TP"].append(f"({bench.split('_')[0].split('.')[1]},{(len(tp_set) / total) * 100:.2f})")
        results["FP"].append(f"({bench.split('_')[0].split('.')[1]},{(len(fp_set) / total) * 100:.2f})")
        results["FN"].append(f"({bench.split('_')[0].split('.')[1]},{(len(fn_set) / total) * 100:.2f})")
        results["TN"].append(f"({bench.split('_')[0].split('.')[1]},{(len(tn_set) / total) * 100:.2f})")
        results["Unknown"].append(f"({bench.split('_')[0].split('.')[1]},{(len(unknown_set) / total) * 100:.2f})")
        print("  Done.\n")


    # 5. Print the final report, grouped by category
    print("\n=============================================")
    print("          AGGREGATED RESULTS")
    print("=============================================\n")

    category_map = {
        "TP": "True Positives (TP)",
        "FP": "False Positives (FP)",
        "FN": "False Negatives (FN)",
        "TN": "True Negatives (TN)",
        "Unknown": "Unknown (in GT only)"
    }

    for category_code, data_list in results.items():
        # print(f"--- {category_map[category_code]} ---")
        print("\\addplot+[ybar, bar shift=-10pt] plot coordinates{")
        # print("\\addplot+[ybar] plot coordinates{")
        # print("\\addplot+[ybar, bar shift=10pt] plot coordinates{")
        for item in data_list:
            print("\t", item)
        print("};\n") # Add a newline for spacing between categories


if __name__ == "__main__":
    main()