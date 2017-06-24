#pragma once

#include <new>

#include "MemPool.hpp"

namespace Flight {

template <typename T>
class PoolAllocator {
    static MemPool<T> m_pool;
public:
    using value_type      = T;
    using pointer         = T *;
    using const_pointer   = const T *;
    using reference       = T &;
    using const_reference = const T &;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    template <typename U>
    struct rebind {
        using other = PoolAllocator<U>;
    };
    
    T * allocate(size_type) { return m_pool.Alloc(); }

    void deallocate(T * p, size_type) { m_pool.Release(p); }
    
    void destroy(pointer p) {
        p->~T();
    }

    template <typename ...Args>
    void construct(pointer p, Args ...args) {
        new((void *)p) T(std::forward<Args>(args)...);
    }

    void construct(pointer p, const_reference val) {
        new((void *)p) T(val);
    }
};

template <typename T>
MemPool<T> PoolAllocator<T>::m_pool;

}
