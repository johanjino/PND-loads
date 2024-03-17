import r2pipe
import sys

label_binary = sys.argv[1]
verify_binary = sys.argv[2]
addresses = []

r2 = r2pipe.open(label_binary)

addrs = [l.split()[0] for l in r2.cmd("/x ffffffff").split('\n') if len(l) > 0]

r2 = r2pipe.open(verify_binary)

pnd_addrs = []

for addr in addresses:
    inst = r2.cmdj('pdj 1 @'+str(addr))[0]
    opcode = inst['opcode'].strip()
    if not opcode.startswith('ldr') and not opcode.startswith('ldur') and not opcode.startswith('ldp'):
        continue
    pnd_addrs.append(addr)

address_file = open(verify_binary+"_pnd_address", "w")
for addr in pnd_addrs:
    address_file.write(str(addr)+"\n")
address_file.close()
