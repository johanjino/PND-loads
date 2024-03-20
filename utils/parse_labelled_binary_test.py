import r2pipe
import sys

label_binary = sys.argv[1]
verify_binary = sys.argv[2]

r2 = r2pipe.open(label_binary)

addrs = [l.split()[0].strip() for l in r2.cmd("/x ffffffff").split('\n') if len(l) > 0]

r2 = r2pipe.open(verify_binary)

pnd_addrs = set()

for addr in addrs:
    inst = r2.cmdj('pdj 1 @'+str(addr))[0]
    try:
        opcode = inst['opcode'].strip()
    except KeyError:
        continue
    if not opcode.startswith('ldr') and not opcode.startswith('ldur') and not opcode.startswith('ldp'):
        print(inst)
        continue
    pnd_addrs.add(addr)

print(len(pnd_addrs))
