#include <iostream>
#include <utility>
#include <vector>
#include <thread>
#include <mutex>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const int BYTE_RANGE = 256;

class HistogramBuilder
{
public:
    HistogramBuilder(std::string fileName, int numThreads)
            : m_fileName(std::move(fileName)), m_numThreads(numThreads), m_globalHistogram(BYTE_RANGE, 0)
    {}

    bool Build()
    {
        if (!OpenFile()) return false;
        if (!MapFile()) return false;

        ProcessFile();
        UnmapFile();
        CloseFile();

        return true;
    }

    void PrintHistogram() const
    {
        for (int i = 0; i < BYTE_RANGE; ++i)
        {
            if (std::isprint(i))
            {
                std::cout << "'" << static_cast<char>(i) << "'   | ";
            }
            else
            {
                std::cout << "0x" << std::hex << i << std::dec << "  | ";
            }
            std::cout << m_globalHistogram[i] << std::endl;
        }
    }

private:
    std::string m_fileName;
    int m_numThreads;
    int m_fileDescriptor = -1;
    size_t m_fileSize = 0;
    char* m_mappedData = nullptr;
    std::vector<int> m_globalHistogram;
    std::mutex m_mutex;

    bool OpenFile()
    {
        m_fileDescriptor = open(m_fileName.c_str(), O_RDONLY);
        if (m_fileDescriptor == -1)
        {
            perror("open");
            return false;
        }

        struct stat sb;
        if (fstat(m_fileDescriptor, &sb) == -1)
        {
            perror("fstat");
            close(m_fileDescriptor);
            return false;
        }
        m_fileSize = sb.st_size;
        return true;
    }

    bool MapFile()
    {
        m_mappedData = static_cast<char*>(mmap(nullptr, m_fileSize, PROT_READ, MAP_PRIVATE, m_fileDescriptor, 0));
        if (m_mappedData == MAP_FAILED)
        {
            perror("mmap");
            CloseFile();
            return false;
        }
        return true;
    }

    void UnmapFile()
    {
        if (munmap(m_mappedData, m_fileSize) == -1)
        {
            perror("munmap");
        }
    }

    void CloseFile()
    {
        if (m_fileDescriptor != -1)
        {
            close(m_fileDescriptor);
        }
    }

    void CountFrequencies(size_t start, size_t end, std::vector<int>& localHistogram)
    {
        for (size_t i = start; i < end; ++i)
        {
            unsigned char byte = static_cast<unsigned char>(m_mappedData[i]);
            localHistogram[byte]++;
        }
    }

    void MergeHistograms(const std::vector<int>& localHistogram)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (int i = 0; i < BYTE_RANGE; ++i)
        {
            m_globalHistogram[i] += localHistogram[i];
        }
    }

    void ProcessFile()
    {
        size_t chunkSize = m_fileSize / m_numThreads;
        std::vector<std::thread> threads;

        for (int i = 0; i < m_numThreads; ++i)
        {
            size_t start = i * chunkSize;
            size_t end = (i == m_numThreads - 1) ? m_fileSize : start + chunkSize;

            threads.emplace_back([this, start, end]() {
                std::vector<int> localHistogram(BYTE_RANGE, 0);
                CountFrequencies(start, end, localHistogram);
                MergeHistograms(localHistogram);
            });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }
};

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: task6_2 FILE_NAME NUM_THREADS\n";
        return EXIT_FAILURE;
    }

    std::string fileName = argv[1];
    int numThreads = std::stoi(argv[2]);

    HistogramBuilder builder(fileName, numThreads);

    if (builder.Build())
    {
        builder.PrintHistogram();
    }
    else
    {
        std::cerr << "Error: Could not build histogram\n";
        return 1;
    }

    return 0;
}
