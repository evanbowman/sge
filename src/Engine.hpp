#pragma once

#include <SFML/Graphics.hpp>
#include <list>
#include <unordered_map>

#include "Entity.hpp"
#include "Types.hpp"
#include "Timer.hpp"
#include "TextureManager.hpp"
#include "Animation.hpp"

class Engine {
public:
    using EntityMap = std::unordered_map<UID, std::shared_ptr<Entity>>;
    using TimerMap = std::unordered_map<UID, SteadyTimer>;
    using AnimationMap = std::unordered_map<UID, Animation>;
    
    Engine();

    enum class RunMode {
        Normal, Interactive
    };
    
    void Run(RunMode mode);
    bool IsRunning() const;
    
    UID CreateEntity();
    UID CreateTimer();
    UID CreateAnimation(std::string sourceFile, const Rect& frameDesc);
    
    void RemoveEntity(UID id);
    void RemoveTimer(UID id);

    Entity* GetEntityRaw(UID id);
    USec ResetTimer(UID id);
    
private:
    void EventLoop();

    sf::RenderWindow m_window;
    std::mutex m_entitiesMtx;
    TextureManager m_textureMgr;
    
    EntityMap m_entities;
    TimerMap m_timers;
    AnimationMap m_animations;
    
    struct UIDCounters {
        UID entityCount;
        UID timerCount;
        UID animationCount;
    };
    UIDCounters m_uidCounters;
};
