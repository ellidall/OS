#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <thread>

bool IsUpper(char ch)
{
    return std::isupper(static_cast<unsigned char>(ch));
}

bool IsLower(char ch)
{
    return std::islower(static_cast<unsigned char>(ch));
}

char ToUpper(char ch)
{
    return static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
}

char ToLower(char ch)
{
    return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

void FlipCase(const std::string& inputFilename)
{
    try
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::string outputFilename = inputFilename + ".out";
        std::ifstream inputFile(inputFilename);
        if (!inputFile.is_open()) throw std::runtime_error("Failed to open file: " + inputFilename);
        std::ofstream outputFile(outputFilename);

        char c;
        while (inputFile.get(c))
        {
            if (IsUpper(c)) outputFile.put(ToLower(c));
            else if (IsLower(c)) outputFile.put(ToUpper(c));
            else outputFile.put(c);

            if (!outputFile) throw std::runtime_error("Error writing to output file: " + outputFilename);
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: task4_1 <file1> <file2> ..." << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<std::jthread> threads;
    for (int i = 1; i < argc; i++)
    {
        // почему работает программа несмотря на точ то временный объект разрушается после вызова
        threads.emplace_back(FlipCase, argv[i]);
    }

    return EXIT_SUCCESS;
}