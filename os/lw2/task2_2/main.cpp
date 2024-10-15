#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <iomanip>

const std::string MAIN_DIR = "/proc";
const char DIR_SEPARATOR = '/';

bool IsNumeric(const char* name)
{
    return std::string(name).find_first_not_of("0123456789") == std::string::npos;
}

class DirHandle
{
public:
    explicit DirHandle(const char* path)
    {
        m_dir = opendir(path);
    }

    ~DirHandle()
    {
        if (m_dir)
        {
            closedir(m_dir);
        }
    }

    DirHandle(DirHandle&& other) noexcept : m_dir(other.m_dir)
    {
        other.m_dir = nullptr;
    }

    DirHandle& operator=(DirHandle&& other) noexcept
    {
        if (this != &other)
        {
            if (m_dir)
            {
                closedir(m_dir);
            }

            m_dir = other.m_dir;
            other.m_dir = nullptr;
        }
        return *this;
    }

    DirHandle(const DirHandle& other) = delete;

    DirHandle& operator=(const DirHandle&) = delete;

    [[nodiscard]] DIR* get() const
    {
        return m_dir;
    }

private:
    DIR* m_dir;
};

void GetProcessInfo(const std::string& pid)
{
    std::string path = MAIN_DIR + DIR_SEPARATOR + pid + "/status";
    std::ifstream statusFile(path);

    if (!statusFile.is_open())
    {
        return;
    }

    std::string line;
    std::string name;
    long memory;

    while (std::getline(statusFile, line))
    {
        if (line.find("Name:") == 0)
        {
            name = line.substr(line.find('\t') + 1);
        }
        if (line.find("VmSize:") == 0)
        {
            memory = std::stol(line.substr(line.find('\t') + 1));
        }
    }

    statusFile.close();

    std::cout << pid << "\t"
              << std::setw(30) << std::left << name << "\t"
              << std::fixed << std::setprecision(2) << memory / 1024 << " MB" << std::endl;
}

int main()
{
    try
    {
        DirHandle dir(MAIN_DIR.c_str());
        dirent* entry;

        while (dir.get() != nullptr && (entry = readdir(dir.get())) != nullptr)
        {
            struct stat entryStat{};
            std::string path = MAIN_DIR + DIR_SEPARATOR + std::string(entry->d_name);

            if (stat(path.c_str(), &entryStat) == 0
                && S_ISDIR(entryStat.st_mode)
                && IsNumeric(entry->d_name))
            {
                GetProcessInfo(entry->d_name);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}