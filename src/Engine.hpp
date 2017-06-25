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
    bool IsRunning() const;
    
    URI CreateEntity();
    URI CreateTimer();
    URI CreateAnimation();
    
    void RemoveEntity(URI id);

    Entity* GetEntityRaw(URI id);
    USec ResetTimer(URI id);
    
private:
    void EventLoop();

    sf::RenderWindow m_window;
    std::mutex m_entitiesMtx;
    std::unordered_map<URI, std::shared_ptr<Entity>> m_entities;
    std::unordered_map<URI, Timer<std::chrono::steady_clock>> m_timers;
    
    struct URICounters {
        URI entityCount;
        URI timerCount;
        URI animationCount;
    };
    URICounters m_uriCounters;
};
