import sys
import re

def parse_file(filepath):
    pnd_addr_counts = re.compile(r"^\s*(\d+)\s+([0-9]*\.?[0-9]+)\s+([0-9]*\.?[0-9]+)")
    data = {}
    with open(filepath, 'r') as f:
        for line in f:

            try:
                addr, violations, execs = pnd_addr_counts.match(line).groups()
                if int(addr) in data:
                    print("DUPLICATE!!")
                else:
                    data[int(addr)] = [float(violations), float(execs)]
                continue
            except AttributeError:
                pass
    return data

def compare_files(file1, file2, threshold=None, output_file="addr_oracle_output.txt"):
    data = parse_file(file2)

    with open(output_file, 'w') as out:
        with open(file1, 'r') as f:
            for line in f:
                addr = int(line)
                if addr not in data:
                    if threshold == None:
                        out.write(f"{addr}: {0} {0} {0}\n")    
                else:
                    percentage = float(data[addr][0])/float(data[addr][1])*100
                    if threshold == None:
                        out.write(f"{addr}: {data[addr][0]} {data[addr][1]} {percentage}\n")
                    elif percentage>=threshold:
                        out.write(f"{addr}: {data[addr][0]} {data[addr][1]} {percentage}\n")
    
        # out.write(str(data))

    print(f"\nDifferences written to: {output_file}")

# def compare_files(file1, file2, threshold=None, output_file="addr_oracle_output.txt"):
#     data = parse_file(file2)
#     seen_in_file1 = set()  # To track addresses we've seen in file1

#     with open(output_file, 'w') as out:
#         # First pass: process file1 and check for missing addresses
#         with open(file1, 'r') as f:
#             for line in f:
#                 addr = int(line.strip())
#                 seen_in_file1.add(addr)  # Mark this address as seen
                
#                 if addr not in data:
#                     if threshold is None:
#                         out.write(f"{addr}: {0} {0}\n")    
#                 else:
#                     if threshold is None:
#                         out.write(f"{addr}: {data[addr][0]} {data[addr][1]}\n")
#                     elif float(data[addr][0])/float(data[addr][1])*100 >= threshold:
#                         out.write(f"{addr}: {data[addr][0]} {data[addr][1]}\n")
        
#         # Second pass: check for keys in data that weren't in file1
#         extra_keys_found = False
#         for addr in data:
#             if addr not in seen_in_file1:
#                 if not extra_keys_found:  # Write header only if we find extras
#                     print("\n\n--- Extra keys in file2 not present in file1 ---\n")
#                     extra_keys_found = True
#                 print(f"{addr}: {0} {0}\n")  # Write with zeros since missing in file1

#     print(f"\nDifferences written to: {output_file}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python compare_with_threshold.py <benchmark_number_name> <benchmark_name> ")
        sys.exit(1)

    file2 = "/work/johan/results/main-tage-properly-scaled/oracle/m4/" + sys.argv[2] + "/results.txt"
    file1 = "/work/johan/PND-Loads/addr_files/default/" + sys.argv[1]
    try:
        threshold = float(sys.argv[3])
    except:
        threshold = None
    compare_files(file1, file2, threshold)
