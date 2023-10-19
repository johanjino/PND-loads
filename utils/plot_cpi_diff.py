import matplotlib.pyplot as plt

results = open("cpi_differences").readlines()

names = []
values = []
for l in results:
   names.append(l.split(": ")[0])
   values.append(float(l.split(": ")[1]))

fig, ax = plt.subplots()
ax.bar(names, values)
ax.set_xlabel('CPI Differences')
ax.set_ylabel('Percent Change')
ax.set_title('Differences in CPI Between Base and Modified: Default, 32 SSIT')
plt.show()
