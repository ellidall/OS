#include <iostream>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

void PrintOSVersion()
{
    utsname unameData{};
    if (uname(&unameData) == -1)
    {
        std::cout << "Error getting OS version" << std::endl;
        return;
    }
    std::cout << "OS: " << unameData.sysname << " " << unameData.release << std::endl;
}

void PrintMemoryInfo()
{
    struct sysinfo memInfo{};
    if (sysinfo(&memInfo) != 0)
    {
        std::cout << "Error getting memory info" << std::endl;
        return;
    }

    auto totalPhysMem = memInfo.totalram / (1024 * 1024);
    auto freePhysMem = memInfo.freeram / (1024 * 1024);

    std::cout << "RAM: " << freePhysMem << "MB / " << totalPhysMem << "MB" << std::endl;
}

void PrintProcessorCount()
{
    long numProcessors = sysconf(_SC_NPROCESSORS_ONLN);
    std::cout << "Processors: " << numProcessors << std::endl;
}

int main()
{
    PrintOSVersion();
    PrintMemoryInfo();
    PrintProcessorCount();

    return EXIT_SUCCESS;
}