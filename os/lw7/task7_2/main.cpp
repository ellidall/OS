#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <sys/inotify.h>
#include <unistd.h>
#include <filesystem>
#include <climits>

namespace fs = std::filesystem;

class DescriptorHandler
{
public:
    explicit DescriptorHandler(int fd) : fd(fd)
    {
        if (fd < 0 && fd != -1)
        {
            throw std::runtime_error("Failed to create file descriptor");
        }
    }

    ~DescriptorHandler()
    {
        Close();
    }

    DescriptorHandler(const DescriptorHandler&) = delete;
    DescriptorHandler& operator=(const DescriptorHandler&) = delete;
    DescriptorHandler(DescriptorHandler&& other) noexcept: fd(other.fd)
    {
        other.fd = -1;
    }
    DescriptorHandler& operator=(DescriptorHandler&& other) noexcept
    {
        if (this != &other)
        {
            Close();
            fd = std::exchange(other.fd, -1);
        }
        return *this;
    }

    void Close()
    {
        if (fd >= 0)
        {
            close(fd);
        }
    }

    [[nodiscard]] int Get() const
    {
        return fd;
    }

private:
    int fd;
};

std::string GetPath(int argc, char* argv[])
{
    if (argc != 2)
    {
        throw std::invalid_argument("Usage <programName> <dirName>");
    }
    return argv[1];
}

void CheckDirExist(const std::string& path)
{
    if (!fs::exists(path))
    {
        throw std::invalid_argument("Invalid path: " + path + " does not exist");
    }
}

void CheckAddWatchSuccess(const int watchDescriptor)
{
    if (watchDescriptor < 0)
    {
        throw std::runtime_error("Failed to add watch");
    }
}

void Print(inotify_event* event)
{
    std::cout << "File: " << (event->len ? event->name : "Untraceable") << std::endl;

    if (event->mask & IN_CREATE)
    {
        std::cout << "Event: Created" << std::endl;
    }
    if (event->mask & IN_DELETE)
    {
        std::cout << "Event: Deleted" << std::endl;
    }
    if (event->mask & IN_MODIFY)
    {
        std::cout << "Event: Modified" << std::endl;
    }
    if (event->mask & IN_ATTRIB)
    {
        std::cout << "Event: Attributes changed" << std::endl;
    }
}

void MonitorFileSystem(const std::string& path)
{
    CheckDirExist(path);
    DescriptorHandler inotifyFd(inotify_init());
    int watchDescriptor = inotify_add_watch(inotifyFd.Get(), path.c_str(),
            IN_CREATE | IN_DELETE | IN_MODIFY | IN_ATTRIB);
    CheckAddWatchSuccess(watchDescriptor);
    std::cout << "Monitoring changes in: " << path << std::endl;

    const size_t bufferSize = 1024 * (sizeof(struct inotify_event) + NAME_MAX + 1);
    // указатель внутри буфера выровнять
    alignas(struct inotify_event) std::vector<char> buffer(bufferSize);

    while (true)
    {
        long bytesRead = read(inotifyFd.Get(), buffer.data(), bufferSize);
        if (bytesRead < 0)
        {
            throw std::runtime_error("Error reading inotify events");
        }

        for (int i = 0; i < bytesRead;)
        {
            auto* event = reinterpret_cast<struct inotify_event*>(&buffer[i]);
            Print(event);
            i += sizeof(struct inotify_event) + event->len;
        }
    }
}

int main(int argc, char* argv[])
{
    try
    {
        std::string path = GetPath(argc, argv);
        MonitorFileSystem(path);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
