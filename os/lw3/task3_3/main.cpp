#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <system_error>
#include <span>
#include <algorithm>
#include <sstream>

class Pipe
{
public:
    // запретить конструкторы и присвоение
    Pipe()
    {
        if (pipe(fds) == -1)
        {
            throw std::system_error(errno, std::generic_category(), "Failed to create pipe");
        }
    }

    ~Pipe()
    {
        close(fds[0]);
        close(fds[1]);
    }

    [[nodiscard]] int GetReadFD() const
    {
        return fds[0];
    }

    [[nodiscard]] int GetWriteFD() const
    {
        return fds[1];
    }

private:
    int fds[2]{};
};

int ProcessCommandAdd(const std::vector<int>& numbers)
{
    int sum = 0;
    for (int num : numbers)
    {
        sum += num;
    }
    return sum;
}

std::string ProcessCommandLongestWord(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }

    std::string word, longestWord;
    while (file >> word)
    {
        if (word.length() > longestWord.length())
        {
            longestWord = word;
        }
    }
    return longestWord;
}

// парсинг должен находиться в главном процессе, а дочерний должен обрабавтывать
void ChildProcess(int readFD, int writeFD)
{
    char buffer[1024];
    while (true)
    {
        ssize_t byteCount = read(readFD, buffer, sizeof(buffer));
        if (byteCount == -1)
        {
            throw std::system_error(errno, std::generic_category(), "Failed to read from pipe");
        }

        buffer[byteCount] = '\0';
        std::string command(buffer);
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "exit")
        {
            break;
        }
        else if (cmd == "add")
        {
            std::vector<int> numbers;
            int num;
            while (iss >> num)
            {
                numbers.push_back(num);
            }
            int result = ProcessCommandAdd(numbers);

            if (write(writeFD, &result, sizeof(result)) == -1)
            {
                throw std::system_error(errno, std::generic_category(), "Failed to write to pipe");
            }
        }
        else if (cmd == "longest_word")
        {
            std::string filename;
            iss >> filename;
            std::string result = ProcessCommandLongestWord(filename);

            if (write(writeFD, result.c_str(), result.size() + 1) == -1)
            {
                throw std::system_error(errno, std::generic_category(), "Failed to write to pipe");
            }
        }
    }
}

int main()
{
    // расзбить функцию напоменьше
    try
    {
        Pipe parentToChild, childToParent;

        pid_t pid = fork();
        if (pid == -1)
        {
            throw std::system_error(errno, std::generic_category(), "Failed to fork process");
        }
        else if (pid == 0)
        {
            close(parentToChild.GetWriteFD());
            close(childToParent.GetReadFD());

            ChildProcess(parentToChild.GetReadFD(), childToParent.GetWriteFD());
            return EXIT_SUCCESS;
        }
        else
        {
            // файловый десткриптор может быть занят другими процессами, поэтому закрытие лучше делать через обёртку
            close(parentToChild.GetReadFD());
            close(childToParent.GetWriteFD());

            std::string command;
            while (true)
            {
                std::getline(std::cin, command);
                std::istringstream iss(command);
                std::string cmd;
                iss >> cmd;

                if (write(parentToChild.GetWriteFD(), command.c_str(), command.size() + 1) == -1)
                {
                    throw std::system_error(errno, std::generic_category(), "Failed to write to pipe");
                }

                if (cmd == "exit")
                {
                    break;
                }
                else if (cmd.find("add") == 0)
                {
                    int result;
                    if (read(childToParent.GetReadFD(), &result, sizeof(result)) == -1)
                    {
                        throw std::system_error(errno, std::generic_category(), "Failed to read from pipe");
                    }
                    std::cout << "Sum is " << result << std::endl;
                }
                else if (cmd.find("longest_word") == 0)
                {
                    char result[100];
                    ssize_t byteCount = read(childToParent.GetReadFD(), result, sizeof(result));
                    if (byteCount == -1)
                    {
                        throw std::system_error(errno, std::generic_category(), "Failed to read from pipe");
                    }
                    result[byteCount] = '\0';
                    std::cout << "longest word is " << result << std::endl;
                }
            }

            wait(nullptr);
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}