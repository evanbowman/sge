#include "SGE.h"

#include <array>
#include <SFML/Graphics.hpp>
#include <list>
#include <unordered_map>
#include <thread>
#include <iostream>

#include "Animation.hpp"
#include "Camera.hpp"
#include "CollisionChecker.hpp"
#include "Entity.hpp"
#include "Renderer.hpp"
#include "Singleton.hpp"
#include "SchemeInterface.hpp"
#include "Types.hpp"
#include "Timer.hpp"
#include "TextureManager.hpp"

namespace errors {
    static const auto badEntityHandle = "Bad entity handle";
    static const auto badAnimationHandle = "Bad animation handle";
    static const auto gfxCompNotAnim = "Gfx component is not an animation";
}

// Note: the Engine has public access so that the C API
// can modify its internals, but the struct definition
// and instantation are private to this source file.
struct Engine {
    using EntityMap = std::unordered_map<SGE_UUID, EntityRef>;
    using TimerMap = std::unordered_map<SGE_UUID, SteadyTimer>;
    using AnimationMap = std::unordered_map<SGE_UUID, Animation>;

    Engine() : recordEvents(false),
               window(sf::VideoMode::getDesktopMode(),
                      "SGE",
                      sf::Style::Fullscreen),
               camera(window),
               renderer(window, camera),
               uidCounter(0) {
        window.setMouseCursorVisible(false);
        window.setVerticalSyncEnabled(true);
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
                    holder.event.textEntered.unicode = event.text.unicode;
                    holder.code = SGE_EventCode_TextEntered;
                    events.push_back(holder);
                }
                break;

            case sf::Event::KeyPressed:
                if (recordEvents) {
                    SGE_EventHolder holder;
                    holder.event.keyPressed.key = event.key.code;
                    holder.code = SGE_EventCode_KeyPressed;
                    events.push_back(holder);
                }
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
        std::thread logicThread([] {
            scheme::Start();
        });
        SteadyTimer gfxDeltaTimer;    
        while (window.isOpen()) {
            HandleTextureRequests();
            EventLoop();
            camera.Update(gfxDeltaTimer.Reset());
            for (auto& entityNode : entities) {
                auto& entity = entityNode.second;
                if (!entity->HasAttribute(SGE_Attr_Hidden)) {
                    if (auto gfx = entity->GetGraphicsComponent()) {
                        gfx->Dispatch(*entity.get(), renderer);
                    }      
                }
            }
            window.clear(refreshColor);
            renderer.Display();
            window.display();
        }
        logicThread.join();
    }

    GraphicsComponent* FindGfxComp(SGE_UUID entity) {
        auto foundEntity = FindEntity(entity);
        if (!foundEntity) {
            return nullptr;
        }
        if (auto gfxComp = foundEntity->GetGraphicsComponent()) {
            return gfxComp;
        }
        return nullptr;
    }

    Entity* FindEntity(SGE_UUID entity) {
        auto entityIter = entities.find(entity);
        if (entityIter == entities.end()) {
            errors.push_back(errors::badEntityHandle);
            return nullptr;
        }
        return entityIter->second.get();
    }

    EntityRef FindEntityRef(SGE_UUID entity) {
        auto entityIter = entities.find(entity);
        if (entityIter == entities.end()) {
            errors.push_back(errors::badEntityHandle);
            return nullptr;
        }
        return entityIter->second;
    }

    bool recordEvents;
    sf::RenderWindow window;
    Camera camera;
    Renderer renderer;
    SGE_UUID uidCounter;
    CollisionChecker collisionChecker;
    sf::Color refreshColor;
    std::mutex entitiesMtx;
    TextureManager textureMgr;
    std::mutex textureReqMtx;
    std::vector<std::shared_ptr<TextureRequest>> textureRequests;
    EntityMap entities;
    TimerMap timers;
    AnimationMap animations;
    std::vector<const char*> errors;
    std::vector<SGE_EventHolder> events;
};

auto& g_engine = Singleton<Engine>::Instance();

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
        std::lock_guard<std::mutex> lock(g_engine.entitiesMtx);
        auto created = std::make_shared<Entity>();
        if (created) {
            g_engine.uidCounter++;
            g_engine.entities[g_engine.uidCounter] = created;
            *entity = g_engine.uidCounter;
            return SGE_True;
        }
        return SGE_False;
    }

    SGE_Bool SGE_CloneEntity(SGE_UUID src, SGE_UUID* dest) {
        std::lock_guard<std::mutex> lock(g_engine.entitiesMtx);
        if (auto foundSrc = g_engine.FindEntity(src)) {
            auto clone = std::make_shared<Entity>();
            if (!clone) {
                return SGE_False;
            }
            clone->SetAttributeSet(foundSrc->GetAttributeSet());
            clone->SetPosition(foundSrc->GetPosition());
            std::unique_ptr<GraphicsComponent> gfxCompClone;
            if (auto srcGfxComp = foundSrc->GetGraphicsComponent()) {
                gfxCompClone = std::unique_ptr<GraphicsComponent>(srcGfxComp->Clone());
                if (!gfxCompClone) {
                    return SGE_False;
                }
            }
            clone->SetGraphicsComponent(std::move(gfxCompClone));
            g_engine.uidCounter++;
            g_engine.entities[g_engine.uidCounter] = clone;
            *dest = g_engine.uidCounter;
            return SGE_True;
        }
        return SGE_False;
    }
    
    SGE_Bool SGE_AddEntityAttribute(SGE_UUID entity, SGE_Attribute attrib) {
        if (auto foundEntity = g_engine.FindEntity(entity)) {
            foundEntity->AddAttribute(attrib);
            return SGE_True;
        }
        return SGE_False;
    }

    SGE_Bool SGE_RemoveEntityAttribute(SGE_UUID entity, SGE_Attribute attrib) {
        if (auto foundEntity = g_engine.FindEntity(entity)) {
            foundEntity->RemoveAttribute(attrib);
            return SGE_True;
        }
        return SGE_False;
    }
    
    SGE_Bool SGE_SetEntityAnimation(SGE_UUID entity, SGE_UUID animation) {
        auto foundEntity = g_engine.FindEntity(entity);
        if (!foundEntity) {
            return SGE_False;
        }
        auto animationIter = g_engine.animations.find(animation);
        if (animationIter == g_engine.animations.end()) {
            g_engine.errors.push_back(errors::badAnimationHandle);
            return SGE_False;
        }
        foundEntity->SetGraphicsComponent({
                std::make_unique<AnimationComponent>(&animationIter->second)
            });
        return SGE_True;
    }
    
    SGE_Bool SGE_SetEntityKeyframe(SGE_UUID entity, SGE_Keyframe keyframe) {
        if (auto gfxComp = g_engine.FindGfxComp(entity)) {
            if (gfxComp->TypeId() !=
                GraphicsComponent::Id::AnimationComponent) {
                g_engine.errors.push_back(errors::gfxCompNotAnim);
                return SGE_False;
            }
            reinterpret_cast<AnimationComponent*>(gfxComp)->SetKeyframe(keyframe);
            return SGE_True;
        }
        return SGE_False;
    }
    
    SGE_Bool SGE_SetEntityPosition(SGE_UUID entity, SGE_Vec2 pos) {
        if (auto foundEntity = g_engine.FindEntity(entity)) {
            foundEntity->SetPosition({ pos.x, pos.y });
            return SGE_True;
        }
        return SGE_False;
    }
    
    SGE_Bool SGE_SetEntityScale(SGE_UUID entity, SGE_Vec2 scale) {
        if (auto gfxComp = g_engine.FindGfxComp(entity)) {
            gfxComp->SetScale({ scale.x, scale.y });
            return SGE_True;
        }
        return SGE_False;
    }
    
    SGE_Bool SGE_SetEntityBlendMode(SGE_UUID entity, SGE_BlendMode mode) {
        static const std::array<sf::BlendMode, 4> modes {{
            sf::BlendNone, sf::BlendAdd, sf::BlendAlpha, sf::BlendMultiply
        }};
        if (mode >= 0 && mode < SGE_BM_Count) {
            if (auto gfxComp = g_engine.FindGfxComp(entity)) {
                gfxComp->GetRenderStates().blendMode = modes[mode];
                return SGE_True;
            }
            return SGE_False;
        }
        return SGE_False;
    }
    
    SGE_Bool SGE_SetEntityZOrder(SGE_UUID entity, int zOrder) {
        if (auto gfxComp = g_engine.FindGfxComp(entity)) {
            gfxComp->SetZOrder(zOrder);
            return SGE_True;
        }
        return SGE_False;
    }
    
    SGE_Bool SGE_SetEntityColor(SGE_UUID entity, SGE_Color color) {
        if (auto gfxComp = g_engine.FindGfxComp(entity)) {
            gfxComp->SetColor({ color.r, color.g, color.b, color.a });
            return SGE_True;
        }
        return SGE_False;
    }
    
    SGE_Bool SGE_GetEntityPosition(SGE_UUID entity, SGE_Vec2* position) {
        if (auto foundEntity = g_engine.FindEntity(entity)) {
            const auto& entityPos = foundEntity->GetPosition();
            *position = { entityPos.x, entityPos.y };
            return SGE_True;
        }
        return SGE_False;
    }
    
    SGE_Bool SGE_GetEntityKeyframe(SGE_UUID entity, SGE_Keyframe* keyframe) {
        if (auto gfxComp = g_engine.FindGfxComp(entity)) {
            if (gfxComp->TypeId() ==
                GraphicsComponent::Id::AnimationComponent) {
                *keyframe = reinterpret_cast<AnimationComponent*>(gfxComp)
                    ->GetKeyframe();
                return SGE_True;
            }
            return SGE_False;
        }
        return SGE_False;
    }
    
    SGE_Bool SGE_RemoveEntity(SGE_UUID entity) {
        std::lock_guard<std::mutex> lock(g_engine.entitiesMtx);
        auto entityIter = g_engine.entities.find(entity);
        if (entityIter != g_engine.entities.end()) {
            g_engine.entities.erase(entityIter);
            return SGE_True;
        }
        g_engine.errors.push_back(errors::badEntityHandle);
        return SGE_False;
    }
    
    SGE_Bool SGE_SetCameraTarget(SGE_UUID entity) {
        if (auto entityRef = g_engine.FindEntityRef(entity)) {
            g_engine.camera.SetTarget(entityRef);
            return SGE_True;
        }
        return SGE_False;
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
        g_engine.uidCounter++;
        g_engine.timers[g_engine.uidCounter] = SteadyTimer{};
        *timer = g_engine.uidCounter;
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
        auto req = std::make_shared<TextureRequest>(sourceFile);
        g_engine.EnqueueTextureRequest(req);
        g_engine.uidCounter++;
        g_engine.animations[g_engine.uidCounter] = {
            *req->GetResult(), Rect {
                start.x, start.y, frameSize.x, frameSize.y
            }, {
                origin.x, origin.y
            }
        };
        *animation = g_engine.uidCounter;
        return SGE_True;
    }

    SGE_Bool SGE_PollEvents(SGE_EventHolder* event) {
        if (!g_engine.events.empty()) {
            *event = g_engine.events.back();
            g_engine.events.pop_back();
            return SGE_True;
        }
        return SGE_False;
    }

    void SGE_RecordEvents(SGE_Bool enabled) {
        g_engine.recordEvents = enabled;
    }

    const char* SGE_GetError() {
        const auto prevError = g_engine.errors.back();
        g_engine.errors.pop_back();
        return prevError;
    }
}

#include "cxxopts/cxxopts.hpp"
#include "ResourcePath.hpp"

int main(int argc, char** argv) {
    cxxopts::Options options("sge", "Simple 2d Game Engine");
    options.add_options()    
        ("p,package", "Run on a specific directory",
         cxxopts::value<std::string>())
        ("h,help", "display this message");
    try {
        options.parse(argc, argv);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    if (options.count("p")) {
        ConfigureResourcePath(options["p"].as<std::string>());
    }
    Singleton<Engine>::Instance().Run();
    return EXIT_SUCCESS;
}


