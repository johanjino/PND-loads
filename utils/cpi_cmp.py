import os

stats = {"CPI", "numStoresSearched",
          "SSITCollisions",
          "memOrderViolationEvents",
          "LFSTInvalidations", #"mistakenReschedules", "cyclesStoreQueueAccessed",
         "loadToUse::mean", "loadToUse::stdev"}

def get_values(results):
    values = {}
    results = open(results, "r").readlines()
    for line in results:
        fields = line.split()
        name = ''.join(fields[:-1])
        value = fields[-1]
        if name in stats:
            values[name] = float(value)
    return values

differences = open("differences", "w")
processed_benchmarks = set()
for f in os.listdir(os.getcwd()):
    if os.path.isdir(f) or not f.endswith(".txt") or f == "differences" or f == "cpi_differences": continue
    benchmark = f.split('_')[0]
    number = ""
    if f.count('.') == 2: #has multiple workloads
        number = "."+f.split('.')[1]

    if benchmark+number in processed_benchmarks: continue
    processed_benchmarks.add(benchmark+number)

    base_results = benchmark+"_base"+number+".txt"
    pna_results = benchmark+"_pna"+number+".txt"
    base = get_values(base_results)
    pna = get_values(pna_results)

    differences.write(benchmark+number+":\n")
    for field in base:
        base_value = base[field]
        pna_value = pna[field]
        difference = ((pna_value - base_value) / base_value) * 100
        differences.write("\t"+field+": "+str(difference)+"\n")

differences.close()
