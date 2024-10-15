#include <string>
#include <cstdint>

class SysInfo
{
public:
    [[nodiscard]] static std::string GetOSName();
    [[nodiscard]] static std::string GetOSVersion();
    [[nodiscard]] static uint64_t GetFreeMemory();
    [[nodiscard]] static uint64_t GetTotalMemory();
    [[nodiscard]] static unsigned GetProcessorCount();
};
