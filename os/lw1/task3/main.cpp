#include <iostream>
#include "SysInfo.h"

int main() {
    try
    {
        std::cout << "OS Name: " << SysInfo::GetOSName() << std::endl;
        std::cout << "OS Version: " << SysInfo::GetOSVersion() << std::endl;
        std::cout << "Free Memory: " << SysInfo::GetFreeMemory() << " MB" << std::endl;
        std::cout << "Total Memory: " << SysInfo::GetTotalMemory() << " MB" << std::endl;
        std::cout << "Processor Count: " << SysInfo::GetProcessorCount() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}