#include <iostream>
#include <unordered_set>
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <algorithm>
#include <fstream>

template<typename Callable, typename... Args>
auto MeasureFuncExecutionTime(Callable callback, Args... args) -> decltype(callback(args...))
{
    auto begin = std::chrono::steady_clock::now();
    auto result = callback(args...);
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "Callback execution time: " << elapsed.count() << " mс" << std::endl;
    return result;
}

template<typename T, typename H = std::hash<T>, typename Comp = std::equal_to<T>>
class ThreadsafeSet
{
public:
    ThreadsafeSet() = default;
    ThreadsafeSet(const ThreadsafeSet&) = delete;
    ThreadsafeSet(ThreadsafeSet&&) = delete;
    ThreadsafeSet& operator=(const ThreadsafeSet&) = delete;
    ThreadsafeSet& operator=(ThreadsafeSet&&) = delete;

    void insert(const T& value)
    {
        std::unique_lock lock(m_mutex);
        m_set.insert(value);
    }

    bool contains(const T& value) const
    {
        std::shared_lock lock(m_mutex);
        return m_set.find(value) != m_set.end();
    }

    std::unordered_set<T, H, Comp> get() const
    {
        std::shared_lock lock(m_mutex);
        return m_set;
    }

private:
    std::unordered_set<T, H, Comp> m_set;
    mutable std::shared_mutex m_mutex;
};

bool IsPrime(uint64_t number)
{
    if (number < 2) return false;
    for (uint64_t i = 2; i * i <= number; i++)
    {
        if (number % i == 0)
        {
            return false;
        }
    }
    return true;
}

void FindPrimes(uint64_t start, uint64_t end, ThreadsafeSet<uint64_t>& primeSet)
{
    for (uint64_t i = start; i <= end; i++)
    {
        if (IsPrime(i))
        {
            primeSet.insert(i);
        }
    }
}

std::unordered_set<uint64_t> FindPrimesInParallel(uint64_t limit, unsigned threadCount)
{
    ThreadsafeSet<uint64_t> primeSet;
    std::vector<std::jthread> threads;
    if (threadCount == 0)
    {
        throw std::invalid_argument("ThreadCount can't be 0");
    }
    uint64_t range = limit / threadCount;

    for (int i = 0; i < threadCount; ++i)
    {
        uint64_t start = i * range;
        uint64_t end = (i == threadCount - 1) ? limit : (i + 1) * range - 1;
        threads.emplace_back(FindPrimes, start, end, std::ref(primeSet));
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
    return primeSet.get();
}

void WriteSetWithSorting(const std::string& path, std::unordered_set<uint64_t>& set)
{
    std::vector<uint64_t> sortedPrimes(set.begin(), set.end());
    std::ranges::sort(sortedPrimes);

    std::ofstream outFile(path);
    if (!outFile.is_open())
    {
        throw std::ios_base::failure("Error open file " + path);
    }
    std::copy(sortedPrimes.begin(), sortedPrimes.end(), std::ostream_iterator<uint64_t>(outFile, "\n"));
    std::cout << "Sorted primes written to " << path << std::endl;
}

int main()
{
    uint64_t limit;
    unsigned treadCount;
    std::cout << "Enter upper limit for prime search:" << std::endl << ">";
    if (!(std::cin >> limit))
    {
        std::cerr << "Limit must be a positive number";
    }
    std::cout << "Enter count of treads:" << std::endl << ">";
    if (!(std::cin >> treadCount))
    {
        std::cerr << "TreadCount must be a positive number";
    }

    try
    {
        auto result = MeasureFuncExecutionTime([](uint64_t limit) {
            std::unordered_set<uint64_t> primeSet;
            for (uint64_t i = 0; i <= limit; i++)
            {
                if (IsPrime(i))
                {
                    primeSet.insert(i);
                }
            }
            return primeSet;
        }, limit);
        WriteSetWithSorting("sortedSet.txt", result);
        auto result1 = MeasureFuncExecutionTime(FindPrimesInParallel, limit, treadCount);
        WriteSetWithSorting("sortedSet_parallel.txt", result1);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}