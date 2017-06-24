#pragma once

#include <forward_list>
#include <mutex>
#include <memory>

namespace Flight {

template <typename T>
class MemPool {
    using Byte = uint8_t;
    struct Cell {
        Cell * next;
        T data;
    };
    std::forward_list<std::unique_ptr<Byte[]>> m_chunks;
    Cell * m_freeList = nullptr;
    size_t m_allocQuota = 2;
    std::mutex m_mutex;
    
    void EnlistChunk() {
        m_chunks.emplace_front(new Byte[m_allocQuota * sizeof(Cell)]);
        for (size_t i = 0; i < m_allocQuota; ++i) {
            auto & cell = reinterpret_cast<Cell *>(m_chunks.front().get())[i];
            cell.next = m_freeList;
            m_freeList = &cell;
        }
        m_allocQuota *= 2;
    }
public:
    MemPool(const MemPool & other) = delete;    
    MemPool() : m_freeList(nullptr), m_allocQuota(2) {}

    T * Alloc() {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_freeList == nullptr) EnlistChunk();
        auto mem = &m_freeList->data;
        m_freeList = m_freeList->next;
        return mem;
    }

    void Release(T * mem) {
        std::lock_guard<std::mutex> lk(m_mutex);
        static const size_t headerBytes = sizeof(Cell *);
        auto cell = reinterpret_cast<Cell *>((Byte *)mem - headerBytes);
        cell->next = m_freeList;
        m_freeList = cell;
    }
};

}
