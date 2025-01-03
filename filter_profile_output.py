import re
import os
import glob

# Define the pattern to match the required format
pattern = re.compile(
    r"^(Load|Store) "
    r"(/[^\s]+)+ "  # Path to file
    r"\d+ "         # Line number
    r"\d+ "         # Column number
    r"0x[0-9a-fA-F]+$"  # Address in hexadecimal
)

def filter_and_process_lines(input_file, output_file):
    """
    Reads a text file, filters lines matching a specific format, and writes them to a new file.
    """
    load_to_stores_map = {}
    address_to_stores_map = {}
    with open(input_file, 'r') as infile:
        for line in infile:
            line = line.strip()
            if pattern.match(line):
                op, file_loc, line_num, col_num, address = line.split()
                if op == "Load":
                    key = op + " " + line_num + ":" + col_num + " " + file_loc
                    if address in address_to_stores_map:
                        if key in load_to_stores_map :
                            load_to_stores_map[key] = load_to_stores_map[key] | address_to_stores_map[address]
                        else:
                            load_to_stores_map[key] = address_to_stores_map[address]

                elif op == "Store":
                    loc = line_num + ":" + col_num
                    if address in address_to_stores_map:
                        address_to_stores_map[address] = address_to_stores_map[address] | set({loc})
                    else:
                        address_to_stores_map[address] = set({loc})
            # else:
            #     print("Filtering line: ", line)
    
    with open(output_file, 'w') as outfile:
        for load, values in load_to_stores_map.items():
            output_line = load + " "
            for store_loc in values:
                output_line += store_loc + " "
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
