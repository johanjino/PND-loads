import re
import os
import glob

# Define the pattern to match the required format
pattern = re.compile(
    r"^(Load|Store) "
    r"(/[^\s]+)+ "  # Path to file
    r"\d+ "         # Line number
    r"\d+ "         # Column number
    r"0x[0-9a-fA-F]+ "  # Address in hexadecimal
    r"-?\d+$"         # Instruction Count
)

def filter_and_process_lines(input_file, output_file):
    load_to_stores_map = {}
    load_execution_count = {}
    address_to_latest_store_map = {}
    last_op_on_address = {}
    total_far = 0
    prev_inst_count = 0
    with open(input_file, 'r') as infile:
        for line in infile:
            line = line.strip()
            if pattern.match(line):
                op, file_loc, line_num, col_num, address, inst_count = line.split()
                inst_count = int(inst_count)
                if inst_count < prev_inst_count:
                    print("Instruction Count overflow?")
                    print(prev_inst_count)
                    print(inst_count)
                    break
                if op == "Load":
                    key = op + " " + line_num + ":" + col_num + " " + file_loc
                    if key in load_execution_count:
                        load_execution_count[key] += 1
                    else: 
                        load_execution_count[key] = 1
                    if address in address_to_latest_store_map and last_op_on_address[address]=="Store":
                        store_loc, store_inst_count = address_to_latest_store_map[address]
                        if (inst_count - store_inst_count <= 1000):
                            if key in load_to_stores_map :
                                if store_loc in load_to_stores_map[key]:
                                    load_to_stores_map[key][store_loc] += 1
                                else:
                                    load_to_stores_map[key][store_loc] = 1
                            else:
                                load_to_stores_map[key] = {store_loc:1}
                        else:
                            # print("Caught a too far case")
                            total_far += 1
                    last_op_on_address[address] = "Load"

                elif op == "Store":
                    loc = line_num + ":" + col_num
                    address_to_latest_store_map[address] = [loc, inst_count]
                    last_op_on_address[address] = "Store"
                prev_inst_count = inst_count
            # else:
            #     print("Filtering line: ", line)
    
    print("Total too far cases caught: ", total_far)

    with open(output_file, 'w') as outfile:
        for load, values in load_to_stores_map.items():
            output_line = load + " " + str(load_execution_count[load]) + " "
            for store_loc, count in values.items():
                output_line += store_loc + " " + str(count) + " "
            outfile.write(output_line + '\n')


# Usage
if __name__ == "__main__":
    # Directory where the raw text files are located
    raw_dir = "/rds/general/user/jj21/home/fyp/pnd-loads/profile_files/raw/"
    processed_dir = "/rds/general/user/jj21/home/fyp/pnd-loads/profile_files/processed/"
    
    # Get all .txt files in the raw directory
    txt_files = glob.glob(os.path.join(raw_dir, "*.txt"))
    
    # Iterate over each file in the raw folder
    for input_file in txt_files:
        print("Processing: ", input_file)
        print()

        # Create an output file path based on the input file name
        output_file = os.path.join(processed_dir, os.path.basename(input_file).replace(".txt", "-filtered.txt"))
        
        # Call the filter_lines function for each file
        filter_and_process_lines(input_file, output_file)
        print(f"Filtered lines written to {output_file}")
        print()
