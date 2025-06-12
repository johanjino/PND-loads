import os
import sys
import math

# Define the stat keys to track (can include more than just CPI)
# stats = {"CPI", "system.switch_cpus.MemDepUnit__0.MDPLookups", "system.switch_cpus.MemDepUnit__0.PHASTMispredictions", "system.switch_cpus.commit.memOrderViolationEvents", "system.switch_cpus.commit.PNDLoadViolations" , "system.switch_cpus.branchPred.mispredicted_0::total", "system.switch_cpus.iew.iqFullEvents", "system.switch_cpus.fetchStats0.fetchRate"}  # example

# Define derived transformations if needed (e.g., CPI → IPC)
derived_metrics = {
    "CPI": lambda x: float(1)/float(x),
}

def get_values(results_file):
    values = {}
    with open(results_file, "r") as f:
        for line in f:
            parts = line.split()
            if len(parts) < 2:
                continue
            name, value = parts[0], parts[1]
            try:
                values[name] = float(value)
            except ValueError:
                continue
    return values

def read_results_from_dir(results_dir):
    results = {}
    for f in os.listdir(results_dir):
        full_path = os.path.join(results_dir, f)
        result_file = os.path.join(full_path, "results.txt")
        if os.path.isdir(full_path) and os.path.exists(result_file):
            results[f] = get_values(result_file)
    return results

def extract_stat(results, stat):
    stat_results = {}
    for bench, vals in results.items():
        if stat in vals:
            raw_val = vals[stat]
            stat_results[bench] = derived_metrics.get(stat, lambda x: x)(raw_val)
            # stat_0 = float(vals["system.switch_cpus.executeStats0.numInsts"])
            # stat_1 = float(vals["system.switch_cpus.MemDepUnit__0.PHASTCorrectPredictions"])
            # stat_2 = float(vals["system.switch_cpus.MemDepUnit__0.MDPLookups"])
            # stat_3 = float(vals["system.switch_cpus.MemDepUnit__0.bypassedMDPLookups"])
            # stat_4 = 0.0
            # if "system.switch_cpus.commit.PNDLoadViolations" in vals:
            #     stat_4 = float(vals["system.switch_cpus.commit.PNDLoadViolations"])
            # stat_5 = float(vals["system.switch_cpus.commit.memOrderViolationEvents"])
            # stat_results[bench] = (stat_1) / (stat_2)
            # stat_results[bench] = (stat_5 + stat_4)
    return stat_results

def print_normalized_results(result_values, base_values, label, stat, mode="percent"):
    print(f"\n--- {mode.title()} {stat} vs Base for: {label} ---")
    
    values = []
    benches = [
        "perlbench.0", "perlbench.1", "perlbench.2",
        "gcc.0", "gcc.1", "gcc.2",
        "mcf.0", 
        "lbm.0", 
        "omnetpp.0", 
        "xalancbmk.0",
        "x264.0", "x264.1", "x264.2",
        "deepsjeng.0", 
        "leela.0", 
        "nab.0",
        "xz.0", "xz.1"
    ]

    for bench in benches:
        if bench not in result_values or bench not in base_values or base_values[bench] == 0:
            print(f"({bench}, 0)")
            continue

        base = base_values[bench]
        target = result_values[bench]

        if mode == "normalize":
            value = target / base
            print(f"({bench}, {value:.6f})")
            values.append(value)
        elif mode == "percent":
            if stat == "system.switch_cpus.MemDepUnit__0.MDPLookups":
                value = - ((target - base) / base) * 100
            else:
                value = ((target - base) / base) * 100
            print(f"({bench}, {value:.6f})")
            values.append((target / base))  # for geometric mean

    if values:
        avg = sum(values) / len(values) 
        geo_mean = ((math.prod(values) ** (1.0 / len(values))))
        if mode == "normalize":
            print(f"(Geomean, {geo_mean:.6f})")
        elif mode == "percent":
            percent_geo_mean = (geo_mean - 1) * 100
            percent_averge = (avg - 1) * 100
            # print(f"(Geomean, {percent_geo_mean:.6f})")
            print(f"(Average, {percent_averge:.6f})")
    else:
        print("No benchmarks found to compute geometric mean.")

# ---- Main Logic ----

if len(sys.argv) < 4:
    print("Usage: python3 script.py <stat> <model> <dir1> <dir2> ... [--mode normalize|percent]")
    sys.exit(1)

# Parse optional --mode argument
if "--mode" in sys.argv:
    mode_index = sys.argv.index("--mode")
    mode = sys.argv[mode_index + 1]
    sys.argv = sys.argv[:mode_index]  # Remove mode args from argv
else:
    mode = "percent"

# Extract stat and arguments
stat = sys.argv[1]

mapped_stats = {
    "CPI":"CPI",
    "Lookups":"system.switch_cpus.MemDepUnit__0.MDPLookups",
    "falseDep":"system.switch_cpus.MemDepUnit__0.PHASTMispredictions",
    "MemOrV":"system.switch_cpus.commit.memOrderViolationEvents",
    "PNDV":"system.switch_cpus.commit.PNDLoadViolations" ,
    "branchPred":"system.switch_cpus.commit.branchMispredicts",
    "IQ": "system.switch_cpus.iew.iqFullEvents",
    "fetchrate": "system.switch_cpus.issueRate",
    "rob": "system.switch_cpus.squashedInstsIssued"
}
stat = mapped_stats[stat]

# if stat not in stats:
#     print(f"Error: '{stat}' not in tracked stats. Add it to the 'stats' set.")
#     sys.exit(1)

model = sys.argv[2]
partial_other_dirs = sys.argv[3:]

root_dir = "/work/johan/results/"
# base_results_dir = os.path.join(root_dir, "main-tage-properly-scaled", "base", model)
base_results_dir = os.path.join(root_dir, "final", "base", model)

# Construct full paths to result dirs
other_dirs = [os.path.join(root_dir, d, model) for d in partial_other_dirs]

# Read base and extract stat
base_results = read_results_from_dir(base_results_dir)
base_stat_values = extract_stat(base_results, stat)

# Compare each dir to base
for result_dir in other_dirs:
    label = os.path.basename(os.path.dirname(result_dir))
    result_data = read_results_from_dir(result_dir)
    result_stat_values = extract_stat(result_data, stat)
    print_normalized_results(result_stat_values, base_stat_values, label, stat, mode=mode)
