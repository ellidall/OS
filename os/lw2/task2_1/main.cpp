#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <iomanip>

const char UNKNOWN_PROCESS[MAX_PATH] = "<unknown>";

double bytesToMB(SIZE_T bytes)
{
    return static_cast<double>(bytes) / (1024 * 1024);
}

class HandleWrapper
{
public:
    explicit HandleWrapper(DWORD processID)
    {
        m_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    }

    ~HandleWrapper()
    {
        if (m_handle)
        {
            CloseHandle(m_handle);
        }
    }

    HandleWrapper(HandleWrapper&& other) noexcept: m_handle(other.m_handle)
    {
        other.m_handle = nullptr;
    }

    HandleWrapper& operator=(HandleWrapper&& other) noexcept
    {
        if (this != &other)
        {
            if (m_handle)
            {
                CloseHandle(m_handle);
            }
            m_handle = other.m_handle;
            other.m_handle = nullptr;
        }
        return *this;
    }

    HandleWrapper(const HandleWrapper& other) = delete;

    HandleWrapper& operator=(const HandleWrapper& other) = delete;

    [[nodiscard]] HANDLE get() const noexcept
    {
        return m_handle;
    }

private:
    HANDLE m_handle;
};

std::string GetProcessName(DWORD processID)
{
    char processName[MAX_PATH];
    strncpy(processName, UNKNOWN_PROCESS, sizeof(processName));

    HandleWrapper hProcess(processID);
    if (hProcess.get())
    {
        // выяснить нужно ли закоывать
        HMODULE hMod;
        DWORD cbNeeded;
        if (EnumProcessModules(hProcess.get(), &hMod, sizeof(hMod), &cbNeeded))
        {
            // использовать GetProcessImageFileNme
            GetModuleFileNameEx(hProcess.get(), hMod, processName, sizeof(processName) / sizeof(char));
        }
    }

    std::string fullPath(processName);
    size_t lastSlashPos = fullPath.find_last_of("\\/");
    if (lastSlashPos != std::string::npos)
    {
        return fullPath.substr(lastSlashPos + 1);
    }

    return fullPath;
}

SIZE_T GetProcessMemoryUsage(DWORD processID)
{
    SIZE_T memUsage = 0;

    HandleWrapper hProcess(processID);
    if (hProcess.get())
    {
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(hProcess.get(), &pmc, sizeof(pmc)))
        {
            memUsage = pmc.WorkingSetSize;
        }
    }

    return memUsage;
}

int main()
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;

    // для большего числа процессов
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        std::cout << "Ошибка при получении списка процессов" << std::endl;
        return EXIT_FAILURE;
    }

    cProcesses = cbNeeded / sizeof(DWORD);
    for (unsigned int i = 0; i < cProcesses; i++)
    {
        DWORD processID = aProcesses[i];
        std::string processName = GetProcessName(processID);
        if (processName == UNKNOWN_PROCESS) continue;
        SIZE_T memoryUsage = GetProcessMemoryUsage(processID);

        std::cout << processID << "\t"
                  << std::setw(50) << std::left << processName
                  << std::fixed << std::setprecision(2) << bytesToMB(memoryUsage) << " MB"
                  << std::endl;
    }

    return EXIT_SUCCESS;
}