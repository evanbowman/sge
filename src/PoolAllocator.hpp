#pragma once

#include <new>

#include "MemPool.hpp"

template <typename T>
class PoolAllocator {
    static MemPool<T> m_pool;
public:
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    template <typename U>
    struct rebind {
        using other = PoolAllocator<U>;
    };

    PoolAllocator() noexcept {};
    
    PoolAllocator(const PoolAllocator&) noexcept {};
    
    PoolAllocator<T>& operator=(const PoolAllocator&) { return *this; }
    
    ~PoolAllocator() {}
    
    template<typename U>
    PoolAllocator(const PoolAllocator<U>&) noexcept {};
    
    pointer allocate(size_type) { return m_pool.Alloc(); }

    void deallocate(pointer p, size_type) { m_pool.Release(p); }
    
    void destroy(pointer p) {
        p->~T();
    }

    template <typename ...Args>
    void construct(pointer p, Args ...args) {
        new((void*)p) T(std::forward<Args>(args)...);
    }

    void construct(pointer p, const_reference val) {
        new((void*)p) T(val);
    }
};

template <typename T, typename U>
inline bool operator==(const PoolAllocator<T>&, const PoolAllocator<U>&) {
    return true;
}

template <typename T, typename U>
inline bool operator!=(const PoolAllocator<T>& a, const PoolAllocator<U>& b) {
    return !(a == b);
}

template <typename T>
MemPool<T> PoolAllocator<T>::m_pool;
