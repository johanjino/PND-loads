import re
import os

def process_files_in_directory(directory_path):
    regex_pattern = re.compile(r'LR.*PA')

    for root, dirs, files in os.walk(directory_path):
        for filename in files:
            if filename.endswith(".cpp"):
                file_path = os.path.join(root, filename)

                try:
                    with open(file_path, 'r') as f:
                        lines = f.readlines()

                    # Filter out lines matching the regex pattern
                    modified_lines = [line for line in lines if not regex_pattern.search(line)]

                    # Overwrite the file with modified content
                    with open(file_path, 'w') as f:
                        f.writelines(modified_lines)

                    print(f"Processed {file_path}")

                except Exception as e:
                    print(f"Error processing {file_path}: {str(e)}")

if __name__ == "__main__":
    # Example: Directory path containing .cpp files
    directory_path = "~/Programming/PND-Loads/llvm/llvm/lib/Target/AArch64"

    process_files_in_directory(directory_path)
