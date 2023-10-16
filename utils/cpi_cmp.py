import os

def get_cpi_value(results):
    results = open(results, "r").readlines()
    for line in results:
        name, value = line.split(":")
        if name == "cpi":
            return float(value)

cpi_differences = open("cpi_differences", "w")
processed_benchmarks = set()
for f in os.listdir(os.getcwd()):
    if not os.path.isdir(f) and not f.endswith(".txt"): continue
    benchmark = f.split('_')[0]
    number = ""
    if f.count('.') == 2: #has multiple workloads
        number = "."+f.split('.')[1]

    if benchmark+number in processed_benchmarks: continue
    processed_benchmarks.add(benchmark+number)

    base_results = benchmark+"_base"+number+".txt"
    pna_results = benchmark+"_pna"+number+".txt"
    base_cpi = get_cpi_value(base_results)
    pna_cpi = get_cpi_value(pna_results)
    difference = ((pna_cpi - base_cpi) / base_cpi) * 100

    cpi_differences.write(benchmark+number+": "+str(difference)+"\n")

cpi_differences.close()
