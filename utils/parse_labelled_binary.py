import r2pipe
import sys

compiler_output_file = sys.argv[1]
binary = sys.argv[2]
compiler_output = open(compiler_output_file).readlines()
load_indexes = {}

for line in compiler_output:
    if line.startswith("FUNC"):
        func_name = line.split(":")[1]
        array = line.split(":")[2]
        counts = array.split(",")
        load_indexes[func_name] = [int(i) for i in counts]

r2 = r2pipe.open(binary)

funcs = [i for i in r2.cmdj('isj') if i['type'] == 'FUNC' and i['name'] in load_indexes]

pnd_addrs = []
for func in funcs:
    addr = func['vaddr']
    num_insts = func['size'] / 4
    r2.cmd('s '+str(addr))
    insts = r2.cmdj('pdj '+str(num_insts))
    counts = load_indexes[func['name']]
    loads = 0
    for i in insts:
        opcode = i['opcode']
        if opcode.startswith('ldr') or opcode.startswith('ldur') or opcode.startswith('ldp'):
            loads += 1
            if loads in counts:
                pnd_addrs.append(i['offset'])

address_file = open(binary+"_pnd_address", "w")
for addr in pnd_addrs:
    address_file.write(str(addr)+"\n")
address_file.close()
