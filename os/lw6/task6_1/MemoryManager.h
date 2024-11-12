#pragma once

#include <cassert>
#include <memory>
#include <cstddef>
#include <vector>
#include <ranges>

class MemoryManager
{
public:
    MemoryManager(void* start, size_t size) noexcept
            : m_current(static_cast<char*>(start)), m_freeMemory(size)
    {}

    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    void* Allocate(size_t size, size_t align = alignof(std::max_align_t)) noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (size == 0 || align == 0 || (align & (align - 1)) != 0)
        {
            return nullptr;
        }

        void* alignedPtr = std::align(align, size, reinterpret_cast<void*&>(m_current), m_freeMemory);
        if (!alignedPtr || m_freeMemory < size)
        {
            return nullptr;
        }

        m_current = static_cast<char*>(alignedPtr) + size;
        m_freeMemory -= size;
        m_allocations.emplace_back(alignedPtr, size);
        return alignedPtr;
    }

    void Free(void* addr) noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (addr == nullptr)
        {
            return;
        }

        auto it = std::ranges::find_if(m_allocations, [&addr](const Allocation& alloc) {
            return alloc.ptr == addr;
        });
        if (it != m_allocations.end())
        {
            m_freeMemory += it->size;
            m_allocations.erase(it);
        }
    }

private:
    struct Allocation
    {
        void* ptr;
        size_t size;

        Allocation(void* p, size_t s) : ptr(p), size(s)
        {}
    };

    std::mutex m_mutex;
    char* m_current;
    size_t m_freeMemory;
    // нужно чтобы данные размещались в самом буфере O(1)
    std::vector<Allocation> m_allocations;
};