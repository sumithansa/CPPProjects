# [Refernced this from google and improvised according to the requirement]

import os
import re

def replace_in_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        content = file.read()
    
    # Replace occurrences
    content = re.sub(r'std::string', 'customString', content)
    content = re.sub(r'std::wstring', 'customString', content)
    
    with open(file_path, 'w', encoding='utf-8') as file:
        file.write(content)

def process_folder(directory):
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(('.cpp', '.h')):  # Process only C++ source/header files
                file_path = os.path.join(root, file)
                replace_in_file(file_path)
                print(f"Processed: {file_path}")

if __name__ == "__main__":
    folder_path = os.path.join("D:\\", "C++", "CPPProjects", "Interview", "test")
    process_folder(folder_path)
    print("Replacement completed!")