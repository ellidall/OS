#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include <filesystem>

std::atomic_flag exitRequested;

void SignalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        std::cout << "Process " << getpid() << " has received signal #" << signal << std::endl;
        exitRequested.test_and_set();
    }
}

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

        std::string filename = "temp.txt";
        std::ofstream outfile(filename);
        if (!outfile.is_open())
        {
            throw std::ios_base::failure("Failed to create file");
        }

        for (int i = 0; i <= 100 && !exitRequested.test(); ++i)
        {
            outfile << i << std::endl;
            std::cout << i << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        outfile.close();
        if (std::filesystem::exists(filename))
        {
            std::filesystem::remove(filename);
        }
        std::cout << "Done" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}