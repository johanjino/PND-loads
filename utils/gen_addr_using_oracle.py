import sys
import re
import os

benches = [
    "600.perlbench_s", 
    "605.mcf_s",
    "619.lbm_s",
    "625.x264_s",
    "631.deepsjeng_s",
    "641.leela_s",
    "657.xz_s", 
    # "620.omnetpp_s",
    # "602.gcc_s",
    "623.xalancbmk_s",
    "644.nab_s"
    ] #"638.imagick_s"]

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

def gen(file1, allAddrFile, threshold, output_file):
    data = parse_file(file1)

    with open(output_file, 'w') as out:
        with open(allAddrFile, 'r') as f:
            for line in f:
                addr = int(line)
                if addr not in data:
                    out.write(f"{addr}\n")
                    # continue
                elif (float(data[addr][0])/(float(data[addr][1])+1e-12))*100 < threshold:
                    out.write(f"{addr}\n")
                # elif (float(data[addr][0])) < threshold and (float(data[addr][0])/(float(data[addr][1])+1e-12))*100 < 10:
                #     out.write(f"{addr}\n")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python compare_with_threshold.py <name> <threshold> ")
        sys.exit(1)

    threshold =  float(sys.argv[2])

    addr_file_type = sys.argv[1]
    os.makedirs("/work/johan/PND-Loads/addr_files/"+addr_file_type, exist_ok=True)

    for bench in benches:
        print(bench)
        oracle_file = "/work/johan/results/main-tage-properly-scaled/oracle/m4/" + bench.split(".")[1].rstrip("_s")+".0" + "/results.txt"
        allAddrFile = "/work/johan/PND-Loads/addr_files/oracle/"+bench
        output_file = "/work/johan/PND-Loads/addr_files/"+addr_file_type+"/"+bench

        gen(oracle_file, allAddrFile, threshold, output_file)



