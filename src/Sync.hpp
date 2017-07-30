#pragma once

#include <mutex>
#include <functional>

template <typename T>
class Sync {
public:
    template <typename F>
    void Get(F&& handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        handler(m_data);
    }
private:
    std::mutex m_mutex;
    T m_data;
};
