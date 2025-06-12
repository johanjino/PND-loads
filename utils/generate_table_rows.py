import os
import sys
import math

# Define benchmarks
BENCHES = [
    "perlbench.0", "perlbench.1", "perlbench.2",
    "gcc.0", "gcc.1", "gcc.2",
    "mcf.0", "lbm.0", "omnetpp.0", "xalancbmk.0",
    "x264.0", "x264.1", "x264.2",
    "deepsjeng.0", "leela.0", "nab.0",
    "xz.0", "xz.1"
]

# Stats to track - ADDED numInsts
stats = {
    "CPI",
    "system.switch_cpus.commitStats0.ipc",
    "system.switch_cpus.MemDepUnit__0.MDPLookups",
    "system.switch_cpus.executeStats0.numInsts",  # ADDED for calculation
    "system.switch_cpus.MemDepUnit__0.PHASTMispredictions",
    "system.switch_cpus.commit.memOrderViolationEvents",
    "system.switch_cpus.commit.PNDLoadViolations",
    "system.switch_cpus.branchPred.mispredicted_0::total"
    "system.switch_cpus.iew.iqFullEvents",
}

# A list of special derived metrics that require multiple stats
derived_stats = {"MDPKilo"}


def get_values(results_file):
    """Parses a results.txt file and returns a dictionary of stats."""
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
    """Reads all results.txt files in a directory and compiles the stats."""
    results = {}
    if not os.path.isdir(results_dir):
        print(f"Warning: Directory not found: {results_dir}", file=sys.stderr)
        return results
        
    for f in os.listdir(results_dir):
        full_path = os.path.join(results_dir, f)
        result_file = os.path.join(full_path, "stats.txt") # Corrected to stats.txt as is common
        if not os.path.exists(result_file):
             result_file = os.path.join(full_path, "results.txt") # Fallback to results.txt

        if os.path.isdir(full_path) and os.path.exists(result_file):
            results[f] = get_values(result_file)
    return results

def extract_stat(results, stat):
    """Extracts a single statistic from the compiled results."""
    stat_results = {}
    for bench, vals in results.items():
        if stat in vals:
            stat_results[bench] = vals[stat]
    return stat_results

def calculate_mdp_kilo(results):
    """Calculates MDPLookups per 1000 instructions."""
    mdp_kilo_results = {}
    for bench, vals in results.items():
        mdp_lookups = vals.get("system.switch_cpus.iew.iqFullEvents")
        num_insts = vals.get("system.switch_cpus.executeStats0.numInsts")

        if mdp_lookups is not None and num_insts is not None and num_insts > 0:
            # Calculate the metric and store it
            mdp_kilo_results[bench] = (mdp_lookups / num_insts) * 100000
    return mdp_kilo_results


def compute_diffs(base, other):
    """Computes the values to be displayed in the table."""
    diffs = []
    for bench in BENCHES:
        if bench not in base or bench not in other or base[bench] == 0:
            diffs.append(None)
        else:
            # The original script just used the value from the 'other' run
            diffs.append(other[bench])
    return diffs

def print_latex_table_rows(base_results, method_dirs, stat_or_metric):
    """Generates and prints the LaTeX table rows for a given metric."""
    # Read all data first
    all_method_data = [read_results_from_dir(path) for path in method_dirs]

    # Calculate the results for the desired metric
    if stat_or_metric == "MDPKilo":
        method_results = [calculate_mdp_kilo(data) for data in all_method_data]
    else:
        method_results = [extract_stat(data, stat_or_metric) for data in all_method_data]
    
    computed_diffs = [compute_diffs(base_results, res) for res in method_results]

    for i, bench in enumerate(BENCHES):
        row = "\\cellcolor[gray]{.9}"+ bench
        for diffs in computed_diffs:
            val = diffs[i]
            if val is not None:
                row += f" & \\multicolumn{{1}}{{c|}}{{{val:.3f}}}"
            else:
                row += " & \\multicolumn{1}{c|}{--}"
        print(row + r" \\")

    # Geomean (Only sensible for ratios, kept from original script)
    geos = []
    for res in method_results:
        ratios = []
        for bench in BENCHES:
            b = base_results.get(bench)
            o = res.get(bench)
            if b and o:
                ratios.append(o / b)
        if ratios:
            gm = (math.prod(ratios))**(1 / len(ratios))
            percent = (gm - 1) * 100
            geos.append(f"{percent:.3f}\\%")
        else:
            geos.append("--")
    print("Geomean & " + " & ".join(geos) + r" \\")

# ---- Entry ----
if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python3 generate_table_rows.py <stat|MDPKilo> <model> <method1> [<method2>...]")
        sys.exit(1)

    stat_or_metric = sys.argv[1]
    if stat_or_metric not in stats and stat_or_metric not in derived_stats:
        print(f"Error: stat or metric '{stat_or_metric}' not recognized.")
        sys.exit(1)

    model = sys.argv[2]
    root_dir = "/work/johan/results/"
    base_results_dir = os.path.join(root_dir, "final", "base", model)
    
    # Read all base data once
    base_data = read_results_from_dir(base_results_dir)
    
    # Calculate the base results for the desired metric
    if stat_or_metric == "MDPKilo":
        base_results = calculate_mdp_kilo(base_data)
    else:
        base_results = extract_stat(base_data, stat_or_metric)

    method_dirs = [os.path.join(root_dir, d, model) for d in sys.argv[3:]]
    print_latex_table_rows(base_results, method_dirs, stat_or_metric)