#include <thread>

#include "Engine.hpp"
#include "SchemeInterface.hpp"
#include "Exceptions.hpp"

Engine::Engine() : m_window(sf::VideoMode::getDesktopMode(),
                            "Engine",
                            sf::Style::Fullscreen),
                   m_renderer(m_window),
                   m_camera(m_window),
                   m_uidCounters{} {
    m_window.setMouseCursorVisible(false);
    m_window.setVerticalSyncEnabled(true);
}

void Engine::EventLoop() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            m_window.close();
            break;
        }
    }
}

/* Note: EnqueueTextureRequest is called by an auxillary thread
   in order to request Texture loading, which must happen on the main
   thread. */
void Engine::EnqueueTextureRequest(std::shared_ptr<TextureLoadRequest> req) {
    std::lock_guard<std::mutex> lk(m_textureRequestMtx);
    m_textureRequests.push_back(req);
}

void Engine::HandleTextureRequests() {
    std::lock_guard<std::mutex> lk(m_textureRequestMtx);
    for (auto & req : m_textureRequests) {
        m_textureMgr.Accept(*req);
    }
    m_textureRequests.clear();
}

void Engine::Run(RunMode mode) {
    std::thread logicThread([mode] {
        if (mode == RunMode::Interactive) {
            scheme::REPLEntry();
        } else {
            scheme::ScriptEntry();
        }
    });
    // Note: the guile interpreter never returns
    logicThread.detach();
    SteadyTimer gfxDeltaTimer;
    while (m_window.isOpen()) {
        HandleTextureRequests();
        EventLoop();
        m_camera.Update(gfxDeltaTimer.Reset());
        for (auto& entityNode : m_entities) {
            auto& entity = entityNode.second;
            if (entity->IsEnabled()) {
                if (auto gfx = entity->GetGraphicsComponent()) {
                    gfx->Dispatch(*entity.get(), m_renderer);
                }      
            }
        }
        m_renderer.Display();
    }
}

bool Engine::IsRunning() const {
    return m_window.isOpen();
}

UID Engine::CreateEntity() {
    std::lock_guard<std::mutex> lock(m_entitiesMtx);
    m_entities[m_uidCounters.entityCount] = std::make_shared<Entity>();
    return m_uidCounters.entityCount++;
}

UID Engine::CreateTimer() {
    m_timers[m_uidCounters.timerCount] = SteadyTimer{};
    return m_uidCounters.timerCount++;
}

UID Engine::CreateAnimation(std::string sourceFile, const Rect& frameDesc,
                            const Vec2& origin) {
    auto req = std::make_shared<TextureLoadRequest>(sourceFile);
    EnqueueTextureRequest(req);
    m_animations[m_uidCounters.animationCount] = {
        *req->GetResult(), frameDesc, origin
    };
    return m_uidCounters.animationCount++;
}

void Engine::SetEntityAnimation(UID entity, UID animation) {
    auto entityIter = FindEntity(entity);
    auto animationIter = m_animations.find(animation);
    if (animationIter == m_animations.end()) {
        throw BadHandle(animation);
    }
    entityIter->second->SetGraphicsComponent({
            std::make_unique<AnimationComponent>(&animationIter->second)
        });
}

void Engine::SetEntityKeyframe(UID entity, size_t keyframe) {
    auto& gfxComp = FindGfxComp(entity);
    if (gfxComp.TypeId() != GraphicsComponent::Id::AnimationComponent) {
        throw "FIXME";
    }
    reinterpret_cast<AnimationComponent&>(gfxComp).SetKeyframe(keyframe);
}

void Engine::SetEntityPosition(UID entity, const Vec2& position) {
    auto entityIter = FindEntity(entity);
    entityIter->second->SetPosition(position);
}

void Engine::SetEntityScale(UID entity, const Vec2& scale) {
    FindGfxComp(entity).SetScale(scale);
}

void Engine::SetEntityBlendMode(UID entity, const sf::BlendMode& blendMode) {
    FindGfxComp(entity).GetRenderStates().blendMode = blendMode;
}

void Engine::SetEntityZOrder(UID entity, ZOrderIndex zOrder) {
    FindGfxComp(entity).SetZOrder(zOrder);
}

void Engine::SetCameraTarget(UID entity) {
    auto entityIter = FindEntity(entity);
    m_camera.SetTarget(entityIter->second);
}

void Engine::SetCameraSpringiness(float springiness) {
    m_camera.SetSpringiness(springiness);
}

const Vec2& Engine::GetEntityPosition(UID entity) {
    auto entityIter = FindEntity(entity);
    return entityIter->second->GetPosition();
}

size_t Engine::GetEntityKeyframe(UID entity) {
    auto entityIter = FindEntity(entity);
    auto gfxConf = entityIter->second->GetGraphicsComponent();
    if (!gfxConf) {
        throw "FIXME";
    }
    if (gfxConf->TypeId() != GraphicsComponent::Id::AnimationComponent) {
        throw "FIXME";
    }
    return reinterpret_cast<AnimationComponent*>(gfxConf)->GetKeyframe();
}

void Engine::RemoveEntity(UID entity) {
    std::lock_guard<std::mutex> lock(m_entitiesMtx);
    auto mapIter = m_entities.find(entity);
    if (mapIter != m_entities.end()) {
        m_entities.erase(mapIter);
    }
}

USec Engine::ResetTimer(UID timer) {
    const auto timerIter = m_timers.find(timer);
    if (timerIter != m_timers.end()) {
        return timerIter->second.Reset();
    }
    return 0;
}

void Engine::RemoveTimer(UID timer) {
    auto mapIter = m_timers.find(timer);
    if (mapIter != m_timers.end()) {
        m_timers.erase(mapIter);
    }
}

Engine::EntityMap::iterator Engine::FindEntity(UID entity) {
    auto entityIter = m_entities.find(entity);
    if (entityIter == m_entities.end()) {
        throw BadHandle(entity);
    }
    return entityIter;
}

GraphicsComponent& Engine::FindGfxComp(UID entity) {
    auto entityIter = FindEntity(entity);
    if (auto gfxConf = entityIter->second->GetGraphicsComponent()) {
        return *gfxConf;
    } else {
        throw "Entity missing graphics component";
    }
}
