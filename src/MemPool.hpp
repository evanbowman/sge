#pragma once

#include <forward_list>
#include <mutex>
#include <memory>
#include <iostream>

template <typename T>
class MemPool {
    using Byte = uint8_t;
    struct Cell {
        Cell* next;
        T data;
    };
    Cell* m_freeList = nullptr;
    std::mutex m_mutex;
    
    void EnlistChunk() {
        static const size_t allocCount = 127;
        static const size_t allocSize = sizeof(Cell) * allocCount;
        auto mem = (Cell*)malloc(allocSize);
        for (Cell* cell = mem; cell < mem + allocCount; ++cell) {
            cell->next = m_freeList;
            m_freeList = cell;
        }
    }
public:
    MemPool(const MemPool& other) = delete;    
    MemPool() : m_freeList(nullptr) {}

    T* Alloc() {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_freeList == nullptr) EnlistChunk();
        T* mem = &m_freeList->data;
        m_freeList = m_freeList->next;
        return mem;
    }

    void Release(T* mem) {
        std::lock_guard<std::mutex> lk(m_mutex);
        static const size_t headerBytes = sizeof(Cell*);
        auto cell = reinterpret_cast<Cell *>((Byte*)mem - headerBytes);
        cell->next = m_freeList;
        m_freeList = cell;
    }
};
