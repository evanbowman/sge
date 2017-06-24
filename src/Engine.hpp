#pragma once

#include <SFML/Graphics.hpp>
#include <list>
#include <unordered_map>

#include "PoolAllocator.hpp"
#include "Entity.hpp"
#include "Types.hpp"
#include "Timer.hpp"

class Engine {
public:
    Engine();

    enum class RunMode {
        Normal, Interactive
    };
    
    void Run(RunMode mode);
    
    URI CreateEntity();
    bool IsRunning() const;
    USec ResetTimer(URI timer);
    URI CreateTimer();
    
private:
    void EventLoop();

    sf::RenderWindow m_window;
    std::mutex m_entitiesMtx;
    std::unordered_map<URI, Entity> m_entities; // TODO: Add PoolAllocator
    std::vector<Timer<std::chrono::steady_clock>> m_timers;
    URI m_uriCounter;
};
