#pragma once

/* Engine.hpp
   A simple game engine interface. All of the engine's
   public member functions return and accept opaque
   Unique IDentifiers, which serve as handles to resources
   owned by the engine. Exposing only typeless integer 
   handles may seem strange at first, but the engine is 
   designed to be used with similarly typeless dynamic
   scripting languages like Scheme and Lua.

   The engine does not deal with scripting language VM's
   in the currency of foreign objects, SMOBS, Userdata,
   or what have you, because these things are API specific,
   and rely on the a Script VM's garbage collector for
   the deallocation of resources. Some VM's run garbage
   collection only periodically, or even never for small
   objects (I'm looking at you, V8). */

#include <SFML/Graphics.hpp>
#include <list>
#include <unordered_map>

#include "Animation.hpp"
#include "Entity.hpp"
#include "Types.hpp"
#include "Timer.hpp"
#include "TextureManager.hpp"
#include "Renderer.hpp"

class Engine {
public:
    using EntityRef = std::shared_ptr<Entity>;
    using EntityMap = std::unordered_map<UID, EntityRef>;
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
    UID CreateAnimation(std::string sourceFile,
                        const Rect& frameDesc,
                        const Vec2& origin);

    void SetEntityAnimation(UID entity, UID animation);
    void SetEntityKeyframe(UID entity, size_t keyframe);
    void SetEntityPosition(UID entity, const Vec2& position);
    const Vec2& GetEntityPosition(UID entity);
    size_t GetEntityKeyframe(UID entity);
    
    void RemoveEntity(UID id);
    void RemoveTimer(UID id);
    
    USec ResetTimer(UID id);
    
private:
    void EventLoop();

    EntityMap::iterator FindEntityById(UID id);
    
    void EnqueueTextureRequest(std::shared_ptr<TextureLoadRequest> req);
    void HandleTextureRequests();

    sf::RenderWindow m_window;
    Renderer m_renderer;

    std::mutex m_entitiesMtx;
    TextureManager m_textureMgr;
    std::mutex m_textureRequestMtx;
    std::vector<std::shared_ptr<TextureLoadRequest>> m_textureRequests;
    
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
