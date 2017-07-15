#pragma once

/* Engine.hpp
   A simple game engine interface. All of the engine's
   public member functions return and accept opaque
   Unique IDentifiers, which serve as handles to resources
   owned by the engine. Exposing only typeless integer 
   handles may seem strange at first, but the engine is 
   designed to be used with similarly typeless dynamic
   scripting languages like Scheme and Lua.

   The engine does not deal with scripting language VMs
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
#include "Camera.hpp"
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

    UIVec2 GetWindowSize() const;
    
    UID CreateEntity();
    UID CreateTimer();
    UID CreateAnimation(std::string sourceFile,
                        const Rect& frameDesc,
                        const Vec2& origin);

    void SetEntityAnimation(UID entity, UID animation);
    void SetEntityKeyframe(UID entity, size_t keyframe);
    void SetEntityPosition(UID entity, const Vec2& position);
    void SetEntityScale(UID entity, const Vec2& scale);
    void SetEntityBlendMode(UID entity, const sf::BlendMode& blendMode);
    void SetEntityZOrder(UID entity, ZOrderIndex zOrder);
    void SetEntityColor(UID entity, const Color& color);
    void SetCameraTarget(UID entity);
    void SetCameraCenter(const Vec2& center);
    void SetCameraSpringiness(float springiness);
    void SetCameraZoom(float zoom);
    
    const Vec2& GetEntityPosition(UID entity);
    size_t GetEntityKeyframe(UID entity);
    
    void RemoveEntity(UID entity);
    void RemoveTimer(UID timer);
    
    USec ResetTimer(UID timer);
    
private:
    void EventLoop();

    EntityMap::iterator FindEntity(UID entity);
    GraphicsComponent& FindGfxComp(UID entity);
    
    void EnqueueTextureRequest(std::shared_ptr<TextureRequest> req);
    void HandleTextureRequests();

    sf::RenderWindow m_window;
    Renderer m_renderer;

    std::mutex m_entitiesMtx;
    TextureManager m_textureMgr;
    std::mutex m_textureRequestMtx;
    std::vector<std::shared_ptr<TextureRequest>> m_textureRequests;
    
    EntityMap m_entities;
    TimerMap m_timers;
    AnimationMap m_animations;
    Camera m_camera;

    struct UIDCounters {
        UID entityCount;
        UID timerCount;
        UID animationCount;
    };
    UIDCounters m_uidCounters;
};
