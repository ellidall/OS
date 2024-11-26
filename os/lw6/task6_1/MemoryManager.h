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
        m_head = reinterpret_cast<Block*>(start);
        m_head->start = static_cast<char*>(start) + sizeof(Block);
        m_head->size = size - sizeof(Block);
        m_head->free = true;
        m_head->prev = nullptr;
        m_head->next = nullptr;
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

        Block* current = m_head;
        while (current != nullptr)
        {
            if (current->free && current->size >= size)
            {
                auto alignedAddr = MemoryManager::CalcAlignedAddress(current, align);
                size_t alignedSize = size + (alignedAddr - reinterpret_cast<uintptr_t>(current->start));

                if (alignedSize <= current->size)
                {
                    if (current->size > alignedSize + sizeof(Block))
                    {
                        auto newBlock = reinterpret_cast<Block*>(static_cast<char*>(current->start) + alignedSize);
                        newBlock->start = reinterpret_cast<char*>(newBlock) + sizeof(Block);
                        newBlock->size = current->size - alignedSize - sizeof(Block);
                        newBlock->free = true;
                        newBlock->prev = current;
                        newBlock->next = current->next;

                        if (current->next)
                        {
                            current->next->prev = newBlock;
                        }
                        current->next = newBlock;
                    }

                    current->size = alignedSize;
                    current->free = false;

                    return reinterpret_cast<void*>(alignedAddr);
                }
            }
            current = current->next;
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

        Block* current = m_head;
        while (current != nullptr)
        {
            if (current->start == addr)
            {
                if (current->free)
                {
                    return;
                }
                current->free = true;
                MemoryManager::MergeBlocks(current);
                return;
            }
            current = current->next;
        }
    }

private:
    struct Block
    {
        void* start;
        size_t size;
        bool free;
        Block* prev;
        Block* next;
    };

    std::mutex m_mutex;
    Block* m_head = nullptr;

    uintptr_t CalcAlignedAddress(Block*& current, size_t align = alignof(std::max_align_t))
    {
        auto alignedAddr = reinterpret_cast<uintptr_t>(current->start);
        uintptr_t offset = alignedAddr % align;
        if (offset != 0)
        {
            alignedAddr += (align - offset);
        }
        return alignedAddr;
    }

    void MergeBlocks(Block*& current)
    {
        if (current->prev && current->prev->free)
        {
            current->prev->size += current->size + sizeof(Block);
            current->prev->next = current->next;

            if (current->next)
            {
                current->next->prev = current->prev;
            }
            current = current->prev;
        }

        if (current->next && current->next->free)
        {
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;

            if (current->next)
            {
                current->next->prev = current;
            }
        }
    }
};

