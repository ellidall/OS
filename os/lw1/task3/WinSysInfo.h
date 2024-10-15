#include <iostream>
#include <cstdint>
#include <windows.h>
#include <sysinfoapi.h>
#include <versionhelpers.h>

std::string GetOSNameImpl()
{
    return "Windows";
}

std::string GetOSVersionImpl()
{
    std::string result;
    if (IsWindows10OrGreater())
    {
        result = "Windows 10 or Greater";
    }
    else if (IsWindows8OrGreater())
    {
        result = "Windows 8 or Greater";
    }
    else if (IsWindows7OrGreater())
    {
        result = "Windows 7 or Greater";
    }
    else
    {
        std::cout << "Old version of Windows" << std::endl;
    }
    return result;
}

uint64_t GetFreeMemoryImpl()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (!GlobalMemoryStatusEx(&memInfo))
    {
        throw std::runtime_error("Error getting memory info");
    }
    return memInfo.ullAvailPhys / (1024 * 1024);
}

uint64_t GetTotalMemoryImpl()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (!GlobalMemoryStatusEx(&memInfo))
    {
        throw std::runtime_error("Error getting memory info");
    }
    return memInfo.ullTotalPhys / (1024 * 1024);
}

unsigned GetProcessorCountImpl()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
}