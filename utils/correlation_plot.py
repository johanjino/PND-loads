import sys
import os
import glob
import numpy as np
import matplotlib.pyplot as plt

def parse_file(filepath):
    data = {}
    with open(filepath, 'r') as f:
        for line in f:
            if not line.strip():
                continue
            parts = line.strip().rsplit(' ', 1)
            if len(parts) != 2:
                continue
            key, value = parts
            try:
                data[key] = float(value)
            except ValueError:
                # print(f"Warning: Could not convert value to float for line: {line.strip()}")
                pass
    return data

def compute_diff(v1, v2):
    if v1 == 0 and v2 == 0:
        return 0.0
    elif v1 == 0 or v2 == 0:
        return 100.0
    else:
        return (v2 - v1) / abs(v1) * 100

def collect_differences(base_dir, default_dir, metrics):
    diffs = {metric: [] for metric in metrics}
    labels = []

    base_paths = glob.glob(os.path.join(base_dir, "*/results.txt"))
    for base_path in base_paths:
        subdir = os.path.basename(os.path.dirname(base_path))
        default_path = os.path.join(default_dir, subdir, "results.txt")

        if not os.path.exists(default_path):
            print(f"Skipping {subdir}: default results.txt not found.")
            continue

        data1 = parse_file(base_path)
        data2 = parse_file(default_path)

        labels.append(subdir)
        for metric in metrics:
            if metric == "TotalViolation":
                v1 = data1.get("system.switch_cpus.commit.memOrderViolationEvents") + data1.get("system.switch_cpus.commit.PNDLoadViolations")
                v2 = data2.get("system.switch_cpus.commit.memOrderViolationEvents") + data2.get("system.switch_cpus.commit.PNDLoadViolations")
            elif metric == "system.switch_cpus.commit.PNDLoadViolations":
                v1 = data1.get("system.switch_cpus.commit.memOrderViolationEvents") + data1.get("system.switch_cpus.commit.PNDLoadViolations")
                v2 = data2.get("system.switch_cpus.commit.memOrderViolationEvents") + data2.get("system.switch_cpus.commit.PNDLoadViolations")
                v1 = (data1.get("system.switch_cpus.commit.PNDLoadViolations")*100)/v1
                v2 = (data2.get("system.switch_cpus.commit.PNDLoadViolations")*100)/v2
            else:
                v1 = data1.get(metric)
                v2 = data2.get(metric)

            if v1 is None or v2 is None:
                print(f"Warning: Missing {metric} in {subdir}")
                diffs[metric].append(None)
                continue

            diff_percent = compute_diff(v1, v2)
            diffs[metric].append(diff_percent)

    # Sort by label alphabetically
    combined = list(zip(labels, *[diffs[m] for m in metrics]))
    combined.sort(key=lambda x: x[0])
    sorted_labels = [x[0] for x in combined]
    sorted_diffs = {metric: [x[i+1] for x in combined] for i, metric in enumerate(metrics)}

    return sorted_labels, sorted_diffs

def plot_differences(labels, diffs):
    x = np.arange(len(labels))
    width = 0.35
    num_metrics = len(diffs)
    offset = width / num_metrics

    plt.figure(figsize=(12, 6))

    for i, (metric, values) in enumerate(diffs.items()):
        y = [val if val is not None else 0 for val in values]
        bar_positions = x + i * offset - offset / 2
        clipped_heights = [max(-1, min(val, 1)) for val in y]

        bars = plt.bar(bar_positions, clipped_heights, width / num_metrics, label=metric)

        # Annotate values that are outside the [-1, 1] range
        for pos, val in zip(bar_positions, y):
            if val > 1:
                plt.text(pos, 1.02, f"{val:.1f}%", ha='center', va='bottom', fontsize=8, rotation=90)
            elif val < -1:
                plt.text(pos, -1.02, f"{val:.1f}%", ha='center', va='top', fontsize=8, rotation=90)


    plt.xticks(x, labels, rotation=45, ha='right')
    plt.ylabel("Percentage Difference (%)")
    plt.title("Metric Differences Between Base and Default")
    plt.legend()
    plt.tight_layout()
    plt.grid(True)
    plt.savefig("Correlation.png")

if __name__ == "__main__":
    # base_dir = "/work/johan/results/main-storesets/base/a14"
    base_dir = "/work/johan/results/main-tage-properly-scaled/base/a14"
    default_dir = "/work/johan/results/main-tage-properly-scaled/withSVF/a14"
    # default_dir = "/work/johan/results/store_distance/store_distance_shift4/a14"
    metrics_to_plot = ["CPI",
        # "system.switch_cpus.branchPred.mispredicted_0::total",
        # "system.switch_cpus.iew.branchMispredicts",
        "system.switch_cpus.commit.branchMispredicts",
        "TotalViolation",
        # "system.switch_cpus.branchPred.mispredicted_0::CallDirect",
        # "system.switch_cpus.branchPred.mispredicted_0::CallIndirect",
        # "system.switch_cpus.branchPred.mispredicted_0::DirectCond",
        # "system.switch_cpus.branchPred.mispredicted_0::IndirectCond",
        # "system.switch_cpus.branchPred.mispredicted_0::Return",
        # "system.switch_cpus.commit.PNDLoadViolations",
        # "system.switch_cpus.fetch.pendingTrapStallCycles",
        # "system.switch_cpus.branchPred.btb.mispredict::total",
        "system.switch_cpus.branchPred.ras.incorrect",
        # "system.l2.ReadExReq.missRate::total",
        # "system.switch_cpus.MemDepUnit__0.insertedStores",
        # "system.switch_cpus.commit.memOrderViolationEvents",
        # "system.switch_cpus.fetch.icacheSquashes",
        # "system.switch_cpus.MemDepUnit__0.PHASTCorrectPredictions"
        # "system.switch_cpus.issueRate",
        # "system.switch_cpus.fetchStats0.fetchRate",
        # "system.switch_cpus.executeStats0.instRate"
    ]

    labels, diffs = collect_differences(base_dir, default_dir, metrics_to_plot)
    plot_differences(labels, diffs)

