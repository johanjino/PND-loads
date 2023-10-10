import sys
import re

name = sys.argv[1]
addrspace = sys.argv[2]
prep = True if len(sys.argv) == 3 else False

ir = open(name, "r").readlines()

def add_addrspace(line):
    if line.find("*") != -1:
        addrspace_string = " addrspace("+addrspace+")*"
        line_list = line.split("*")
        for i in range(len(line_list)-1):
            line_list[i] = line_list[i] + addrspace_string
        return ''.join(line_list)
    elif line.find("ptr") != -1:
        addrspace_string = " addrspace("+addrspace+")"
        count = line.count("ptr")
        start = 0
        if line.find("getelementptr") != -1:
            start = 1
        for i in range(start, count):
            ptrs = [i for i in re.finditer("ptr", line)]
            ptr = ptrs[i].end()
            line_list = list(line)
            line_list.insert(ptr, addrspace_string)
            line = ''.join(line_list)
        return line
    else:
        print("line not a pointer!")
        print(line)
        exit(1)

def is_pointer_type(line):
    first_type = line.split(',')[0]
    return (first_type[-1] == '*' or first_type[len(first_type)-3:len(first_type)] == 'ptr'
            or line.find("getelementptr") != -1 or (line.find("phi") != -1 and line.split('=')[1].split()[1] == "ptr") or
            line.find("bitcast") != -1)

def update_use_chain(ir, start, ssa_name):
	regex = "("+ssa_name+")[\D|$]"
	for c, line in enumerate(ir[start:], start):
		if line.find("}") != -1:
			return
        #causes problems when your pointer is being compared to a pointer of another addrspace
        #sometimes easier to handle these cases by hand, sometimes not
		# if line.find("icmp") != -1:
		# 	continue
		if re.search(regex, line) is not None and line.find("addrspace") == -1:
			ir[c] = add_addrspace(line)
			if is_pointer_type(line):
				this_ssa_name = line.split("=")[0].strip()
				update_use_chain(ir, c+1, this_ssa_name)

if prep:
    for c, line in enumerate(ir):
        if line.find("getelementptr") != -1 and line.find("%pna") != -1 and line.find(";!predict") == -1:
            ir[c] = line.strip('\n') + " ;!predict\n"

for c, line in enumerate(ir):
	if line.find("!predict") != -1:
		ssa_name = line.split("=")[0].strip()
		ir[c] = add_addrspace(line)
		update_use_chain(ir, c+1, ssa_name)

if addrspace == "420": predict = "pa"
elif addrspace == "69": predict = "pna"

output = open(name.split('.')[0]+"-"+predict+".ll", "w")
for line in ir:
	output.writelines(line)
output.close()
