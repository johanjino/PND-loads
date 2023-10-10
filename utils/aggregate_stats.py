import os
import collections

def aggregate_values(field_names):
    aggregated_values = collections.defaultdict(float)

    # Iterate through each .out directory
    for dirname in os.listdir("."):
        if os.path.isdir(dirname):
            stats_file = os.path.join(dirname, "stats.txt")
            if os.path.isfile(stats_file):
                seen_fields = set()
                with open(stats_file, "r") as stats:
                    # Process each line in the file
                    lines = stats.readlines()
                    for line in lines:
                        field_name = line.strip().split()[0]
                        if field_name in field_names and field_name in seen_fields:
                            value = float(line.strip().split()[1])
                            aggregated_values[field_name] += value
                        seen_fields.add(field_name)

    return aggregated_values

def write_results(results_file, aggregated_values):
    with open(results_file, "w") as file:
        for field_name, value in aggregated_values.items():
            name = field_name.split(".")[-1]
            file.write(f"{name} {value}\n")

if __name__ == "__main__":
    # List of field names to aggregate
    field_names_to_aggregate = ["system.switch_cpus.StoreSet__0.BypassStoreSetCheck",
                                "system.switch_cpus.StoreSet__0.baseUsingStoreSetCheck",
                                "system.switch_cpus.StoreSet__0.SSITOverwrites",
                                "system.switch_cpus.StoreSet__0.SSITCollisions",
                                "system.switch_cpus.StoreSet__0.LFSTInvalidations",
                                "system.switch_cpus.iew.memOrderViolationEvents",
                                "system.switch_cpus.iew.notExactPhysicalAddrViolation",
                                "system.switch_cpus.numInsts",
                                "system.switch_cpus.numCycles"]

    # Output file to store the aggregated results
    output_file = "results.txt"

    # Aggregate the values for the specified field names
    aggregated_values = aggregate_values(field_names_to_aggregate)

    aggregated_values["adjustedMemOrderViolationEvents"] = aggregated_values.get("memOrderViolationEvents") - aggregated_values.get("notExactPhysicalAddrViolation")
    aggregated_values["cpi"] = aggregated_values.get("numCycles") / aggregated_values.get("numInsts")

    # Write the results to the output file
    write_results(output_file, aggregated_values)
