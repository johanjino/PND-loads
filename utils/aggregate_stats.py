import os
import collections

def aggregate_values(field_names):
    field_names_to_average = {"system.switch_cpus.lsq0.loadToUse::mean": [],
                              "system.switch_cpus.lsq0.loadToUse::stdev": []}
    aggregated_values = collections.defaultdict(float)

    broken_chkpts = []

    # Iterate through each .out directory
    if "only_pna" in os.getcwd():
        pna = ' PNA'
        broken_file = open("broken", "w")
    else:
        pna = 'base'
        broken_chkpts = [i.strip().split(",")[0] for i in open("broken").readlines()[1:-1]]
    print("Benchmark: ", os.getcwd().split("/")[-1]+pna)
    for dirname in os.listdir("."):
        if os.path.isdir(dirname) and dirname[0].isdigit() and dirname.split('.')[1] == 'out':
            stats_file = os.path.join(dirname, "stats.txt")
            chkpt_number = int(dirname.split('.')[0])
            if chkpt_number in broken_chkpts: continue
            for cpt in os.listdir("."):
                if cpt.startswith("cpt.") and int(cpt.split('_')[1]) == (chkpt_number-1):
                    weight = float(cpt.split('_')[5])
            seen_fields = collections.defaultdict(int)
            with open(stats_file, "r") as stats:
                # Process each line in the file
                lines = stats.readlines()
                if len(lines) == 0:
                    print("Checkpoint " + str(chkpt_number) + " has empty stats file")
                    broken_chkpts.append((chkpt_number, weight))
                for line in lines:
                    if len(line.strip().split()) == 0: continue
                    field_name = line.strip().split()[0]
                    if field_name in field_names and seen_fields[field_name] != 0:
                        value = float(line.strip().split()[1]) * weight
                        if field_name in field_names_to_average:
                            field_names_to_average[field_name].append(value)
                        else:
                            aggregated_values[field_name] += value
                    if field_name in field_names:
                        seen_fields[field_name] += 1
                for field in seen_fields:
                    if seen_fields[field] != 2:
                        print("Checkpoint " + str(chkpt_number) + " has seen fields with a value other than 2")
                        broken_chkpts.append(chkpt_number)
                        break

    if len(broken_chkpts) > 0:
        broken_file.write(os.getcwd().split("/")[-1]+'\n')
        total_weight = 0
        for n, w in broken_chkpts:
            broken_file.write(str(n)+","+str(w)+"\n")
            total_weight += w
        broken_file.write("Total missing weight: "+str(total_weight)+"\n")
        broken_file.close()

    for field_name in field_names_to_average:
        values = field_names_to_average[field_name]
        aggregated_values[field_name] = sum(values) / len(values)

    return aggregated_values

def write_results(results_file, aggregated_values):
    with open(results_file, "w") as file:
        for field_name, value in aggregated_values.items():
            name = field_name.split(".")[-1]
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
aggregated_values["CPI"] = aggregated_values.get("system.switch_cpus.numCycles") / aggregated_values.get("system.switch_cpus.numInsts")
aggregated_values["Lookup reduction"] = aggregated_values.get("system.switch_cpus.StoreSet__0.BypassStoreSetCheck") / (aggregated_values.get("system.switch_cpus.StoreSet__0.baseUsingStoreSetCheck") + aggregated_values.get("system.switch_cpus.StoreSet__0.BypassStoreSetCheck"))

# Write the results to the output file
write_results(output_file, aggregated_values)
