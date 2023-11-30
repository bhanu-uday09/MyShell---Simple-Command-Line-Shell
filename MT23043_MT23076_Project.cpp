#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

class MyShell
{
public:
    void run()
    {
        std::string command;
        while (true)
        {
            std::cout << currentDirectory << " $ ";
            std::getline(std::cin, command);

            if (command == "exit")
            {
                break;
            }

            executeCommand(command);
        }
    }

private:
    std::string currentDirectory = fs::current_path().string();

    void executeCommand(const std::string &command)
    {
        std::vector<std::string> args = splitCommand(command);

        if (args.empty())
        {
            return;
        }

        std::string cmd = args[0];
        args.erase(args.begin()); // Remove command from arguments

        if (cmd == "cd")
        {
            changeDirectory(args);
        }
        else if (cmd == "mv")
        {
            move(args);
        }
        else if (cmd == "rm")
        {
            remove(args);
        }
        else if (cmd == "ls")
        {
            lsDirectory(args);
        }
        else if (cmd == "cp")
        {
            copy(args);
        }
        else
        {
            std::cout << "Command not recognized: " << cmd << std::endl;
        }
    }

    std::vector<std::string> splitCommand(const std::string &command)
    {
        std::vector<std::string> result;
        std::istringstream iss(command);
        for (std::string s; iss >> s;)
        {
            result.push_back(s);
        }
        return result;
    }

    void changeDirectory(const std::vector<std::string> &args)
    {
        if (args.empty())
        {
            std::cout << "cd command requires a target directory." << std::endl;
            return;
        }

        std::string targetDirectory = args[0];

        if (targetDirectory == "-h" || targetDirectory == "--help")
        {
            printCdHelp();
            return;
        }

        if (targetDirectory == "-u" || targetDirectory == "--up")
        {
            // Move up one level in the directory hierarchy
            fs::current_path(fs::current_path().parent_path());
            currentDirectory = fs::current_path().string();
        }
        else if (targetDirectory == "~")
        {
            // Move to the root directory
            fs::current_path(fs::path("/"));
            currentDirectory = fs::current_path().string();
        }
        else if (targetDirectory == "-l" || targetDirectory == "--list")
        {
            // List contents of the current directory
            lsDirectory({});
        }
        else
        {
            // Change to the specified directory
            try
            {
                fs::current_path(targetDirectory);
                currentDirectory = fs::current_path().string();
            }
            catch (const fs::filesystem_error &e)
            {
                std::cout << "Directory not found: " << targetDirectory << std::endl;
            }
        }
    }

    void printCdHelp()
    {
        std::cout << "cd command options:" << std::endl;
        std::cout << "  -u, --up       : Move up one level in the directory hierarchy." << std::endl;
        std::cout << "  ~              : Move to the root directory." << std::endl;
        std::cout << "  -l, --list     : List contents of the current directory." << std::endl;
        std::cout << "  --help, -h     : Display this help message." << std::endl;
    }

    void moveRecursiveThread(const fs::path &source, const std::string &destination)
    {
        for (const auto &entry : fs::directory_iterator(source))
        {
            const fs::path currentPath = entry.path();
            const fs::path newPath = fs::path(destination) / currentPath.filename().string();

            if (fs::is_directory(currentPath))
            {
                std::thread moveThread(&MyShell::moveRecursiveThread, this, currentPath, newPath.string());
                moveThread.detach(); // Detach the thread to run asynchronously
            }
            else
            {
                fs::rename(currentPath, newPath);
            }
        }
    }

    void move(const std::vector<std::string> &args)
    {
        if (args.size() < 2)
        {
            std::cout << "mv command requires source and destination paths." << std::endl;
            return;
        }

        bool recursiveMode = std::find(args.begin(), args.end(), "-rt") != args.end();
        bool recursiveThreadMode = std::find(args.begin(), args.end(), "-r") != args.end();
        bool interactiveMode = std::find(args.begin(), args.end(), "-i") != args.end();
        bool backupMode = std::find(args.begin(), args.end(), "-b") != args.end();
        bool helpMode = std::find(args.begin(), args.end(), "--help") != args.end();

        if (helpMode)
        {
            displayMoveHelp();
            return;
        }

        std::string source;
        std::string destination;

        // Parse options and paths
        for (const auto &arg : args)
        {
            if (arg == "-rt")
            {
                recursiveMode = true;
            }
            else if (arg == "-r")
            {
                recursiveThreadMode = true;
            }
            else if (arg == "-i")
            {
                interactiveMode = true;
            }
            else if (arg == "-b")
            {
                backupMode = true;
            }
            else if (source.empty())
            {
                source = arg;
            }
            else
            {
                destination = arg;
            }
        }

        // Make source path absolute
        fs::path absoluteSource = fs::absolute(source);

        // Check if source exists
        if (!fs::exists(absoluteSource))
        {
            std::cout << "Source does not exist: " << absoluteSource << std::endl;
            return;
        }

        try
        {
            if (fs::is_directory(absoluteSource))
            {
                if (recursiveMode)
                {
                    std::cout<<"Threaded Recursion\n";
                    auto start = std::chrono::high_resolution_clock::now(); // Start time
                    // Manually move the directory recursively
                    moveRecursiveThread(absoluteSource, destination);
                    auto end = std::chrono::high_resolution_clock::now(); // End time
                    std::chrono::duration<double> duration = (end - start) * 1000;
                    std::cout << "Move execution time: " << duration.count() << " milliseconds" << std::endl;
                }
                else if (recursiveThreadMode)
                {
                    std::cout<<"Normal Recursion\n";
                    auto start = std::chrono::high_resolution_clock::now(); // Start time
                    // Use multi-threading for recursive move
                    std::thread moveThread(&MyShell::moveRecursiveThread, this, absoluteSource, destination);
                    moveThread.join();                                    // Wait for the thread to finish
                    auto end = std::chrono::high_resolution_clock::now(); // End time
                    std::chrono::duration<double> duration = (end - start) * 1000;
                    std::cout << "Move execution time: " << duration.count() << " milliseconds" << std::endl;
                }
                else
                {
                    // Use fs::rename for non-recursive directory move
                    fs::rename(absoluteSource, destination);
                }
            }
            else
            {
                // Use fs::rename for file move
                fs::rename(absoluteSource, destination);
            }

            std::cout << "Successfully moved " << absoluteSource << " to " << destination << std::endl;

            if (interactiveMode)
            {
                std::cout << "Do you want to overwrite " << destination << "? (y/n): ";
                char response;
                std::cin >> response;
                if (response != 'y' && response != 'Y')
                {
                    std::cout << "Move canceled." << std::endl;
                    return;
                }
            }

            if (backupMode)
            {
                fs::copy(destination, destination + ".bak", fs::copy_options::overwrite_existing);
                std::cout << "Backup created for " << destination << " as " << destination + ".bak" << std::endl;
            }
        }
        catch (const fs::filesystem_error &e)
        {
            std::cout << "Error moving files: " << e.what() << std::endl;
        }
    }

    void displayMoveHelp()
    {
        std::cout << "Usage: mv [options] source destination" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -r            Move directories recursively." << std::endl;
        std::cout << "  -rt           Move directories recursively with Threading." << std::endl;
        std::cout << "  -i            Prompt before overwriting files." << std::endl;
        std::cout << "  -b            Create a backup of the destination file." << std::endl;
        std::cout << "  --help        Display this help message." << std::endl;
    }

    void remove(const std::vector<std::string> &args)
    {
        if (args.empty())
        {
            std::cout << "rm command requires at least one file or directory to remove." << std::endl;
            return;
        }

        bool recursiveMode = std::find(args.begin(), args.end(), "-r") != args.end() ||
                             std::find(args.begin(), args.end(), "--recursive") != args.end();
        bool forceMode = std::find(args.begin(), args.end(), "-f") != args.end();
        bool backupMode = std::find(args.begin(), args.end(), "-b") != args.end();
        bool helpMode = std::find(args.begin(), args.end(), "-h") != args.end() ||
                        std::find(args.begin(), args.end(), "--help") != args.end();

        if (helpMode)
        {
            printRemoveHelp();
            return;
        }

        for (const auto &target : args)
        {
            if (target[0] == '-')
            {
                continue;
            }

            try
            {
                if (fs::exists(target))
                {
                    if (fs::is_directory(target) && recursiveMode)
                    {
                        std::cout << "Removed directory recursively: " << target << std::endl;
                        fs::remove_all(target);
                    }
                    else
                    {
                        if (backupMode)
                        {
                            fs::copy(target, target + ".bak", fs::copy_options::overwrite_existing);

                            std::cout << "Backup created for " << target << " as " << target + ".bak" << std::endl;
                        }

                        if (fs::is_regular_file(target) || !recursiveMode)
                        {

                            std::cout << "Removed: " << target << std::endl;

                            fs::remove(target);
                        }
                    }
                }
                else if (forceMode)
                {

                    std::cout << "Forcefully removed : " << target << std::endl;
                }
                else
                {
                    std::cout << "File not found: " << target << std::endl;
                }
            }
            catch (const fs::filesystem_error &e)
            {
                std::cout << "Error removing target: " << e.what() << std::endl;
            }
        }
    }

    void printRemoveHelp()
    {
        std::cout << "Usage: rm [OPTION]... FILE...\n"
                  << "Remove (unlink) the FILE(s).\n\n"
                  << "Options:\n"
                  << "  -r, --recursive     remove directories and their contents recursively\n"
                  << "  -f                 ignore nonexistent files and arguments, never prompt\n"
                  << "  -b                 create backups of removed files with a .bak extension\n"
                  << "  -v                 explain what is being done\n"
                  << "  -h, --help         display this help and exit\n";
    }

    void lsDirectory(const std::vector<std::string> &args)
    {
        if (std::find(args.begin(), args.end(), "--help") != args.end())
        {
            printlsDirectoryHelp();
            return;
        }
        bool recursive = std::find(args.begin(), args.end(), "-r") != args.end();
        bool showHidden = std::find(args.begin(), args.end(), "--hidden") != args.end();
        bool showSize = std::find(args.begin(), args.end(), "--size") != args.end();
        bool sortAlphabetically = std::find(args.begin(), args.end(), "--sort") != args.end();

        std::vector<fs::directory_entry> entries;

        for (const auto &entry : fs::directory_iterator(currentDirectory))
        {
            if (!showHidden && entry.path().filename().string()[0] == '.')
            {
                continue; // Skip hidden files
            }
            entries.push_back(entry);
        }

        if (sortAlphabetically)
        {
            std::sort(entries.begin(), entries.end(), [](const auto &a, const auto &b)
                      { return a.path().filename().string() < b.path().filename().string(); });
        }

        for (const auto &entry : entries)
        {
            if (showSize)
            {
                std::cout << std::setw(10) << fs::file_size(entry) << " ";
            }

            std::cout << entry.path().filename().string();

            if (fs::is_directory(entry))
            {
                std::cout << "/";
            }

            std::cout << std::endl;

            if (recursive && fs::is_directory(entry))
            {
                lsDirectoryRecursive(entry.path(), showHidden, showSize, sortAlphabetically);
            }
        }
    }

    void lsDirectoryRecursive(const fs::path &path, bool showHidden, bool showSize, bool sortAlphabetically)
    {
        for (const auto &entry : fs::directory_iterator(path))
        {
            if (!showHidden && entry.path().filename().string()[0] == '.')
            {
                continue; // Skip hidden files
            }

            if (showSize)
            {
                std::cout << std::setw(10) << fs::file_size(entry) << " ";
            }

            std::cout << entry.path().filename().string();

            if (fs::is_directory(entry))
            {
                std::cout << "/";
            }

            std::cout << std::endl;

            if (fs::is_directory(entry))
            {
                lsDirectoryRecursive(entry.path(), showHidden, showSize, sortAlphabetically);
            }
        }
    }

    void printlsDirectoryHelp()
    {
        std::cout << "ls - List files and directories in the current directory." << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -r                 List subdirectories recursively." << std::endl;
        std::cout << "  --hidden           Include hidden files and directories." << std::endl;
        std::cout << "  --size             Display file sizes." << std::endl;
        std::cout << "  --sort             Sort entries alphabetically." << std::endl;
        std::cout << "  --help             Display this help message." << std::endl;
    }

    void cpdirectory(const fs::path &source, const fs::path &destination)
    {
        fs::create_directories(destination);

        for (const auto &entry : fs::directory_iterator(source))
        {
            const fs::path currentPath = entry.path();
            const fs::path newPath = destination / currentPath.filename();

            if (fs::is_directory(currentPath))
            {
                cpdirectory(currentPath, newPath);
            }
            else
            {
                fs::copy(currentPath, newPath, fs::copy_options::overwrite_existing);
            }
        }
    }

    void copy(const std::vector<std::string> &args)
    {
        if (args.size() < 2)
        {
            std::cout << "cp command requires source and destination paths." << std::endl;
            return;
        }

        bool recursiveMode = std::find(args.begin(), args.end(), "-rt") != args.end();
        bool recursiveThreadMode = std::find(args.begin(), args.end(), "-r") != args.end();
        bool interactiveMode = std::find(args.begin(), args.end(), "-i") != args.end();
        bool backupMode = std::find(args.begin(), args.end(), "-b") != args.end();
        bool helpMode = std::find(args.begin(), args.end(), "--help") != args.end();

        if (helpMode)
        {
            displayCopyHelp();
            return;
        }

        std::string source;
        std::string destination;

        // Parse options and paths
        for (const auto &arg : args)
        {
            if (arg == "-r")
            {
                recursiveThreadMode = true;
            }
            else if (arg == "-rt")
            {
                recursiveMode = true;
            }
            else if (arg == "-i")
            {
                interactiveMode = true;
            }
            else if (arg == "-b")
            {
                backupMode = true;
            }
            else if (source.empty())
            {
                source = arg;
            }
            else
            {
                destination = arg;
            }
        }

        // Make source path absolute
        fs::path absoluteSource = fs::absolute(source);

        // Check if source exists
        if (!fs::exists(absoluteSource))
        {
            std::cout << "Source does not exist: " << absoluteSource << std::endl;
            return;
        }

        try
        {
            if (fs::is_directory(absoluteSource))
            {
                if (recursiveMode)
                {
                    std::cout<<"Threaded Recursion\n";
                    auto start = std::chrono::high_resolution_clock::now(); // Start time
                    // Manually copy the directory recursively
                    cpdirectory(absoluteSource, destination);
                    auto end = std::chrono::high_resolution_clock::now(); // End time
                    std::chrono::duration<double> duration = (end - start) * 1000;
                    std::cout << "Copy execution time: " << duration.count() << " milliseconds" << std::endl;
                }
                else if (recursiveThreadMode)
                {
                    std::cout<<"Normal Recursion\n";
                    auto start = std::chrono::high_resolution_clock::now(); // Start time
                    // Use multi-threading for recursive copy
                    std::thread copyThread(&MyShell::copyRecursiveThread, this, absoluteSource, destination);
                    copyThread.join();                                    // Wait for the thread to finish
                    auto end = std::chrono::high_resolution_clock::now(); // End time
                    std::chrono::duration<double> duration = (end - start) * 1000;
                    std::cout << "Copy execution time " << duration.count() << " milliseconds" << std::endl;
                }
                else
                {
                    // Use fs::copy for non-recursive directory copy
                    fs::copy(absoluteSource, destination, fs::copy_options::overwrite_existing);
                }
            }
            else
            {
                // Use fs::copy for file copy
                fs::copy(absoluteSource, destination, fs::copy_options::overwrite_existing);
            }

            std::cout << "Successfully copied " << absoluteSource << " to " << destination << std::endl;

            if (interactiveMode)
            {
                std::cout << "Do you want to overwrite " << destination << "? (y/n): ";
                char response;
                std::cin >> response;
                if (response != 'y' && response != 'Y')
                {
                    std::cout << "Copy canceled." << std::endl;
                    return;
                }
            }

            if (backupMode)
            {
                fs::copy(destination, destination + ".bak", fs::copy_options::overwrite_existing);
                std::cout << "Backup created for " << destination << " as " << destination + ".bak" << std::endl;
            }
        }
        catch (const fs::filesystem_error &e)
        {
            std::cout << "Error copying files: " << e.what() << std::endl;
        }
    }

    void copyRecursiveThread(const fs::path &source, const std::string &destination)
    {
        for (const auto &entry : fs::directory_iterator(source))
        {
            const fs::path currentPath = entry.path();
            const fs::path newPath = fs::path(destination) / currentPath.filename().string();

            if (fs::is_directory(currentPath))
            {
                std::thread copyThread(&MyShell::copyRecursiveThread, this, currentPath, newPath.string());
                copyThread.detach(); // Detach the thread to run asynchronously
            }
            else
            {
                fs::copy(currentPath, newPath, fs::copy_options::overwrite_existing);
            }
        }
    }

    void displayCopyHelp()
    {
        std::cout << "cp - Copy files and directories\n"
                  << "Usage: cp [OPTIONS] SOURCE DESTINATION\n\n"
                  << "Options:\n"
                  << "  -r                Copy directories recursively\n"
                  << "  -rt               Copy directories recursively with Threading\n"
                  << "  -i                Prompt before overwriting files\n"
                  << "  -b                Create a backup of the destination file\n"
                  << "  --help            Display this help message\n"
                  << std::endl;
    }
};

int main()
{
    MyShell myShell;
    myShell.run();

    return 0;
}
