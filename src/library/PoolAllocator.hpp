#pragma once

#include <new>

#include "MemPool.hpp"

template <typename T, size_t GroupSize>
class PoolAllocator {
    static MemPool<T, GroupSize> m_pool;
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
        using other = PoolAllocator<U, GroupSize>;
    };

    PoolAllocator() noexcept {};
    
    PoolAllocator(const PoolAllocator&) noexcept {};
    
    PoolAllocator<T, GroupSize>&
    operator=(const PoolAllocator&) { return *this; }
    
    ~PoolAllocator() {}
    
    template<typename U>
    PoolAllocator(const PoolAllocator<U, GroupSize>&) noexcept {};
    
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

template <typename T, typename U, size_t GroupSize>
inline bool operator==(const PoolAllocator<T, GroupSize>&,
                       const PoolAllocator<U, GroupSize>&) {
    return true;
}

template <typename T, typename U, size_t GroupSize>
inline bool operator!=(const PoolAllocator<T, GroupSize>& a,
                       const PoolAllocator<U, GroupSize>& b) {
    return !(a == b);
}

template <typename T, size_t GroupSize>
MemPool<T, GroupSize> PoolAllocator<T, GroupSize>::m_pool;
