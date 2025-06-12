import sys
import os
import glob
import numpy as np
from scipy.stats import pearsonr

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
                print(f"Warning: Could not convert value to float for line: {line.strip()}")
    return data

def compute_diff(v1, v2):
    if v1 == 0 and v2 == 0:
        return 0.0
    elif v1 == 0 or v2 == 0:
        return 100.0
    else:
        return (v2 - v1) / abs(v1) * 100

def find_top_correlated_metrics(base_dir, default_dir, top_n=10):
    all_metrics = {}
    cpi_diffs = []
    labels = []

    base_paths = glob.glob(os.path.join(base_dir, "*/results.txt"))
    for base_path in base_paths:
        # if ("leela" not in base_path and "xz" not in base_path):
            # continue

        subdir = os.path.basename(os.path.dirname(base_path))
        default_path = os.path.join(default_dir, subdir, "results.txt")

        if not os.path.exists(default_path):
            continue

        data1 = parse_file(default_path)
        data2 = parse_file(base_path)

        if "CPI" not in data1 or "CPI" not in data2:
            continue

        labels.append(subdir)
        v1_cpi, v2_cpi = data1["CPI"], data2["CPI"]
        cpi_diff = compute_diff(v1_cpi, v2_cpi)
        cpi_diffs.append(cpi_diff)

        for key in set(data1.keys()) & set(data2.keys()):
            if key == "CPI":
                continue
            try:
                v1 = data1[key]
                v2 = data2[key]
                diff = compute_diff(v1, v2)
                all_metrics.setdefault(key, []).append(diff)
            except Exception:
                continue

    # Filter metrics that have data for all entries
    full_metrics = {k: v for k, v in all_metrics.items() if len(v) == len(cpi_diffs)}

    # Compute Pearson correlation
    correlations = {}
    for key, diffs in full_metrics.items():
        if np.std(diffs) == 0:
            continue  # skip constant metrics that cause warnings
        corr, _ = pearsonr(cpi_diffs, diffs)
        correlations[key] = corr  # retain sign for interpretation

    # Sort and print top N
    top_metrics = sorted(correlations.items(), key=lambda x: x[1], reverse=True)[:top_n]
    # print(f"Top {top_n} metrics most correlated with CPI change:")
    # for metric, corr in top_metrics:
    #     print(f"{metric:30s} | Correlation: {corr:.4f}")

    # Write to file
    with open("top_correlation.txt", 'w') as f:
        f.write(f"Top {top_n} metrics most correlated with CPI change:\n")
        f.write("-----------------------------------------------------\n")
        for metric, corr in top_metrics:
            direction = "↑" if corr > 0 else "↓"
            f.write(f"{metric:30s} | Correlation: {corr:.4f} {direction}\n")


if __name__ == "__main__":
    base_dir = "/work/johan/results/main-storesets/base/a14"
    default_dir = "/work/johan/results/main-storesets/no_may_alias/a14"

    find_top_correlated_metrics(base_dir, default_dir, top_n=100000000)
