#include <iostream>
#include <string>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <atomic>
#include <thread>

std::atomic_flag childExitFlag;

void ChildSignalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        childExitFlag.test_and_set();
    }
}

void ChildProcess()
{
    std::signal(SIGINT, ChildSignalHandler);
    std::signal(SIGTERM, ChildSignalHandler);

    pid_t pid = getpid();

    while (!childExitFlag.test())
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "Child process " << pid << " is still alive" << std::endl;
    }

    std::cout << "Child process " << pid << " is exiting" << std::endl;
}

void KillChildProcess(pid_t childPid)
{
    if (kill(childPid, SIGTERM) == -1)
    {
        std::cout << "Error: Failed to send SIGTERM to process " << childPid << std::endl;
        return;
    }

    int status;
    if (waitpid(childPid, &status, 0) == -1)
    {
        std::cout << "Error: Failed to wait for process " << childPid << std::endl;
        return;
    }

    std::cout << "Child process " << childPid << " terminated" << std::endl;
}

int main()
{
    std::vector<pid_t> children;

    std::cout << "Enter command (spawn, kill, exit): ";
    std::string command;
    while (true)
    {
        std::cin >> command;
        if (command == "exit")
        {
            std::cout << "Exiting program..." << std::endl;
            break;
        }
        else if (command == "spawn")
        {
            pid_t pid = fork();
            if (pid < 0)
            {
                std::cout << "Failed to fork process" << std::endl;
            }
            else if (pid == 0)
            {
                ChildProcess();
                return EXIT_SUCCESS;
            }
            else
            {
                std::cout << "Spawned child process with PID " << pid << std::endl;
                children.push_back(pid);
            }
        }
        else if (command == "kill")
        {
            if (children.empty())
            {
                std::cout << "No children to kill" << std::endl;
                continue;
            }
            pid_t childPid = children.back();
            children.pop_back();
            KillChildProcess(childPid);
        }
        else
        {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

//    std::cout << "Killing remaining child processes" << std::endl;
//    for (pid_t childPid : children)
//    {
//        KillChildProcess(childPid);
//    }

    return EXIT_SUCCESS;
}