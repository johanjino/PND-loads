import os

stats = {"CPI", "numStoresSearched",
          "SSITCollisions",
          "memOrderViolationEvents",
          "LFSTInvalidations", "cyclesStoreQueueAccessed",
         "loadToUse::mean", "loadToUse::stdev", "BypassStoreSetCheck", "baseUsingStoreSetCheck", "numInsts"}

def get_values(results):
    values = {}
    results = open(results, "r").readlines()
    for line in results:
        fields = line.split()
        name = ''.join(fields[:-1])
        if len(name.split('.')) == 2 and "thread_0" not in name:
            name = name.split('.')[1]
        value = fields[-1]
        if name in stats:
            if float(value) == 0: continue
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

    pna_lookups_per_kinst = (pna['baseUsingStoreSetCheck'] / pna['numInsts']) * 1000
    base_lookups_per_kinst = (base['baseUsingStoreSetCheck'] / base['numInsts']) * 1000
    # pna_cycles_sq_accessed_per_inst = pna['cyclesStoreQueueAccessed'] / pna['numInsts']
    # base_cycles_sq_accessed_per_inst = base['cyclesStoreQueueAccessed'] / pna['numInsts']
    differences.write(benchmark+number+":\n")
    differences.write("\tBase Lookups: "+str(base_lookups_per_kinst)+"\n")
    differences.write("\tPND Lookups: "+str(pna_lookups_per_kinst)+"\n")
    #differences.write("\tLookup Reduction: "+str(-(pna_lookups_per_kinst - base_lookups_per_kinst)/base_lookups_per_kinst * 100)+"\n")
    # differences.write("\tCycles SQ Accessed Per Inst PND: "+str(pna_cycles_sq_accessed_per_inst)+"\n")
    # differences.write("\tCycles SQ Accessed Per Inst Base: "+str(base_cycles_sq_accessed_per_inst)+"\n")
    for field in base:
        if field == "Lookupreduction": continue
        base_value = base[field]
        pna_value = pna[field]
        difference = ((pna_value - base_value) / base_value) * 100
        differences.write("\t"+field+": "+str(difference)+"\n")

differences.close()
