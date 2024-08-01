import os
import collections
import sys
import re

bench = sys.argv[1]
run = sys.argv[2]

aggregated_values = collections.defaultdict(float)

mode = "-pnd"
if "base" in os.getcwd(): mode = "-base"
bench_name = bench.split("_")[0].split(".")[1]
print("Benchmark: "+bench_name+"."+run+mode)

#pass over each .out and aggregate weighted values
for dirname in os.listdir("."):
    if os.path.isdir(dirname) and dirname[0].isdigit() and dirname.split('.')[1] == 'out':
        stats_file = os.path.join(dirname, "stats.txt")
        chkpt_number = int(dirname.split('.')[0])
        cpt_dir = "/work/muke/checkpoints/"+bench+"/checkpoints."+run
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
        count = 0
        for line in lines:
            # ignore empty lines and lines starting with "---"
            #regex would catch empty lines but we want to count number of begin/end markers for correctness checking
            if len(line.strip()) == 0: continue
            if not ignores.match(line):
                statKind = statLine.match(line).group(1)
                statValue = statLine.match(line).group(2)
                if statValue == 'nan':
                    print("%s is nan. Setting it to 0" % statKind)
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
    if '.' in field_name:
        name = '.'.join(field_name.split(".")[2:])
        if 'cpu' in field_name and 'switch_cpu' not in field_name:
            cpu_fields[name] = value
            continue
    else: name = field_name
    if name not in seen_fields:
        if 'switch_cpu' in field_name and name in cpu_fields:
            del cpu_fields[name]
        results_file.write(f"{name} {value}\n")
        seen_fields.add(name)
    elif value != 0:
        pass
        #print("Field "+(field_name)+" has duplicate but doesn't have a value of 0")
for name, value in cpu_fields.items():
    results_file.write(f"{name} {value}\n")
results_file.close()
