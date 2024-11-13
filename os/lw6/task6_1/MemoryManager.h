#pragma once

#include <mutex>
#include <cstddef>
#include <memory>
#include <algorithm>

class MemoryManager
{
public:
    MemoryManager(void* start, size_t size) noexcept
    {
        m_head = std::make_unique<Block>(static_cast<char*>(start), size, true);
    }

    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    void* Allocate(size_t size, size_t align = alignof(std::max_align_t)) noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (size == 0 || align == 0 || (align & (align - 1)) != 0)
        {
            return nullptr;
        }

        Block* current = m_head.get();
        while (current != nullptr)
        {
            if (current->free && current->size >= size)
            {
                void* alignedPtr = std::align(align, size, reinterpret_cast<void*&>(current->start), current->size);
                if (alignedPtr && current->size >= size)
                {
                    if (current->size > size)
                    {
                        auto newBlock = std::make_unique<Block>(
                                static_cast<char*>(alignedPtr) + size,
                                current->size - size,
                                true
                        );
                        newBlock->next = std::move(current->next);
                        current->next = std::move(newBlock);
                        current->size = size;
                    }

                    current->free = false;
                    return alignedPtr;
                }
            }
            current = current->next.get();
        }
        return nullptr;
    }

    void Free(void* addr) noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (addr == nullptr)
        {
            return;
        }

        Block* current = m_head.get();
        Block* prev = nullptr;
        while (current != nullptr)
        {
            if (current->start == addr)
            {
                current->free = true;

                if (prev && prev->free)
                {
                    prev->size += current->size;
                    prev->next = std::move(current->next);
                    current = prev;
                }

                Block* next = current->next.get();
                if (next && next->free)
                {
                    current->size += next->size;
                    current->next = std::move(next->next);
                }
                return;
            }

            prev = current;
            current = current->next.get();
        }
    }
    // заменить на двусвязный список
    // количество памяти должно быть константным

private:
    struct Block
    {
        void* start;
        size_t size = 0;
        bool free = false;
        std::unique_ptr<Block> next;

        Block(void* s, size_t sz, bool f) : start(s), size(sz), free(f), next(nullptr)
        {}
    };

    std::mutex m_mutex;
    std::unique_ptr<Block> m_head;
};
