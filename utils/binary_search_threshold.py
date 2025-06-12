import os
import math
import subprocess
import sys

def compute_ipc_and_mdp_change(base_results_dir, target_results_dir):
    stats = {
        "CPI",
        "system.switch_cpus.MemDepUnit__0.MDPLookups"
    }

    derived_metrics = {
        "CPI": lambda x: float(1) / float(x),  # IPC
    }

    def get_values(results_file):
        values = {}
        with open(results_file, "r") as f:
            for line in f:
                parts = line.split()
                if len(parts) < 2:
                    continue
                name, value = parts[0], parts[1]
                if name in stats:
                    try:
                        values[name] = float(value)
                    except ValueError:
                        continue
        return values

    def read_results_from_dir(results_dir):

        expected_benchmarks = [
            "perlbench.0", "perlbench.1", "perlbench.2",
            "gcc.0", "gcc.1", "gcc.2",
            # "mcf.0", 
            # "lbm.0", 
            # "omnetpp.0", 
            "xalancbmk.0",
            "x264.0", "x264.1", "x264.2",
            # "deepsjeng.0", 
            "leela.0", 
            "nab.0",
            "xz.0", "xz.1"
        ]

        results = {}
        missing = []

        for bench in expected_benchmarks:
            bench_dir = os.path.join(results_dir, bench)
            result_file = os.path.join(bench_dir, "results.txt")
            if os.path.isdir(bench_dir) and os.path.exists(result_file):
                results[bench] = get_values(result_file)
            else:
                missing.append(bench)

        if missing:
            raise FileNotFoundError(f"Missing results.txt for benchmark(s): {', '.join(missing)}")

        return results

    def extract_stat(results, stat):
        stat_results = {}
        for bench, vals in results.items():
            if stat in vals:
                raw_val = vals[stat]
                stat_results[bench] = derived_metrics.get(stat, lambda x: x)(raw_val)
        return stat_results

    def percent_change(base_values, target_values, stat, reverse=False):
        values = []
        for bench in base_values:
            if bench in target_values and base_values[bench] != 0:
                base = base_values[bench]
                target = target_values[bench]
                change = ((target - base) / base) * 100
                if reverse:
                    change *= -1
                values.append((target / base))
        if values:
            geo_mean = math.prod(values) ** (1.0 / len(values))
            return (geo_mean - 1) * 100
        else:
            return 0.0

    # Load and compute
    base_data = read_results_from_dir(base_results_dir)
    target_data = read_results_from_dir(target_results_dir)

    ipc_base = extract_stat(base_data, "CPI")
    ipc_target = extract_stat(target_data, "CPI")
    ipc_increase = percent_change(ipc_base, ipc_target, "CPI")  # Higher is better

    mdp_base = extract_stat(base_data, "system.switch_cpus.MemDepUnit__0.MDPLookups")
    mdp_target = extract_stat(target_data, "system.switch_cpus.MemDepUnit__0.MDPLookups")
    mdp_reduction = percent_change(mdp_base, mdp_target, "MDPLookups", reverse=True)  # Lower is better

    return {
        "ipc_increase_percent": ipc_increase,
        "mdp_reduction_percent": -mdp_reduction
    }






def cost(ipc_diff, mdp_red):
    ipc_diff = float(ipc_diff)
    mdp_red = float(mdp_red)
    return (ipc_diff) + (mdp_red/2000)



def evaluate(threshold):

    threshold = str(threshold)

    base_dir = "/work/johan/results/binary_search/base/m4"
    target_dir = "/work/johan/results/binary_search/sd_train_"+ threshold +"_full/m4"

    try:
        results = compute_ipc_and_mdp_change(base_dir, target_dir)

    except FileNotFoundError:
        # Generate labels
        command = "python3 /work/johan/PND-Loads/utils/store_distance_train_aggregate.py " + threshold
        exe = subprocess.run(command, shell=True, capture_output=True, text=True)
        if exe.returncode != 0:
            print(f"Error")
            print("Stdout:\n", exe.stdout)
            print("Stderr:\n", exe.stderr)
            sys.exit(exe.returncode)


        # Run Spec
        command = "python3 /work/johan/PND-Loads/utils/run_models.py --run-type=binary_search --addr-types=sd_train_"+threshold+"_full --cpu-models=m4"
        exe = subprocess.run(command, shell=True, capture_output=True, text=True)
        if exe.returncode != 0:
            print(f"Error")
            print("Stdout:\n", exe.stdout)
            print("Stderr:\n", exe.stderr)
            sys.exit(exe.returncode)

        results = compute_ipc_and_mdp_change(base_dir, target_dir)

    return cost(results["ipc_increase_percent"], results["mdp_reduction_percent"])


def get(threshold):
    if threshold not in results:
        results[threshold] = evaluate(threshold)
    
    return results[threshold]


def search(low, high):

    low = int(low)
    high = int(high)

    best_so_far = -100

    while (high-low)>1:
        mid = (low + high) // 2
        step = (high - low) // 5 # dynamic steps
        step = max(1, step) # prevent going sub 1
        
        print("At mid = ", mid)

        if get(mid) > get(mid - step):
            low = mid  # peak is to the right
            best = get(mid)
        else:
            high = mid - step    # peak is at mid or to the left
            best = get(mid-step)

        best_so_far = max(best, best_so_far)
        print("Best so far = ", best_so_far)

    return low, high


results = dict({})

threshold1, threshold2 = search(0, 100)

print("Best Threshold = ",threshold1, " ", threshold2)
print(results)


    


        