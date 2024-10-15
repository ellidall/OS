#include "SysInfo.h"

#ifdef _WIN32
#include "WinSysInfo.h"
#endif

#ifdef linux
#include "LinuxSysInfo.h"
#endif

std::string SysInfo::GetOSName()
{
    return GetOSNameImpl();
}

std::string SysInfo::GetOSVersion()
{
    return GetOSVersionImpl();
}

uint64_t SysInfo::GetFreeMemory()
{
    return GetFreeMemoryImpl();
}

uint64_t SysInfo::GetTotalMemory()
{
    return GetTotalMemoryImpl();
}

unsigned SysInfo::GetProcessorCount()
{
    return GetProcessorCountImpl();
}