import os
import pandas as pd



titles = [
     "simTicks", 
    "system.cpu.totalCpi",
    "system.cpu.MemDepUnit__0.baseUsingStoreSetCheck",
    "system.cpu.MemDepUnit__0.BypassStoreSetCheck",
    "system.cpu.iew.baseStoreSetViolationAddition",
    # "system.cpu.iew.bypassStoreSetViolationAddition",
    "system.cpu.iew.memOrderViolationEvents",
    "system.cpu.iew.notExactPhysicalAddrViolation"
]

clean_titles = [
    "Simulation Ticks", 
    "CPI",
    "Storeset checks",
    "Storeset checks bypassed",
    "Violation pair added to storeset",
    "Memory Order Violations",
    "Mismatched memory violation catches",
    "Adjusted number of violations"
]

benchmark_data = []
indexes = []

def extractData(file):
    mylines = []
    extract = []
    title = []
    data = []
    with open (file, 'rt') as myfile:
        print(file)
        print("--------")
        # data.append(file)
        for line in myfile:                   # For each line of text,
            mylines.append(line)

        for line in mylines:
            for token in line.split():
                try:
                    if token in titles:
                        extract.append(line)
                        title.append(token)
                except ValueError:
                    continue
        for line in extract:
            for token in line.split():
                try:
                    data.append(float(token))
                except ValueError:
                    continue
    # calculating adjusted number of violations        
    data.append(data[5]-data[6])
    return data

for filename in os.listdir("results/"):
    if filename.endswith(".txt"): 
        d = extractData('results/'+ filename)
        benchmark_data.append(d)
        indexes.append(filename[:-10])

df = pd.DataFrame(benchmark_data, columns = clean_titles, index= indexes)
df.to_csv("20-itPA.csv")
# extractData()


