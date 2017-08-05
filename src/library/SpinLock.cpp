#include "SpinLock.hpp"

void SpinLock::lock() {
    while (m_lockFlag.test_and_set(std::memory_order_acquire));
}

void SpinLock::unlock() {
    m_lockFlag.clear(std::memory_order_release);
}
