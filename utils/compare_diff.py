import sys

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

def compare_files(file1, file2, threshold_percent, output_file="diff_output.txt"):
    data1 = parse_file(file1)
    data2 = parse_file(file2)

    with open(output_file, 'w') as out:
        for key in data1:
            if key not in data2:
                msg = f"Missing key in second file: {key}\n"
                print(msg.strip())
                out.write(msg)
                continue

            v1 = data1[key]
            v2 = data2[key]

            if v1 == 0 and v2 == 0:
                continue
            elif v1 == 0 or v2 == 0:
                diff_percent = 100.0
            else:
                diff_percent = (v2 - v1) / abs(v1) * 100

            if abs(diff_percent) >= threshold_percent:
                msg = f"{key} differs by {diff_percent:.2f}%: {v1} vs {v2}\n"
                print(msg.strip())
                out.write(msg)

    print(f"\nDifferences written to: {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python compare_with_threshold.py <benchmark> <threshold_percent>")
        sys.exit(1)

    file2 = "/work/johan/results/final/sd_train_2_full/s5/" + sys.argv[1] + "/results.txt"
    file1 = "/work/johan/results/final/base/s5/" + sys.argv[1] + "/results.txt"
    # file2 = "/work/johan/results/main-tage-properly-scaled/no_may_alias/m4/" + sys.argv[1] + "/results.txt"
    threshold = float(sys.argv[2])

    compare_files(file1, file2, threshold)
