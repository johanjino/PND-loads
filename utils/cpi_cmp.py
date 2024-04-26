import os

stats = {"CPI", "numStoresSearched",
          "StoreSet__0.SSITCollisions",
          "memOrderViolationEvents",
          "StoreSet__0.LFSTInvalidations","cyclesStoreQueueAccessed","baseUsingStoreSetCheck",
         "loadToUse::mean", "loadToUse::stdev", "Lookupreduction", "instsIssued"}

def get_values(results):
    values = {}
    results = open(results, "r").readlines()
    for line in results:
        fields = line.split()
        name = ''.join(fields[:-1])
        if len(name.split('.')) == 2:
            if name == 'lsq0.memOrderViolationEvents': continue #want iew
            name = name.split('.')[1]
        value = fields[-1]
        if name in values and float(value) == 0: continue
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
    pna_results = benchmark+"_pnd"+number+".txt"
    base = get_values(base_results)
    pna = get_values(pna_results)

    differences.write(benchmark+number+":\n")
    differences.write("\tLookup Reduction: "+str(pna['Lookupreduction'] * 100)+"\n")
    differences.write("\tBase CPI: "+str(base['CPI'])+"\n")
    differences.write("\tPND CPI: "+str(pna['CPI'])+"\n")
    differences.write("\tBase Lookups: "+str((base['baseUsingStoreSetCheck']/base['instsIssued'])*1000)+"\n")
    differences.write("\tPND Lookups: "+str((pna['baseUsingStoreSetCheck']/pna['instsIssued'])*1000)+"\n")
    differences.write("\tBase Violations: "+str((base['memOrderViolationEvents']/base['instsIssued'])*1000000)+"\n")
    differences.write("\tPND Violations: "+str((pna['memOrderViolationEvents']/pna['instsIssued'])*1000000)+"\n")
    for field in base:
        if field == 'Lookupreduction' or field == 'baseUsingStoreSetCheck': continue
        base_value = base[field]
        pna_value = pna[field]
        difference = ((pna_value - base_value) / base_value) * 100
        differences.write("\t"+field+": "+str(difference)+"\n")

differences.close()
