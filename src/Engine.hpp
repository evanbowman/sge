#pragma once

#include <SFML/Graphics.hpp>
#include <list>
#include <unordered_map>
#include <random>

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
    using RNGMap = std::unordered_map<UID, std::mt19937>;
    
    Engine();

    enum class RunMode {
        Normal, Interactive
    };
    
    void Run(RunMode mode);
    bool IsRunning() const;
    
    UID CreateEntity();
    UID CreateTimer();
    UID CreateRNG();
    UID CreateAnimation(std::string sourceFile,
                        const Rect& frameDesc,
                        const Vec2& origin);
    
    void RemoveEntity(UID id);
    void RemoveTimer(UID id);
    void RemoveRNG(UID id);
    
    Entity* GetEntityRaw(UID id);
    Animation* GetAnimationRaw(UID id);
    USec ResetTimer(UID id);
    std::mt19937::result_type GetRandom(UID id);
    
private:
    void EventLoop();
    
    void EnqueueTextureRequest(std::shared_ptr<TextureLoadRequest> req);
    void HandleTextureRequests();
    
    sf::RenderWindow m_window;
    std::mutex m_entitiesMtx;
    TextureManager m_textureMgr;
    std::mutex m_textureRequestMtx;
    std::vector<std::shared_ptr<TextureLoadRequest>> m_textureRequests;
    
    EntityMap m_entities;
    TimerMap m_timers;
    AnimationMap m_animations;
    RNGMap m_rngs;

    struct UIDCounters {
        UID entityCount;
        UID timerCount;
        UID animationCount;
        UID rngCount;
    };
    UIDCounters m_uidCounters;
};
