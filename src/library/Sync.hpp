#pragma once

#include <mutex>
#include <functional>

template <typename T, typename Lock = std::mutex>
class Sync {
public:
    template <typename F>
    void Enter(F&& handler) {
        std::lock_guard<Lock> lkGuard(m_lock);
        handler(m_data);
    }
private:
    Lock m_lock;
    T m_data;
};
