import os
from scipy.stats import gmean
import prettytable

benches = {}
fields = ['instsIssued', 'StoreSet__0.BypassStoreSetCheck']
for f in os.listdir('.'):
    if 'pnd' in f:
        name = f.split('_')[0]+'.'+f.split('.')[1]
        data = open(f).readlines()
        for line in data:
            try:
                field = line.split()[0]
                if field in fields:
                    value = float(line.split()[1])
                    if benches.get(name):
                        benches[name][field] = value
                    else:
                        benches[name] = {field: value}
            except IndexError:
                continue

table = prettytable.PrettyTable()
table.field_names = ["Benchmark","Base Bandwidth","Label Bandwidth","Total Bandwidth","Percentage Change"]
percentages = []
for bench in benches:
    base_bandwidth = round(benches[bench]['instsIssued']*32)
    label_bandwidth = round(benches[bench]['StoreSet__0.BypassStoreSetCheck']*8)
    total_bandwidth = round(base_bandwidth + label_bandwidth)
    percentage_change = round(((total_bandwidth - base_bandwidth)/base_bandwidth) * 100,2)
    percentages.append(percentage_change)
    table.add_row([bench,base_bandwidth,label_bandwidth,total_bandwidth,percentage_change])
print(table)
print("Average Percent Change: ",round(gmean(percentages),2))
