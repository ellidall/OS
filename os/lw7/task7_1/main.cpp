#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

fs::path GetStartDirName(int argc, char* argv[])
{
    if (argc == 1)
    {
        return fs::current_path();
    }
    else if (argc == 2)
    {
        return static_cast<fs::path>(argv[1]);
    }
    throw std::invalid_argument("Usage <programName> <dirName>");
}

struct FileEntry
{
    std::string name;
    bool isDirectory;
};

std::vector<FileEntry> GetEntriesByPath(const fs::path& path)
{
    std::vector<FileEntry> entries;
    for (const auto& entry : fs::directory_iterator(path))
    {
        entries.push_back({entry.path().filename().string(), entry.is_directory()});
    }

    std::ranges::sort(entries, [](const FileEntry& a, const FileEntry& b) {
        if (a.isDirectory != b.isDirectory)
        {
            return b.isDirectory;
        }
        return a.name < b.name;
    });

    return entries;
}

void AddGap(
        const std::vector<FileEntry>& entries, const size_t currentIndex,
        const fs::path& path, const std::string& prefix
)
{
    if (currentIndex > 0)
    {
        bool isPrevDirEmpty = entries[currentIndex - 1].isDirectory
                              && (fs::directory_iterator(path / fs::path(entries[currentIndex - 1].name)) !=
                                  fs::directory_iterator());

        if (entries[currentIndex].isDirectory && (!entries[currentIndex - 1].isDirectory || isPrevDirEmpty))
        {
            std::cout << prefix << "|   " << std::endl;
        }
    }
}

void PrintFileTree(const fs::path& path, const std::string& prefix = "")
{
    if (!fs::exists(path) || !fs::is_directory(path))
    {
        throw std::invalid_argument("Invalid path: " + static_cast<std::string>(path));
    }

    std::vector<FileEntry> entries = GetEntriesByPath(path);

    for (size_t i = 0; i < entries.size(); ++i)
    {
        const auto& entry = entries[i];
        bool isLast = (i == entries.size() - 1);

        AddGap(entries, i, path, prefix);

        std::string currentPrefix = prefix + (entry.isDirectory ? (isLast ? "\\---" : "+---") : "|   ");
        std::cout << currentPrefix << (entry.isDirectory ? "[D] " : "[F] ") << entry.name << std::endl;

        if (entry.isDirectory)
        {
            PrintFileTree(path / entry.name, prefix + (isLast ? "    " : "|   "));
        }
    }
}

int main(int argc, char* argv[])
{
    try
    {
        fs::path startDirectory = GetStartDirName(argc, argv);
        std::cout << startDirectory.filename().string() << std::endl;
        PrintFileTree(startDirectory);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}