#!/usr/bin/env python3.5m

import os
import subprocess
import sys
import tempfile
import zipfile
import re
import glob

REQUIRED_FILES = ['makefile', 'readme']
ALLOWED_FILE_TYPES = ['.c', '.h', '.hpp', '.cpp']
GCC_ARGS = ['-Wall', '-Werror', '-pedantic-errors', '-DNDEBUG', '-pthread']

def main():
    if len(sys.argv) != 3:
        print("Usage: %s <zipfile> <exec_file_name>" % sys.argv[0])
        return os.EX_USAGE

    zip_file = sys.argv[1]
    if not(check_zip(zip_file)):
        print("ERROR: zip naming.")
        return os.EX_DATAERR

    with tempfile.TemporaryDirectory() as path:
        print("Extracting to a temporary folder:", path)
        zip_ref = zipfile.ZipFile(zip_file, 'r')
        zip_ref.extractall(path)
        zip_ref.close()
        if not check_folder(path):
            print("FAILED. Please fix the errors above.")
            return os.EX_DATAERR
        if not build(path, sys.argv[2]):
            print("FAILED. Please fix the errors above.")
            return os.EX_DATAERR
        print("Congratulations! Your submission follows the guidelines and compiles successfully.")

def check_zip(path):
    zip_name, file_ext = os.path.splitext(os.path.basename(path))
    
    if file_ext != '.zip':
        print('ERROR: the archive file name must end with .zip.')
        return False
   
    if not(re.match(r"^[0-9]{9}_[0-9]{9}$", zip_name)):
        print("ERROR: your zip file does not follow the naming convention: id1_id2.")
        return False

    return True

def check_folder(path):
    print("Verifying contents...")
    files = os.listdir(path)
    found_errors = check_folder_contents(files,
                                         REQUIRED_FILES,
                 			 ALLOWED_FILE_TYPES)
    if found_errors:
        print("There are errors in the contents of the ZIP file.")
    else:
        print("The ZIP file contains all the necessary files.")
    return not found_errors

def check_folder_contents(contents, required_files_case_insensitive, allowed_file_types):
    found_errors = False
    for filename in contents:
        if filename.lower() in required_files_case_insensitive:
            print('Found a required file:', filename)
            required_files_case_insensitive.remove(filename.lower())
        elif os.path.splitext(filename)[1] in allowed_file_types:
            print('Found a C/C++ file:', filename)
        else:
            found_errors = True
            print('ERROR: found an unexpected file/directory:', filename)

    if len(required_files_case_insensitive) > 0:
        found_errors = True
        for filename in required_files_case_insensitive:
            print('ERROR: missing the required file:', filename.upper())
        
    return found_errors

def run_command(path, command, command_name):
    print("Executing ", command_name)
    print(' '.join(command))
    if subprocess.call(command, shell=False, cwd=path):
        print("ERROR: failed ", command_name)
        return False
    return True

def build(path, exec_file):
    cpp_files = glob.glob(os.path.join(path, '*.cpp'))
    c_files = glob.glob(os.path.join(path, '*.c'))
    if len(cpp_files) and len (c_files):
        print("ERROR: found both C and C++ source files")
        return False
    cpp = True if len(cpp_files) else False
    compiler = 'g++' if cpp else 'gcc'
    std_ver = '-std=c++11' if cpp else '-std=c99'
    source_files = cpp_files if cpp else c_files
    command = [compiler, std_ver] + GCC_ARGS + source_files + ['-o', exec_file]
    if not(run_command(path, command, "basic compilation")):
        return False
    os.unlink(os.path.join(path, exec_file))
    if not(run_command(path, ['make'], "make")):
        return False
    if not exec_file in os.listdir(path):
        print("ERROR: make failed to create an executable with the expected name: {}".format(exec_file))
        return False
    if not(run_command(path, ['make', 'clean'], "make clean")):
        return False
    if glob.glob(os.path.join(path, '*.o')) or exec_file in os.listdir(path):
        print("ERROR: make clean failed. There are object/executable files in the directory.")
        return False
    return True

if __name__ == '__main__':
    sys.exit(main())
