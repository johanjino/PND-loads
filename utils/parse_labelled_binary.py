import r2pipe
import sys

label_binary = sys.argv[1]
verify_binary = sys.argv[2]
addr_file_type = sys.argv[3]
bench = verify_binary.split("/")[-4]
addr_file_dir = "/work/muke/PND-Loads/addr_files/"+addr_file_type+"/"

r2 = r2pipe.open(label_binary)

addrs = [l.split()[0] for l in r2.cmd("/x ffffffff").split('\n') if len(l) > 0]

print()
print(len(addrs))
print()

r2 = r2pipe.open(verify_binary)

pnd_addrs = []

for addr in addrs:
    inst = r2.cmdj('pdj 1 @'+str(addr))[0]
    try:
        opcode = inst['opcode'].strip()
    except KeyError:
        continue
    if not opcode.startswith('ldr') and not opcode.startswith('ldur') and not opcode.startswith('ldp') and not opcode.startswith('ld1'):
        if opcode != "invalid":
            print("Warning: found a non-load instruction: ", opcode)
            print(inst)
        continue
    pnd_addrs.append(int(addr,16))

print()
print(len(pnd_addrs))
print()

address_file = open(addr_file_dir+bench, "w")
for addr in pnd_addrs:
    address_file.write(str(addr)+"\n")
address_file.close()
