#pragma once

#include <mutex>
#include <memory>
#include <forward_list>
#include <array>

template <typename T, size_t GroupSize>
class MemPool {
    using Byte = uint8_t;
    struct Cell {
        Cell* next;
        T data;
    };
    using Group = std::array<Byte, GroupSize * sizeof(Cell)>;
    
    Cell* m_freeList;
    std::forward_list<Group> m_groups;
    std::mutex m_mutex;
    
    void EnlistGroup() {
        m_groups.emplace_front();
        Cell* cells = reinterpret_cast<Cell*>(m_groups.front().data());
        for (int i = 0; i < GroupSize; ++i) {
            cells[i].next = m_freeList;
            m_freeList = &cells[i];
        }
    }
    
public:
    MemPool(const MemPool& other) = delete;
    
    MemPool() : m_freeList(nullptr) {}

    T* Alloc() {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_freeList == nullptr) EnlistGroup();
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
