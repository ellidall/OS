#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <thread>
#include "../task6_1/MemoryManager.h"

class MemoryManagerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        buffer = std::vector<char>(1000);
        memoryManager = std::make_unique<MemoryManager>(buffer.data(), buffer.size());
    }

    void TearDown() override
    {}

    std::vector<char> buffer;
    std::unique_ptr<MemoryManager> memoryManager;
};

TEST_F(MemoryManagerTest, AllocateValidMemory)
{
    auto ptr = memoryManager->Allocate(sizeof(double));
    ASSERT_NE(ptr, nullptr);

    *static_cast<double*>(ptr) = 3.1415927;
    ASSERT_EQ(*static_cast<double*>(ptr), 3.1415927);
}

TEST_F(MemoryManagerTest, AllocateInvalidMemoryZeroSize)
{
    auto ptr = memoryManager->Allocate(0);
    ASSERT_EQ(ptr, nullptr);
}

TEST_F(MemoryManagerTest, AllocateInvalidMemoryZeroAlignment)
{
    auto ptr = memoryManager->Allocate(sizeof(double), 0);
    ASSERT_EQ(ptr, nullptr);
}

TEST_F(MemoryManagerTest, AllocateExceedsAvailableMemory)
{
    auto ptr = memoryManager->Allocate(buffer.size() + 1);
    ASSERT_EQ(ptr, nullptr);
}

TEST_F(MemoryManagerTest, FreeMemory)
{
    auto ptr = memoryManager->Allocate(sizeof(double));
    ASSERT_NE(ptr, nullptr);

    memoryManager->Free(ptr);
    memoryManager->Free(ptr);

    auto newPtr = memoryManager->Allocate(sizeof(double));
    ASSERT_NE(newPtr, nullptr);
    ASSERT_NE(newPtr, ptr);
}

TEST_F(MemoryManagerTest, FreeInvalidMemory)
{
    memoryManager->Free(nullptr);
    ASSERT_NO_THROW(memoryManager->Free(nullptr));
}

TEST_F(MemoryManagerTest, AllocateAndFreeMultipleTimes)
{
    auto ptr1 = memoryManager->Allocate(sizeof(double));
    auto ptr2 = memoryManager->Allocate(sizeof(int));

    ASSERT_NE(ptr1, ptr2);

    memoryManager->Free(ptr1);
    memoryManager->Free(ptr2);

    auto ptr3 = memoryManager->Allocate(sizeof(float));
    ASSERT_NE(ptr3, nullptr);
}

TEST_F(MemoryManagerTest, AllocateMemoryInMultipleThreads)
{
    const size_t allocationSize = sizeof(double);
    const int numThreads = 10;
    std::vector<void*> pointers(numThreads, nullptr);
    std::atomic<int> successCount(0);

    std::vector<std::jthread> threads;
    threads.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([this, i, &pointers, &successCount] {
            void* ptr = memoryManager->Allocate(allocationSize);
            if (ptr != nullptr)
            {
                pointers[i] = ptr;
                successCount++;
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    ASSERT_EQ(successCount.load(), numThreads);
}

TEST_F(MemoryManagerTest, FreeMemoryInMultipleThreads)
{
    const size_t allocationSize = sizeof(double);
    const int numThreads = 10;
    std::vector<void*> pointers(numThreads, nullptr);
    std::atomic<int> successCount(0);

    for (int i = 0; i < numThreads; ++i)
    {
        void* ptr = memoryManager->Allocate(allocationSize);
        if (ptr != nullptr)
        {
            pointers[i] = ptr;
            successCount++;
        }
    }

    ASSERT_EQ(successCount.load(), numThreads);

    std::atomic<int> freeCount(0);
    std::vector<std::jthread> threads;
    threads.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([this, i, &pointers, &freeCount] {
            memoryManager->Free(pointers[i]);
            freeCount++;
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    ASSERT_EQ(freeCount.load(), successCount);
}

TEST_F(MemoryManagerTest, AllocateAndFreeInMultipleThreads) {
    const size_t allocationSize = sizeof(double);
    const int numThreads = 10;

    std::vector<void*> pointers(numThreads, nullptr);
    std::atomic<int> successCount(0);
    std::atomic<int> freeCount(0);

    std::vector<std::jthread> threads;
    threads.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([this, i, &pointers, &successCount, &freeCount] {
            void* ptr = memoryManager->Allocate(allocationSize);
            if (ptr != nullptr) {
                pointers[i] = ptr;
                successCount++;
                memoryManager->Free(ptr);
                freeCount++;
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    ASSERT_EQ(successCount.load(), numThreads);
    ASSERT_EQ(freeCount.load(), successCount);
}