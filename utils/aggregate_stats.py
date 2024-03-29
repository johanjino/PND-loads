import os
import collections

def aggregate_values(field_names):
    field_names_to_average = {"system.switch_cpus.lsq0.loadToUse::mean": [],
                              "system.switch_cpus.lsq0.loadToUse::stdev": []}
    aggregated_values = collections.defaultdict(float)

    broken_chkpts = []

    print("Benchmark: ", os.getcwd().split("/")[-1])
    for dirname in os.listdir("."):
        if os.path.isdir(dirname) and dirname[0].isdigit() and dirname.split('.')[1] == 'out':
            stats_file = os.path.join(dirname, "stats.txt")
            chkpt_number = int(dirname.split('.')[0])
            #if chkpt_number in broken_chkpts: continue
            cpt_dir = "../checkpoints."+os.getcwd().split('.')[-1]
            for cpt in os.listdir(cpt_dir):
                if cpt.startswith("cpt.") and int(cpt.split('_')[1]) == (chkpt_number-1):
                    weight = float(cpt.split('_')[5])
            begin_marker = 0
            with open(stats_file, "r") as stats:
                # Process each line in the file
                lines = stats.readlines()
                if len(lines) == 0:
                    print("Checkpoint " + str(chkpt_number) + " has empty stats file")
                for line in lines:
                    if len(line.strip().split()) == 0: continue
                    if '----' in line:
                        begin_marker += 1
                        if begin_marker == 3: continue #skip this line
                    if begin_marker != 3: continue
                    if 'bad format' in line: continue
                    field_name = line.strip().split()[0]
                    value = float(line.strip().split()[1]) * weight
                    if field_name in field_names_to_average:
                        field_names_to_average[field_name].append(value)
                    else:
                        aggregated_values[field_name] += value
                if begin_marker != 4:
                    if begin_marker != 2:
                        print("something fucky is happening")
                        exit(1)
                    print("Checkpoint "+str(chkpt_number)+" only has warmup")

    for field_name in field_names_to_average:
        values = field_names_to_average[field_name]
        aggregated_values[field_name] = sum(values) / len(values)

    return aggregated_values

def write_results(results_file, aggregated_values):
    seen_fields = set()
    cpu_fields = {}
    with open(results_file, "w") as file:
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
                file.write(f"{name} {value}\n")
                seen_fields.add(name)
            elif value != 0:
                print("Field "+(field_name)+" has duplicate but doesn't have a value of 0")
        for name, value in cpu_fields.items():
            file.write(f"{name} {value}\n")

field_names_to_aggregate = ["system.switch_cpus.StoreSet__0.BypassStoreSetCheck",
                            "system.switch_cpus.StoreSet__0.baseUsingStoreSetCheck",
                            "system.switch_cpus.StoreSet__0.SSITOverwrites",
                            "system.switch_cpus.StoreSet__0.SSITCollisions",
                            "system.switch_cpus.StoreSet__0.LFSTInvalidations",
                            "system.switch_cpus.lsq0.numStoresSearched",
                            "system.switch_cpus.lsq0.mistakenReschedules",
                            "system.switch_cpus.lsq0.loadToUse::mean",
                            "system.switch_cpus.lsq0.loadToUse::stdev",
                            "system.switch_cpus.iew.cyclesStoreQueueAccessed",
                            "system.switch_cpus.iew.memOrderViolationEvents",
                            "system.switch_cpus.iew.notExactPhysicalAddrViolation",
                            "system.switch_cpus.iew.bypassStoreSetViolationAddition",
                            "system.switch_cpus.numInsts",
                            "system.switch_cpus.numCycles"]


# Output file to store the aggregated results
output_file = "results.txt"

# Aggregate the values for the specified field names
aggregated_values = aggregate_values(field_names_to_aggregate)

aggregated_values["Adjusted Mem Order Violation Events"] = aggregated_values.get("system.switch_cpus.iew.memOrderViolationEvents") - aggregated_values.get("system.switch_cpus.iew.notExactPhysicalAddrViolation")
aggregated_values["Non-PND Violations"] = aggregated_values.get("Adjusted Mem Order Violation Events") - aggregated_values.get("system.switch_cpus.iew.bypassStoreSetViolationAddition")
aggregated_values["CPI"] = aggregated_values.get("system.switch_cpus.numCycles") / aggregated_values.get("system.switch_cpus.commit.instsCommitted")
aggregated_values["Lookup reduction"] = aggregated_values.get("system.switch_cpus.StoreSet__0.BypassStoreSetCheck") / (aggregated_values.get("system.switch_cpus.StoreSet__0.baseUsingStoreSetCheck") + aggregated_values.get("system.switch_cpus.StoreSet__0.BypassStoreSetCheck"))

# Write the results to the output file
write_results(output_file, aggregated_values)
