import sys
f = open(sys.argv[1])

ir = 0
backend = 0
for line in f:
    if line.startswith("COUNT"):
        ir += 1
    elif line.startswith("BACKEND"):
        backend += 1

print("IR count: ", ir)
print("Backend count: ", ir)
