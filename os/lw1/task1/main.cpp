#include <windows.h>
#include <iostream>
#include <sysinfoapi.h>
#include <versionhelpers.h>

void PrintOSVersion()
{
    if (IsWindows10OrGreater())
    {
        std::cout << "OS: Windows 10 or Greater" << std::endl;
    }
    else if (IsWindows8OrGreater())
    {
        std::cout << "OS: Windows 8 or Greater" << std::endl;
    }
    else if (IsWindows7OrGreater())
    {
        std::cout << "OS: Windows 7 or Greater" << std::endl;
    }
    else
    {
        std::cout << "OS: Old version of Windows" << std::endl;
    }
}

void PrintMemoryInfo()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    if (!GlobalMemoryStatusEx(&memInfo))
    {
        std::cout << "Error getting memory info" << std::endl;
        return;
    }

    auto totalPhysMem = memInfo.ullTotalPhys / (1024 * 1024);
    auto freePhysMem = memInfo.ullAvailPhys / (1024 * 1024);

    std::cout << "RAM: " << freePhysMem << "MB / " << totalPhysMem << "MB" << std::endl;
}

void PrintProcessorCount()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    std::cout << "Processors: " << sysInfo.dwNumberOfProcessors << std::endl;
}

int main()
{
    PrintOSVersion();
    PrintMemoryInfo();
    PrintProcessorCount();

    return EXIT_SUCCESS;
}