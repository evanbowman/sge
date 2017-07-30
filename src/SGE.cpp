#include "../include/SGE/SGE.h"

#include <atomic>
#include <array>
#include <SFML/Graphics.hpp>
#include <list>
#include <thread>
#include <iostream>
#include <queue>

#include "Animation.hpp"
#include "Camera.hpp"
#include "CollisionChecker.hpp"
#include "Entity.hpp"
#include "Renderer.hpp"
#include "ResourcePath.hpp"
#include "Singleton.hpp"
#include "Sync.hpp"
#include "Types.hpp"
#include "Timer.hpp"
#include "TextureManager.hpp"
#include "Utility.hpp"
#include <sparsepp/spp.h>

namespace errors {
    static const auto badEntityHandle = "Bad entity handle";
    static const auto badAnimationHandle = "Bad animation handle";
    static const auto gfxCompNotAnim = "Gfx component is not an animation";
    static const auto missingGfxComp = "Entity has no graphics component";
    static const auto badBM = "Invalid blend mode id";
}

// TODO: sparse_hash supposedly needs a special allocator for
// MS Windows.
using EntityMap = spp::sparse_hash_map<SGE_UUID, EntityRef>;
using TimerMap = spp::sparse_hash_map<SGE_UUID, SteadyTimer>;
using AnimationMap = std::unordered_map<SGE_UUID, Animation>;
using EventQueue = std::queue<SGE_EventHolder>;

class Engine {
public:
    Engine() : window(sf::VideoMode::getDesktopMode(),
                      "SGE",
                      sf::Style::Fullscreen),
               camera(window),
               m_running(false),
               m_uidCounter(0) {
        window.setMouseCursorVisible(false);
        window.setVerticalSyncEnabled(true);        
    }

    void RecordEvent(const SGE_EventHolder& holder) {
        m_events.Get([&holder](std::queue<SGE_EventHolder>& eventsQueue) {
            eventsQueue.push(holder);
        });
    }
    
    void EventLoop() {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                m_running = false;
                break;

            case sf::Event::TextEntered: {
                SGE_EventHolder holder;
                holder.event.text.unicode = event.text.unicode;
                holder.code = SGE_EventCode_TextEntered;
                RecordEvent(holder);
                break;
            }

            case sf::Event::KeyPressed: {
                SGE_EventHolder holder;
                holder.event.key.keyCode = (SGE_KeyCode)event.key.code;
                holder.code = SGE_EventCode_KeyPressed;
                RecordEvent(holder);
                break;
            }

            case sf::Event::KeyReleased: {
                SGE_EventHolder holder;
                holder.event.key.keyCode = (SGE_KeyCode)event.key.code;
                holder.code = SGE_EventCode_KeyReleased;
                RecordEvent(holder);
                break;
            }
            }
        }
    }

    using TextureReqList = std::vector<std::shared_ptr<TextureRequest>>;
    
    void EnqueueTextureRequest(std::shared_ptr<TextureRequest> req) {
        m_textureRequests.Get([&](TextureReqList& requests) {
            requests.push_back(req);
        });
    }

    void HandleTextureRequests() {
        m_textureRequests.Get([&](TextureReqList& requests) {
            for (auto & req : requests) {
                textureMgr.Accept(*req);
            }
            requests.clear();
        });
    }

    void GraphicsLoop() {
        SteadyTimer gfxDeltaTimer;
        while (window.isOpen()) {
            HandleTextureRequests();
            EventLoop();
            camera.Update(gfxDeltaTimer.Reset());
            renderer.Configure(camera);
            m_entities.Get([this](EntityMap& entities) {
                for (auto& entityNode : entities) {
                    auto& entity = entityNode.second;
                    if (!entity->HasAttribute(SGE_Attr_Hidden)) {
                        if (auto gfx = entity->GetGraphicsComponent()) {
                            gfx->Dispatch(*entity.get(), this->renderer);
                        }
                    }
                }
            });
            window.clear(refreshColor);
            renderer.Display(window);
            window.display();
            if (!m_running) {
                window.close();
            }
        }
    }
    
    void Run(std::function<void()> entry) {
        m_running = true;
        std::thread logicThread([entry] {
            entry();
        });
        GraphicsLoop();
        logicThread.join();
    }

    sf::RenderWindow window;
    Camera camera;
    Renderer renderer;
    CollisionChecker collisionChecker;
    sf::Color refreshColor;
    TextureManager textureMgr;

    // Note: shallow copy only. Pass a statically allocated string.
    void PushError(const char* err) {
        std::lock_guard<std::mutex> lock(m_errorsMtx);
        m_errors.push_back(err);
    }

    bool HasError() {
        std::lock_guard<std::mutex> lock(m_errorsMtx);
        return m_errors.empty();
    }

    const char* PollError() {
        std::lock_guard<std::mutex> lock(m_errorsMtx);
        if (!m_errors.empty()) {
            const auto lastError = m_errors.back();
            m_errors.pop_back();
            return lastError;
        }
        return nullptr;
    }

    template <typename F>
    void WithEntities(F&& procedure) {
        m_entities.Get(procedure);
    }

    template <typename F>
    void WithAnimations(F&& procedure) {
        m_animations.Get(procedure);
    }

    template <typename F>
    void WithEvents(F&& procedure) {
        m_events.Get(procedure);
    }

    template <typename F>
    void WithTimers(F&& procedure) {
        m_timers.Get(procedure);
    }
    
    SGE_UUID NewUUID() { return ++m_uidCounter; }

    bool IsRunning() const { return m_running.load(); }
    void Exit() { m_running = false; }
    
private:
    Sync<EntityMap> m_entities;
    Sync<std::queue<SGE_EventHolder>> m_events;
    Sync<AnimationMap> m_animations;
    Sync<TimerMap> m_timers;
    Sync<TextureReqList> m_textureRequests;
    std::atomic_bool m_running;
    std::vector<const char*> m_errors;
    std::mutex m_errorsMtx;
    std::atomic<SGE_UUID> m_uidCounter;
};

auto& g_engine = Singleton<Engine>::Instance();

namespace {
    Entity* FindEntity(EntityMap& entities, SGE_UUID id) {
        auto entityIter = entities.find(id);
        if (entityIter == entities.end()) {
            g_engine.PushError(errors::badEntityHandle);
            return nullptr;
        }
        return entityIter->second.get();
    }

    GraphicsComponent* FindGfxComp(EntityMap& entities, SGE_UUID id) {
        auto foundEntity = FindEntity(entities, id);
        if (!foundEntity) {
            return nullptr;
        }
        if (auto gfxComp = foundEntity->GetGraphicsComponent()) {
            return gfxComp;
        }
        g_engine.PushError(errors::missingGfxComp);
        return nullptr;
    }

    EntityRef FindEntityRef(EntityMap& entities, SGE_UUID entity) {
        auto entityIter = entities.find(entity);
        if (entityIter == entities.end()) {
            g_engine.PushError(errors::badEntityHandle);
            return nullptr;
        }
        return entityIter->second;
    }
}

extern "C" {
    SGE_Bool SGE_Main(void(*entryFn)()) {
        g_engine.Run(entryFn);
        return static_cast<SGE_Bool>(g_engine.HasError());
    }
    
    SGE_Bool SGE_IsRunning() {
        return static_cast<SGE_Bool>(g_engine.IsRunning());
    }
    
    void SGE_Exit() {  g_engine.Exit(); }

    void SGE_SetRefreshColor(const SGE_Color* color) {
        g_engine.refreshColor = sf::Color(color->r, color->g,
                                          color->b, color->a);
    }

    SGE_UIVec2 SGE_GetWindowSize() {
        const auto& windowSize = g_engine.window.getSize();
        return { windowSize.x, windowSize.y };
    }

    SGE_Bool SGE_EntityCreate(SGE_UUID* entity) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity](EntityMap& entities) {
            auto created = std::make_shared<Entity>();
            if (created) {
                const auto uuid = g_engine.NewUUID();
                entities[uuid] = created;
                *entity = uuid;
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntityClone(SGE_UUID src, SGE_UUID* dest) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, src, dest](EntityMap& entities) {
            if (auto foundSrc = FindEntity(entities, src)) {
                auto clone = std::make_shared<Entity>();
                if (!clone) {
                    return;
                }
                clone->SetAttributeSet(foundSrc->GetAttributeSet());
                clone->SetPosition(foundSrc->GetPosition());
                std::unique_ptr<GraphicsComponent> gfxCompClone;
                if (auto srcGfxComp = foundSrc->GetGraphicsComponent()) {
                    gfxCompClone =
                        std::unique_ptr<GraphicsComponent>(srcGfxComp->Clone());
                    if (!gfxCompClone) {
                        return;
                    }
                }
                clone->SetGraphicsComponent(std::move(gfxCompClone));
                const auto uuid = g_engine.NewUUID();
                entities[uuid] = clone;
                *dest = uuid;
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntityAddAttribute(SGE_UUID entity, SGE_Attribute attrib) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, attrib](EntityMap& entities) {
            if (auto foundEntity = FindEntity(entities, entity)) {
                foundEntity->AddAttribute(attrib);
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntitySetAnimation(SGE_UUID entity, SGE_UUID animation) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, animation](EntityMap& entities) {
            auto foundEntity = FindEntity(entities, entity);
            if (!foundEntity) {
                return;
            }
            g_engine.WithAnimations([&](AnimationMap& animations) {
                auto animationIter = animations.find(animation);
                if (animationIter == animations.end()) {
                    g_engine.PushError(errors::badAnimationHandle);
                    return;
                }
                foundEntity->SetGraphicsComponent({
                        make_unique<AnimationComponent>(&animationIter->second)
                    });
                rc = SGE_True;
            });
        });
        return rc;
    }

    SGE_Bool SGE_EntitySetKeyframe(SGE_UUID entity, SGE_Keyframe keyframe) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, keyframe](EntityMap& entities) {
            if (auto gfxComp = FindGfxComp(entities, entity)) {
                if (gfxComp->TypeId() !=
                    GraphicsComponent::Id::AnimationComponent) {
                    g_engine.PushError(errors::gfxCompNotAnim);
                    return;
                }
                reinterpret_cast<AnimationComponent*>(gfxComp)->
                    SetKeyframe(keyframe);
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntitySetPosition(SGE_UUID entity, const SGE_Vec2* pos) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, &pos](EntityMap& entities) {
            if (auto foundEntity = FindEntity(entities, entity)) {
                foundEntity->SetPosition({ pos->x, pos->y });
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntitySetScale(SGE_UUID entity, const SGE_Vec2* scale) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, &scale](EntityMap& entities) {
            if (auto gfxComp = FindGfxComp(entities, entity)) {
                gfxComp->SetScale({ scale->x, scale->y });
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntitySetBlendMode(SGE_UUID entity, SGE_BlendMode mode) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, mode](EntityMap& entities) {
            static const std::array<sf::BlendMode, 4> modes {{
                sf::BlendNone, sf::BlendAdd, sf::BlendAlpha, sf::BlendMultiply
            }};
            if (mode >= 0 && mode < SGE_BM_Count) {
                if (auto gfxComp = FindGfxComp(entities, entity)) {
                    gfxComp->GetRenderStates().blendMode = modes[mode];
                    rc = SGE_True;
                }
            } else {
                g_engine.PushError(errors::badBM);
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntitySetZOrder(SGE_UUID entity, int zOrder) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, zOrder](EntityMap& entities) {
            if (auto gfxComp = FindGfxComp(entities, entity)) {
                gfxComp->SetZOrder(zOrder);
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntitySetColor(SGE_UUID entity, const SGE_Color* color) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, &color](EntityMap& entities) {
            if (auto gfxComp = FindGfxComp(entities, entity)) {
                gfxComp->SetColor({ color->r, color->g, color->b, color->a });
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntityGetPosition(SGE_UUID entity, SGE_Vec2* position) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, position](EntityMap& entities) {
            if (auto foundEntity = FindEntity(entities, entity)) {
                const auto& entityPos = foundEntity->GetPosition();
                *position = { entityPos.x, entityPos.y };
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntityGetKeyframe(SGE_UUID entity, SGE_Keyframe* keyframe) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, keyframe](EntityMap& entities) {
            if (auto gfxComp = FindGfxComp(entities, entity)) {
                if (gfxComp->TypeId() ==
                    GraphicsComponent::Id::AnimationComponent) {
                    *keyframe = reinterpret_cast<AnimationComponent*>(gfxComp)
                        ->GetKeyframe();
                    rc = SGE_True;
                }
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntityRemove(SGE_UUID entity) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity](EntityMap& entities) {
            auto entityIter = entities.find(entity);
            if (entityIter != entities.end()) {
                entities.erase(entityIter);
                rc = SGE_True;
            }
            g_engine.PushError(errors::badEntityHandle);
        });
        return rc;
    }

    SGE_Bool SGE_EntityRemoveAttribute(SGE_UUID entity, SGE_Attribute attrib) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, attrib](EntityMap& entities) {
            if (auto foundEntity = FindEntity(entities, entity)) {
                foundEntity->RemoveAttribute(attrib);
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_EntityEmitSound(SGE_UUID entity,
                                 SGE_UUID sound,
                                 float attenuation ) {
        SGE_Bool ret = SGE_False;
        // TODO...
        return ret;
    }
    
    SGE_Bool SGE_CameraSetTarget(SGE_UUID entity) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity](EntityMap& entities) {
            if (auto entityRef = FindEntityRef(entities, entity)) {
                g_engine.camera.SetTarget(entityRef);
                rc = SGE_True;
            }
        });
        return rc;
    }

    void SGE_CameraSetCenter(const SGE_Vec2* center) {
        g_engine.camera.SetCenter({ center->x, center->y });
    }

    void SGE_CameraSetSpringiness(float springiness) {
        g_engine.camera.SetSpringiness(springiness);
    }

    void SGE_CameraSetZoom(float zoom) {
        g_engine.camera.SetZoom(zoom);
    }

    SGE_Vec2 SGE_CameraGetViewSize() {
        const auto& view = g_engine.camera.GetView();
        const auto& viewSize = view.getSize();
        return { viewSize.x, viewSize.y };
    }

    SGE_Bool SGE_TimerCreate(SGE_UUID* timer) {
        const auto uuid = g_engine.NewUUID();
        g_engine.WithTimers([&](TimerMap& timers) {
            timers[uuid] = SteadyTimer{};
            *timer = uuid;
        });
        return SGE_True;
    }

    SGE_Bool SGE_TimerReset(SGE_UUID timer, SGE_USec* elapsed) {
        SGE_Bool rc = SGE_False;
        g_engine.WithTimers([&](TimerMap& timers) {
            auto timerIter = timers.find(timer);
            if (timerIter != timers.end()) {
                *elapsed = timerIter->second.Reset();
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_TimerRemove(SGE_UUID timer) {
        SGE_Bool rc = SGE_False;
        g_engine.WithTimers([&](TimerMap& timers) {
            auto timerIter = timers.find(timer);
            if (timerIter != timers.end()) {
                timers.erase(timerIter);
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_SoundCreate(SGE_UUID* sound, const char* sourceFile) {
        SGE_Bool rc = SGE_False;
        // TODO...
        return rc;
    }
    
    SGE_Bool SGE_AnimationCreate(SGE_UUID* animation,
                                 const char* sourceFile,
                                 SGE_IVec2 start,
                                 SGE_IVec2 frameSize,
                                 SGE_Vec2 origin) {
        SGE_Bool rc = SGE_False;
        auto req = std::make_shared<TextureRequest>(sourceFile);
        g_engine.EnqueueTextureRequest(req);
        g_engine.WithAnimations([&](AnimationMap& animations) {
            const auto uuid = g_engine.NewUUID();
            animations[uuid] = {
                *req->GetResult(), IntRect {
                    start.x, start.y, frameSize.x, frameSize.y
                }, {
                    origin.x, origin.y
                }
            };
            *animation = uuid;
            rc = SGE_True;
        });
        return rc;
    }

    SGE_Bool SGE_CollisionTest() {
        SGE_Bool ret = SGE_False;
        // TODO: CollisionTest should run a collision check, and
        // push a collision event to the queue for each collision.
        return ret;
    }

    SGE_Bool SGE_PollEvents(SGE_EventHolder* event) {
        SGE_Bool ret = SGE_False;
        g_engine.WithEvents([&ret, event](EventQueue& eventsQueue) {
            if (!eventsQueue.empty()) {
                *event = eventsQueue.front();
                eventsQueue.pop();
                ret = SGE_True;
            }
        });
        return ret;
    }

    void SGE_Microsleep(SGE_USec sleepTime) {
        std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
    }
    
    const char* SGE_ResourcePath() {
        return ResourcePath().c_str();
    }

    void SGE_ConfigureResourcePath(const char* path) {
        ConfigureResourcePath(path);
    }
    
    const char* SGE_GetError() {
        return g_engine.PollError();
    }
}
