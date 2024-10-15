#include <iostream>
#include <fstream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

char IsUpper(unsigned char ch)
{
    return static_cast<char>(std::isupper(ch));
}
// сделать красиво
void FlipCase(const std::string &inputFile, const std::string &outputFile)
{
    std::ifstream inFile(inputFile);
    std::ofstream outFile(outputFile);

    if (!inFile.is_open())
    {
        throw std::ios_base::failure("Error opening file: " + inputFile);
    }
    if (!outFile.is_open())
    {
        throw std::ios_base::failure("Error opening file: " + outputFile);
    }

    char ch;
    while (inFile.get(ch))
    {
       auto castedChar = static_cast<unsigned char>(ch);
        if (isupper(castedChar))
        {
            outFile.put(static_cast<char>(tolower(castedChar)));
        }
        else if (islower(castedChar))
        {
            outFile.put(static_cast<char>(toupper(castedChar)));
        }
        else
        {
            outFile.put(ch);
        }
        if (!outFile)
        {
            throw std::runtime_error("Error writing to output file: " + outputFile);
        }
    }

    if (inFile.bad())
    {
        throw std::runtime_error("Error reading input file: " + inputFile);
    }

    inFile.close();
    outFile.flush();
}

int ChildProcess(const std::string& inputFileName)
{
    try
    {
        std::string outputFileName = inputFileName + ".out";

        std::cout << "Process " << getpid() << " is processing " << inputFileName << std::endl;
        FlipCase(inputFileName, outputFileName);
        std::cout << "Process " << getpid() << " has finished writing to " << outputFileName << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <file1> <file2> ..." << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<pid_t> vec;

    for (int i = 1; i < argc; ++i)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            std::cout << "Fork failed!" << std::endl;
            return EXIT_FAILURE;
        }

        vec.push_back(pid);
        if (pid == 0)
        {
            return ChildProcess(argv[i]);
        }
    }

    while (!vec.empty())
    {
        int status;
        pid_t finishedPid = waitpid(-1, &status, 0);
        std::erase(vec, finishedPid);
        if (finishedPid > 0)
        {
            std::cout << "Child process " << finishedPid << " is over" << std::endl;
        }
    }

    return EXIT_SUCCESS;
}