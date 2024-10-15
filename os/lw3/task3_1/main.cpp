#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include <filesystem>
#include <functional>

std::atomic_flag exitRequested;

void SignalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        std::cout << "Process " << getpid() << " has received signal #" << signal << std::endl;
        exitRequested.test_and_set();
    }
}

class FileWriter
{
    std::ofstream outfile;
    std::string filename;
public:
    FileWriter(std::string const& name) : outfile(name), filename(name)
    {
        if (!outfile.is_open())
        {
            throw std::ios_base::failure("Failed to create file");
        }
    }

    void addData(std::function<void(std::ofstream & out)> && job)
    {
        job(outfile);
    }

    ~FileWriter()
    {
        outfile.close();
        if (std::filesystem::exists(filename))
        {
            std::filesystem::remove(filename);
        }
    }
};

int main()
{
    try
    {
        if ((std::signal(SIGINT, SignalHandler) == SIG_ERR)
            || (std::signal(SIGTERM, SignalHandler) == SIG_ERR))
        {
            std::cout << "Error setting signal handler" << std::endl;
            return EXIT_SUCCESS;
        }

        FileWriter writer("temp.txt");

        for (int i = 0; i <= 100 && !exitRequested.test(); ++i)
        {
            writer.addData([=](auto & out) {
                out << i << std::endl;
            });
            std::cout << i << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    std::cout << "Done" << std::endl;
    return EXIT_SUCCESS;
}