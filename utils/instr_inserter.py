import re

type_one_loads = list(map(str.upper, ["ldrbb", "ldrhh", "ldrsw", "ldrx", "ldrw", "ldrs", "ldrd", "ldrq", "ldrshw", "ldrshx", "ldrsbw", "ldrsbx"]))
type_two_loads = list(map(str.upper, ["ldpw", "ldps", "ldpsw", "ldpx", "ldpd", "ldpq"]))
type_three_loads = list(map(str.upper, ["ldurbb", "ldurhh", "ldurw", "ldurshx", "ldursbw", "ldursbx", "ldursw", "ldurs", "ldurx", "ldurd", "ldurq"]))
literals = list(map(str.upper, ['ldrw', 'ldrx', 'ldrs', 'ldrd', 'ldrq']))

files = ["/home/muke/Programming/Huawei/llvm-project/llvm-alias-pass/llvm/lib/Target/AArch64/AArch64"+s+".cpp" for s in ["LoadStoreOptimizer", "InstrInfo", "FrameLowering"]] + ["/home/muke/Programming/Huawei/llvm-project/llvm-alias-pass/llvm/lib/Target/AArch64/AsmParser/AArch64AsmParser.cpp"]+["/home/muke/Programming/Huawei/llvm-project/llvm-alias-pass/llvm/lib/Target/AArch64/Disassembler/AArch64Disassembler.cpp"]

def insert_pass_cases(load, variant, start, lines):
    insertion = """
      case AArch64::{org}:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::{pna}; //{org}-No-Predict-Alias
        else continue;
        break;
    """.format(org = load+variant, pna = load+"PNA"+variant)
    lines.insert(start, insertion)

def remove_duplicate_cases(load, label, variant, lines):
    for c, line in enumerate(lines):
        if re.search(r'case AArch64::'+re.escape(load+label+variant)+r':$', line) != None:
            if re.search(r'case AArch64::'+re.escape(load+label+variant)+r':$', lines[c+1]) != None:
                print(lines[c])
                del lines[c]

def insert_load(load, label, variant, lines, name):
    for c, line in enumerate(lines):
        if re.search(r'case AArch64::'+re.escape(load+variant)+r':$', line) != None:
            lines.insert(c+1, "case AArch64::"+load+label+variant+":\n")
        # elif re.search(re.escape(load+variant), line) != None:
        #     print("=============", end=' ')
        #     print("at line "+str(c+1)+" in "+name.split("/")[-1]+":")
        #     for i in range(c-5, c+5):
        #         if (i == c):
        #             print("> ", end='')
        #         print(lines[i])
        #     input("")

# def insert_neithers(load, lines):
#     for c, line in enumerate(lines[:-2]):
#             lines[c+1] = line.replace("defm "+load, "defm "+load+"PNA")
#             lines[c+2] = line.replace("defm "+load, "defm "+load+"PA")

# for name in files:
#     reader = open(name, "r")
#     lines = reader.readlines()
#     for load in new_loads:
#         for label in ["PNA", "PA"]:
#             for variant in ["rox", "row", "ui", "pre", "post"]:
#                 #remove_duplicate_cases(load, label, variant, lines)
#                 insert_load(load, label, variant, lines, name)
    # for load in type_one_loads:
    #     for label in ["PNA", "PA"]:
    #         for variant in ["rox", "row", "ui", "pre", "post"]:
    #             remove_duplicate_cases(load, label, variant, lines)
    #             #insert_load(load, label, variant, lines, name)
    # for load in type_two_loads:
    #     for label in ["PNA", "PA"]:
    #         for variant in ["pre", "post", "i"]:
    #             remove_duplicate_cases(load, label, variant, lines)
    #             #insert_load(load, label, variant, lines, name)
    # for load in type_three_loads:
    #     for label in ["PNA", "PA"]:
    #         for variant in ["i"]:
    #             remove_duplicate_cases(load, label, variant, lines)
    #             #insert_load(load, label, variant, lines, name)
    # reader.close()
    # writer = open(name, "w")
    # writer.writelines(lines)
    # writer.close()

pass_file_name = "/home/muke/Programming/PND-Loads/llvm/llvm/lib/Target/AArch64/AArch64LoadAliasMetadataInsertionPass.cpp"
reader = open(pass_file_name, "r")
lines = reader.readlines()
start = None
for c, line in enumerate(lines):
    if re.search(re.escape("switch (MI.getOpcode())"), line) != None:
        start = c
if not start:
    raise Exception("switch start not found")
for load in type_one_loads:
    for variant in ["roX", "roW", "ui", "pre", "post", "l"]:
        if variant != "l" or (variant == "l" and load in literals):
            insert_pass_cases(load, variant, start, lines)
for load in type_two_loads:
    for variant in ["pre", "post", "i"]:
        insert_pass_cases(load, variant, start, lines)
for load in type_three_loads:
    for variant in ["i"]:
        insert_pass_cases(load, variant, start, lines)

# file_name = "/home/muke/Programming/Huawei/llvm-project/llvm/lib/Target/AArch64/AArch64InstrInfo.td"
# reader = open(file_name, "r")
# lines = reader.readlines()
# for ty in [type_one_loads, type_two_loads]:
#     for load in ty:
#         insert_neithers(load, lines)
reader.close()
writer = open(pass_file_name, "w")
writer.writelines(lines)
writer.close()
