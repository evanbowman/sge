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
    void RemoveEntity(URI id);
    Entity* GetEntityRaw(URI id);
    bool IsRunning() const;
    USec ResetTimer(URI id);
    URI CreateTimer();
    
private:
    void EventLoop();

    sf::RenderWindow m_window;
    std::mutex m_entitiesMtx;
    // TODO: Add PoolAllocator for entities?
    std::unordered_map<URI, std::shared_ptr<Entity>> m_entities;
    std::vector<Timer<std::chrono::steady_clock>> m_timers;
    URI m_uriCounter;
};
