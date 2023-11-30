# OOPD_Project

# MyShell - Simple Command Line Shell
MyShell is a basic command line shell implemented in C++ using the filesystem library. It provides a set of commands for file and directory manipulation.

Features
1. Navigation Commands: Change directory (cd), list directory contents (ls).
2. File and Directory Operations: Move (mv), copy (cp), and remove (rm) files and directories.
3. Options for Move and Copy: Recursive move and copy, interactive mode, backup creation.
4. Threading Support: Choose between normal recursion and threaded recursion for improved performance.
5. Help Commands: Get help for specific commands using --help or -h options.

Available Commands:

1. cd: Change directory.
       cd command Options:
      -u, --up       : Move up one level in the directory hierarchy.
      ~              : Move to the root directory.
      -l, --list     : List contents of the current directory.
      --help, -h     : Display this help message.
2. ls: List directory contents.
       ls command Options:
      -r             : List subdirectories recursively.
      --hidden       : Include hidden files and directories.
      --size         : Display file sizes.
      --sort         : Sort entries alphabetically.
      --help         : Display this help message.
3. mv: Move files or directories.
       mv command Options:
      -r             : Move directories recursively.
      -rt            : Move directories recursively with Threading.
      -i             : Prompt before overwriting files.
      -b             : Create a backup of the destination file.
      --help         : Display this help message.
4. cp: Copy files or directories.
       cp command Options:
      -r             : Copy directories recursively
      -rt            : Copy directories recursively with Threading
      -i             : Prompt before overwriting files
      -b             : Create a backup of the destination file
      --help         : Display this help message
5. rm: Remove files or directories.
       rm command Options:
      -r,            : remove directories and their contents recursively
      -f             : ignore nonexistent files and arguments, never prompt
      -b             : create backups of removed files with a .bak extension
      -v             : explain what is being done
      -h, --help     : display this help and exit
6. exit: Exit the shell.

# Profiling

This script automates the creation of directories with large files for testing and benchmarking purposes. It creates three different scenarios:

1. directory1: Contains 100 files, each of 1GB in size.
2. directory2: Contains 1000 files, each of 10MB in size.
3. directory3: Contains 100 files of 10MB each and subdirectories with 10 files of 10MB each in 50 subdirectories.
