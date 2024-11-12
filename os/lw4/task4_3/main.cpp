#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>
#include <cassert>
#include <string>
#include <filesystem>
#include <iterator>
#include <functional>

const std::string COMMAND_GENERATE = "generate";
const std::string COMMAND_SORT = "sort";
const std::string COMMAND_HELP = "--help";
const std::string COMMAND_HELP_SHORT = "-h";

using Numbers = std::vector<int64_t>;

void ShowHelp()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "task4_3 generate OUTPUT_FILE SIZE" << std::endl;
    std::cout << "task4_3 sort INPUT_FILE OUTPUT_FILE NUM_THREADS" << std::endl;
}

void GenerateFile(const std::string& outputFile, size_t size)
{
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) throw std::ios_base::failure("Can't open file " + outputFile);
    std::mt19937_64 gen(std::random_device{}());
    std::uniform_int_distribution<int64_t> dist(INT64_MIN, INT64_MAX);
    std::generate_n(
            std::ostream_iterator<int64_t>(outFile, "\n"),
            size,
            std::bind_front(dist, gen)
    );
}

// подумать почему прирост не кратен увеличению количеству потоков
void ParallelMergeSort(Numbers& numbers, size_t left, size_t right, unsigned threads)
{
    if (left >= right) return;

    auto mid = static_cast<int64_t>(left + (right - left) / 2);

    if (threads >= 2)
    {
        std::jthread leftThread(ParallelMergeSort, std::ref(numbers), left, mid, threads / 2);
        ParallelMergeSort(numbers, mid + 1, right, threads - threads / 2);
        leftThread.join();
    }
    else
    {
        std::sort(numbers.begin() + static_cast<int64_t>(left), numbers.begin() + static_cast<int64_t>(right) + 1);
        return;
    }

    std::inplace_merge(
            numbers.begin() + static_cast<int64_t>(left),
            numbers.begin() + mid + 1,
            numbers.begin() + static_cast<int64_t>(right) + 1
    );
}

Numbers ReadNumbers(const std::string& fileName)
{
    std::ifstream inFile(fileName);
    if (!inFile.is_open()) throw std::ios_base::failure("Can't open file " + fileName);
    std::istream_iterator<int64_t> begin(inFile), end;
    if (inFile.fail() || inFile.bad()) throw std::runtime_error("Failed to read number");

    Numbers numbers;
    numbers.reserve(static_cast<size_t>(std::filesystem::file_size(fileName) / 18));
    numbers.assign(begin, end);

    return numbers;
}

void WriteNumbers(const Numbers& numbers, const std::string& fileName)
{
    std::ofstream outFile(fileName);
    if (!outFile.is_open()) throw std::ios_base::failure("Can't open file " + fileName);
    std::ranges::copy(numbers, std::ostream_iterator<int64_t>(outFile, "\n"));
}

void SortFile(const std::string& inputFileName, const std::string& outputFileName, unsigned numThreads)
{
    numThreads = numThreads != 0 ? numThreads : std::thread::hardware_concurrency();
    Numbers numbers = ReadNumbers(inputFileName);

    auto startTime = std::chrono::high_resolution_clock::now();
    ParallelMergeSort(numbers, 0, static_cast<int64_t>(numbers.size()) - 1, numThreads);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Sorting with " << numThreads << " threads took " << elapsed.count() << " seconds" << std::endl;

    assert(std::is_sorted(numbers.begin(), numbers.end()));

    WriteNumbers(numbers, outputFileName);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Error: wrong number of parameters" << std::endl;
        ShowHelp();
        return EXIT_FAILURE;
    }

    try
    {
        std::string command = argv[1];
        if (command == COMMAND_GENERATE && argc == 4)
            GenerateFile(argv[2], std::stoull(argv[3]));
        else if (command == COMMAND_SORT && argc == 5)
            SortFile(argv[2], argv[3], [argv] {
                int n = std::stoi(argv[4]);
                return (n >= 0)
                       ? static_cast<unsigned>(n)
                       : throw std::runtime_error("Number of threads must not be negative");
            }());
        else if (command == COMMAND_HELP || command == COMMAND_HELP_SHORT)
            ShowHelp();
        else
        {
            ShowHelp();
            return EXIT_FAILURE;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}