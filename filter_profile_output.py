import re

# Define the pattern to match the required format
pattern = re.compile(
    r"^(Load|Store) "
    r"(/[^\s]+)+ "  # Path to file
    r"\d+ "         # Line number
    r"\d+ "         # Column number
    r"0x[0-9a-fA-F]+$"  # Address in hexadecimal
)

def filter_lines(input_file, output_file):
    """
    Reads a text file, filters lines matching a specific format, and writes them to a new file.
    """
    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        for line in infile:
            line = line.strip()
            if pattern.match(line):
                outfile.write(line + '\n')
            else:
                print("Filtering line: ", line)

# Usage
if __name__ == "__main__":
    input_file = "/rds/general/user/jj21/home/fyp/pnd-loads/sha-profile.txt"  # Replace with your input file name
    output_file = "/rds/general/user/jj21/home/fyp/pnd-loads/sha-profile-filtered.txt"  # Replace with your desired output file name
    filter_lines(input_file, output_file)
    print(f"Filtered lines written to {output_file}")
