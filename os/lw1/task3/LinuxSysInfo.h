#include <iostream>
#include <cstdint>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

std::string GetOSNameImpl()
{
    return "Linux";
}

std::string GetOSVersionImpl()
{
    struct utsname unameData{};
    if (uname(&unameData) == -1)
    {
        throw std::runtime_error("Error getting OS version");
    }
    return unameData.release;
}

uint64_t GetFreeMemoryImpl()
{
    struct sysinfo memInfo{};
    if (sysinfo(&memInfo) != 0)
    {
        throw std::runtime_error("Error getting memory info");
    }
    return memInfo.freeram / (1024 * 1024);
}

uint64_t GetTotalMemoryImpl()
{
    struct sysinfo memInfo{};
    if (sysinfo(&memInfo) != 0)
    {
        throw std::runtime_error("Error getting memory info");
    }
    return memInfo.totalram / (1024 * 1024);
}

unsigned GetProcessorCountImpl()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}
