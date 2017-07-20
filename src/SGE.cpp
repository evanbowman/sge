#include "SGE.h"

#include <atomic>
#include <array>
#include <SFML/Graphics.hpp>
#include <list>
#include <unordered_map>
#include <thread>
#include <iostream>
#include <queue>

#include "Animation.hpp"
#include "Camera.hpp"
#include "CollisionChecker.hpp"
#include "Entity.hpp"
#include "PooledHashmap.hpp"
#include "Renderer.hpp"
#include "Singleton.hpp"
#include "SchemeInterface.hpp"
#include "Sync.hpp"
#include "Types.hpp"
#include "Timer.hpp"
#include "TextureManager.hpp"
#include "Utility.hpp"

namespace errors {
    static const auto badEntityHandle = "Bad entity handle";
    static const auto badAnimationHandle = "Bad animation handle";
    static const auto gfxCompNotAnim = "Gfx component is not an animation";
    static const auto missingGfxComp = "Entity has no graphics component";
}

using EntityMap = std::unordered_map<SGE_UUID, EntityRef>;
using TimerMap = std::unordered_map<SGE_UUID, SteadyTimer>;
using AnimationMap = std::unordered_map<SGE_UUID, Animation>;

// TODO: Needs synchronization: Timers, Camera

struct Engine {

    Engine() : recordEvents(false),
               running(false),
               window(sf::VideoMode::getDesktopMode(),
                      "SGE",
                      sf::Style::Fullscreen),
               camera(window),
               renderer(window, camera),
               m_uidCounter(0) {
        window.setMouseCursorVisible(false);
        window.setVerticalSyncEnabled(true);
    }

    void RecordEvent(const SGE_EventHolder& holder) {
        events.Get([&holder](std::queue<SGE_EventHolder>& eventsQueue) {
            eventsQueue.push(holder);
        });
    }
    
    void EventLoop() {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::TextEntered:
                if (recordEvents) {
                    SGE_EventHolder holder;
                    holder.event.text.unicode = event.text.unicode;
                    holder.code = SGE_EventCode_TextEntered;
                    RecordEvent(holder);
                }
                break;

            case sf::Event::KeyPressed:
                if (recordEvents) {
                    SGE_EventHolder holder;
                    holder.event.key.keyCode = event.key.code;
                    holder.event.key.altPressed = (SGE_Bool)event.key.alt;
                    holder.event.key.controlPressed = (SGE_Bool)event.key.control;
                    holder.event.key.shiftPressed = (SGE_Bool)event.key.shift;
                    holder.event.key.systemPressed = (SGE_Bool)event.key.system;
                    holder.code = SGE_EventCode_KeyPressed;
                    RecordEvent(holder);
                }
                break;

            case sf::Event::KeyReleased:
                if (recordEvents) {
                    SGE_EventHolder holder;
                    holder.event.key.keyCode = event.key.code;
                    holder.event.key.altPressed = (SGE_Bool)event.key.alt;
                    holder.event.key.controlPressed = (SGE_Bool)event.key.control;
                    holder.event.key.shiftPressed = (SGE_Bool)event.key.shift;
                    holder.event.key.systemPressed = (SGE_Bool)event.key.system;
                    holder.code = SGE_EventCode_KeyReleased;
                    RecordEvent(holder);
                }
                break;
            }
        }
    }

    void EnqueueTextureRequest(std::shared_ptr<TextureRequest> req) {
        std::lock_guard<std::mutex> lk(textureReqMtx);
        textureRequests.push_back(req);
    }

    void HandleTextureRequests() {
        std::lock_guard<std::mutex> lk(textureReqMtx);
        for (auto & req : textureRequests) {
            textureMgr.Accept(*req);
        }
        textureRequests.clear();
    }

    void Run() {
        running = true;
        std::thread logicThread([] {
            scheme::Start();
        });
        SteadyTimer gfxDeltaTimer;
        while (window.isOpen()) {
            HandleTextureRequests();
            EventLoop();
            camera.Update(gfxDeltaTimer.Reset());
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
            renderer.Display();
            window.display();
            if (!running) {
                window.close();
            }
        }
        logicThread.join();
    }

    bool recordEvents;
    std::atomic<bool> running;
    sf::RenderWindow window;
    Camera camera;
    Renderer renderer;
    CollisionChecker collisionChecker;
    sf::Color refreshColor;
    TextureManager textureMgr;
    std::mutex textureReqMtx;
    std::vector<std::shared_ptr<TextureRequest>> textureRequests;
    TimerMap timers;
    Sync<std::queue<SGE_EventHolder>> events;

    // Note: shallow copy only. Pass a statically allocated string.
    void PushError(const char* err) {
        std::lock_guard<std::mutex> lock(m_errorsMtx);
        m_errors.push_back(err);
    }

    const char* PollError() {
        std::lock_guard<std::mutex> lock(m_errorsMtx);
        const auto lastError = m_errors.back();
        m_errors.pop_back();
        return lastError;
    }

    void WithEntities(std::function<void(EntityMap&)> procedure) {
        m_entities.Get(procedure);
    }

    void WithAnimations(std::function<void(AnimationMap&)> procedure) {
        m_animations.Get(procedure);
    }

    SGE_UUID NewUUID() { return ++m_uidCounter; }

private:
    Sync<EntityMap> m_entities;
    Sync<AnimationMap> m_animations;
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
    SGE_Bool SGE_IsRunning() {
        return static_cast<SGE_Bool>(g_engine.window.isOpen());
    }

    void SGE_SetRefreshColor(SGE_Color color) {
        g_engine.refreshColor = sf::Color(color.r, color.g, color.b, color.a);
    }

    SGE_UIVec2 SGE_GetWindowSize() {
        const auto& windowSize = g_engine.window.getSize();
        return {
            windowSize.x, windowSize.y
        };
    }

    SGE_Bool SGE_CreateEntity(SGE_UUID* entity) {
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

    SGE_Bool SGE_CloneEntity(SGE_UUID src, SGE_UUID* dest) {
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
                    gfxCompClone = std::unique_ptr<GraphicsComponent>(srcGfxComp->Clone());
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

    SGE_Bool SGE_AddEntityAttribute(SGE_UUID entity, SGE_Attribute attrib) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, attrib](EntityMap& entities) {
            if (auto foundEntity = FindEntity(entities, entity)) {
                foundEntity->AddAttribute(attrib);
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_RemoveEntityAttribute(SGE_UUID entity, SGE_Attribute attrib) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, attrib](EntityMap& entities) {
            if (auto foundEntity = FindEntity(entities, entity)) {
                foundEntity->RemoveAttribute(attrib);
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_SetEntityAnimation(SGE_UUID entity, SGE_UUID animation) {
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

    SGE_Bool SGE_SetEntityKeyframe(SGE_UUID entity, SGE_Keyframe keyframe) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, keyframe](EntityMap& entities) {
            if (auto gfxComp = FindGfxComp(entities, entity)) {
                if (gfxComp->TypeId() !=
                    GraphicsComponent::Id::AnimationComponent) {
                    g_engine.PushError(errors::gfxCompNotAnim);
                    return;
                }
                reinterpret_cast<AnimationComponent*>(gfxComp)->SetKeyframe(keyframe);
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_SetEntityPosition(SGE_UUID entity, SGE_Vec2 pos) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, &pos](EntityMap& entities) {
            if (auto foundEntity = FindEntity(entities, entity)) {
                foundEntity->SetPosition({ pos.x, pos.y });
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_SetEntityScale(SGE_UUID entity, SGE_Vec2 scale) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, &scale](EntityMap& entities) {
            if (auto gfxComp = FindGfxComp(entities, entity)) {
                gfxComp->SetScale({ scale.x, scale.y });
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_SetEntityBlendMode(SGE_UUID entity, SGE_BlendMode mode) {
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
            }
        });
        return rc;
    }

    SGE_Bool SGE_SetEntityZOrder(SGE_UUID entity, int zOrder) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, zOrder](EntityMap& entities) {
            if (auto gfxComp = FindGfxComp(entities, entity)) {
                gfxComp->SetZOrder(zOrder);
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_SetEntityColor(SGE_UUID entity, SGE_Color color) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity, &color](EntityMap& entities) {
            if (auto gfxComp = FindGfxComp(entities, entity)) {
                gfxComp->SetColor({ color.r, color.g, color.b, color.a });
                rc = SGE_True;
            }
        });
        return rc;
    }

    SGE_Bool SGE_GetEntityPosition(SGE_UUID entity, SGE_Vec2* position) {
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

    SGE_Bool SGE_GetEntityKeyframe(SGE_UUID entity, SGE_Keyframe* keyframe) {
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

    SGE_Bool SGE_RemoveEntity(SGE_UUID entity) {
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

    SGE_Bool SGE_SetCameraTarget(SGE_UUID entity) {
        SGE_Bool rc = SGE_False;
        g_engine.WithEntities([&rc, entity](EntityMap& entities) {
            if (auto entityRef = FindEntityRef(entities, entity)) {
                g_engine.camera.SetTarget(entityRef);
                rc = SGE_True;
            }
        });
        return rc;
    }

    void SGE_SetCameraCenter(SGE_Vec2 center) {
        g_engine.camera.SetCenter({ center.x, center.y });
    }

    void SGE_SetCameraSpringiness(float springiness) {
        g_engine.camera.SetSpringiness(springiness);
    }

    void SGE_SetCameraZoom(float zoom) {
        g_engine.camera.SetZoom(zoom);
    }

    SGE_Vec2 SGE_GetCameraViewSize() {
        const auto& view = g_engine.camera.GetView();
        const auto& viewSize = view.getSize();
        return { viewSize.x, viewSize.y };
    }

    SGE_Bool SGE_CreateTimer(SGE_UUID* timer) {
        const auto uuid = g_engine.NewUUID();
        g_engine.timers[uuid] = SteadyTimer{};
        *timer = uuid;
        return SGE_True;
    }

    SGE_Bool SGE_ResetTimer(SGE_UUID timer, SGE_USec* elapsed) {
        auto timerIter = g_engine.timers.find(timer);
        if (timerIter != g_engine.timers.end()) {
            *elapsed = timerIter->second.Reset();
            return SGE_True;
        }
        return SGE_False;
    }

    SGE_Bool SGE_RemoveTimer(SGE_UUID timer) {
        auto timerIter = g_engine.timers.find(timer);
        if (timerIter != g_engine.timers.end()) {
            g_engine.timers.erase(timerIter);
            return SGE_True;
        }
        return SGE_False;
    }

    SGE_Bool SGE_CreateAnimation(SGE_UUID* animation,
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
                *req->GetResult(), Rect {
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

    SGE_Bool SGE_PollEvents(SGE_EventHolder* event) {
        SGE_Bool ret = SGE_False;
        g_engine.events.Get([&ret, event](std::queue<SGE_EventHolder>& eventsQueue) {
            if (!eventsQueue.empty()) {
                *event = eventsQueue.front();
                eventsQueue.pop();
                ret = SGE_True;
            }
        });
        return ret;
    }

    void SGE_RecordEvents(SGE_Bool enabled) {
        g_engine.recordEvents = enabled;
    }

    const char* SGE_GetError() {
        return g_engine.PollError();
    }

    void SGE_Exit() {
        g_engine.running = false;
    }
}

// #include "cxxopts/cxxopts.hpp"
#include "ResourcePath.hpp"

int main(int argc, char** argv) {
    // cxxopts::Options options("sge", "Simple 2d Game Engine");
    // options.add_options()
    //     ("p,package", "Run on a specific directory",
    //      cxxopts::value<std::string>())
    //     ("h,help", "display this message");
    // try {
    //     options.parse(argc, argv);
    // } catch (const std::exception& ex) {
    //     std::cerr << ex.what() << std::endl;
    //     return EXIT_FAILURE;
    // }
    // if (options.count("p")) {
    //     ConfigureResourcePath(options["p"].as<std::string>());
    // }
    ConfigureResourcePath("/Users/evan_bowman/Documents/platformer/");
    Singleton<Engine>::Instance().Run();
    return EXIT_SUCCESS;
}


