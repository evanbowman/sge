#pragma once

#include <atomic>
#include <mutex>

class SpinLock {
public:
    void lock();

    void unlock();
    
private:
    std::atomic_flag m_lockFlag = ATOMIC_FLAG_INIT;
};
