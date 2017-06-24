#pragma once

#include <chrono>

#include "Types.hpp"

template <typename Clock>
class Timer {
    using Instant = typename Clock::time_point;
public:
    Timer() : m_lastReset(Clock::now()) {}

    USec Reset() {
        using namespace std::chrono;
        const Instant now = Clock::now();
        const USec ret = duration_cast<microseconds>(now - m_lastReset).count();
        m_lastReset = now;
        return ret;
    }
    
private:
    Instant m_lastReset;
};

using SteadyTimer = Timer<std::chrono::steady_clock>;
