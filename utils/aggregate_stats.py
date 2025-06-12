import os
import collections
import sys
import re

bench = sys.argv[1]
run = sys.argv[2]

aggregated_values = collections.defaultdict(float)
pnd_addr_aggregated = {}

mode = "-pnd"
if "base" in os.getcwd(): mode = "-base"
bench_name = bench.split("_")[0].split(".")[1]
print("Benchmark: "+bench_name+"."+run+mode)

#pass over each .out and aggregate weighted values
for dirname in os.listdir("."):
    if os.path.isdir(dirname) and dirname[0].isdigit() and dirname.split('.')[1] == 'out':
        stats_file = os.path.join(dirname, "stats.txt")
        chkpt_number = int(dirname.split('.')[0])
        cpt_dir = "/mnt/data/checkpoints//"+bench+"/checkpoints."+run
        for cpt in os.listdir(cpt_dir):
            if cpt.startswith("cpt.") and int(cpt.split('_')[1]) == (chkpt_number-1):
                weight = float(cpt.split('_')[5])
        f = open(stats_file)
        lines = f.readlines()
        if len(lines) == 0:
            print("Checkpoint "+str(chkpt_number)+" has empty stats file!")
            exit(1)
        stats = {}
        ignores = re.compile(r'^---|^$')
        statLine = re.compile(r'([a-zA-Z0-9_\.:-]+)\s+([-+]?[0-9]+\.[0-9]+|[-+]?[0-9]+|nan|inf)')
        pnd_addr_counts = re.compile(r"^\s*(\d+)\s+(\d+)\s+(\d+)")
        count = 0
        for line in lines:
            #ignore empty lines and lines starting with "---"
            #regex would catch empty lines but we want to count number of begin/end markers for correctness checking
            if len(line.strip()) == 0: continue
            if not ignores.match(line):
                try:
                    addr, violations, execs = pnd_addr_counts.match(line).groups()
                    if addr in pnd_addr_aggregated:
                        pnd_addr_aggregated[addr][0] += (float(violations)*weight)
                        pnd_addr_aggregated[addr][1] += (float(execs)*weight)
                    else:
                        pnd_addr_aggregated[addr] = [(float(violations)*weight), (float(execs)*weight)]
                    continue
                except AttributeError:
                    pass
                try:
                    statKind = statLine.match(line).group(1)
                    statValue = statLine.match(line).group(2)
                except AttributeError:
                    continue
                #ignore atomic cpu stats
                if "system.cpu." in statKind: continue
                if statValue == 'nan':
                    statValue = '0'
                stats[statKind] = float(statValue) * weight
            else:
                count += 1
        if count != 4:
            print("Checkpoint "+str(chkpt_number)+" only has warmup!")
            exit(1)
        f.close()

        #collecting this passing over to clobber warmup values
        for stat in stats:
            aggregated_values[stat] += stats[stat]

#handle CPI separately so we dont have to worry about averaging ratios
aggregated_values["CPI"] = aggregated_values.get("system.switch_cpus.numCycles") / aggregated_values.get("system.switch_cpus.commitStats0.numInstsNotNOP")

# Write the results to the output file
results_file = open("results.txt", "w")
seen_fields = set()
cpu_fields = {}
for field_name, value in aggregated_values.items():
    results_file.write(f"{field_name} {value}\n")
results_file.write("\nPer-Address PND Load Violations:\n")
for key in sorted(pnd_addr_aggregated):
    violations, execs = pnd_addr_aggregated[key][0], pnd_addr_aggregated[key][1]
    results_file.write(f"{key} {violations} {execs}\n")
results_file.close()
