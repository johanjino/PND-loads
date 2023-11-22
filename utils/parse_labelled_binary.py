import r2pipe
import sys

label_binary = sys.argv[1]
verify_binary = sys.argv[2]
addresses = []

r2 = r2pipe.open(label_binary)

r2.cmd('aaa')

funcs = [i for i in r2.cmdj('isj') if i['type'] == 'FUNC']

for func in funcs:
    addr = func['vaddr']
    num_insts = func['size'] / 4
    r2.cmd('s '+str(addr))
    insts = r2.cmdj('pdj '+str(num_insts))
    for i in insts:
        if i['opcode'] == "invalid":
            addresses.append(i['offset'])

r2 = r2pipe.open(verify_binary)

for addr in addresses:
    inst = r2.cmdj('pdj 1 @'+str(addr))[0]
    if inst['type'] != 'load' and inst['type'] != 'lea':
        print("Found instruction at address "+str(addr)+" that isn't a load!")
        print("Instruction: ", r2.cmd('pd 1 @'+str(addr)))
        exit(1)

print("Verified")

address_file = open(verify_binary+"_pnd_address", "w")
for addr in addresses:
    address_file.write(str(addr)+"\n")
address_file.close()
