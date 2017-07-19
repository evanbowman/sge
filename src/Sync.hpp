#pragma once

#include <mutex>
#include <functional>

template <typename T>
class Sync {
public:
    void Get(std::function<void(T&)> handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        handler(m_data);
    }
private:
    std::mutex m_mutex;
    T m_data;
};
