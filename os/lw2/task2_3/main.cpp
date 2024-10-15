#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        std::cout << "Error creating child process" << std::endl;
        return EXIT_FAILURE;
    }
    else if (pid == 0)
    {
//        std::cout << "Child Process PID: " << getpid() << std::endl;
    }
    else
    {
        int status;
        std::string waitPid;

        std::cout << "Enter the PID of the child process:" << std::endl;
        while (true)
        {
            std::getline(std::cin, waitPid);
            if (waitPid.find_first_not_of("0123456789") == std::string::npos)
            {
                auto pidToWait = static_cast<pid_t>(std::stoi(waitPid));
                pid_t waitResult = waitpid(pidToWait, &status, 0);

                if (waitResult == -1)
                {
                    std::cout << "Error waiting for child process with PID: " << pidToWait << ". Try again"
                              << std::endl;
                    continue;
                }
                if (WIFEXITED(status))
                {
                    std::cout << "Child process with PID " << pidToWait << " exited with status: "
                              << WEXITSTATUS(status) << std::endl;
                }
                else
                {
                    std::cout << "Child process with PID " << pidToWait << " did not exit normally" << std::endl;
                }

                break;
            }
            else
            {
                std::cout << "Error: PID must be a positive number. Try again" << std::endl;
            }
        }
    }

    return EXIT_SUCCESS;
}