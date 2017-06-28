#include <thread>

#include "Engine.hpp"
#include "SchemeInterface.hpp"

Engine::Engine() : m_window(sf::VideoMode(480, 540),
                            "Engine",
                            sf::Style::Default),
                   m_renderer(m_window),
                   m_uidCounters{} {
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
    while (m_window.isOpen()) {
        HandleTextureRequests();
        EventLoop();
        m_window.clear();
        for (auto& entity : m_entities) {
            if (auto gfx = entity.second->GetGraphicsComponent()) {
                gfx->Display(*entity.second.get(), m_renderer);
            }
        }
        m_window.display();
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
    auto entityIter = FindEntityById(entity);
    auto animationIter = m_animations.find(animation);
    if (animationIter == m_animations.end()) {
        throw "FIXME";
    }
    entityIter->second->SetGraphicsComponent({
            std::make_unique<AnimationComponent>(&animationIter->second)
        });
}

void Engine::SetEntityKeyframe(UID entity, size_t keyframe) {
    auto entityIter = FindEntityById(entity);
    auto gfxConf = entityIter->second->GetGraphicsComponent();
    if (!gfxConf) {
        throw "FIXME";
    }
    if (gfxConf->TypeId() != GraphicsComponent::Id::AnimationComponent) {
        throw "FIXME";
    }
    reinterpret_cast<AnimationComponent*>(gfxConf)->SetKeyframe(keyframe);
}

void Engine::SetEntityPosition(UID entity, const Vec2& position) {
    auto entityIter = FindEntityById(entity);
    entityIter->second->SetPosition(position);
}

const Vec2& Engine::GetEntityPosition(UID entity) {
    auto entityIter = FindEntityById(entity);
    return entityIter->second->GetPosition();
}

size_t Engine::GetEntityKeyframe(UID entity) {
    auto entityIter = FindEntityById(entity);
    auto gfxConf = entityIter->second->GetGraphicsComponent();
    if (!gfxConf) {
        throw "FIXME";
    }
    if (gfxConf->TypeId() != GraphicsComponent::Id::AnimationComponent) {
        throw "FIXME";
    }
    return reinterpret_cast<AnimationComponent*>(gfxConf)->GetKeyframe();
}

void Engine::RemoveEntity(UID id) {
    std::lock_guard<std::mutex> lock(m_entitiesMtx);
    auto mapIter = m_entities.find(id);
    if (mapIter != m_entities.end()) {
        m_entities.erase(mapIter);
    }
}

USec Engine::ResetTimer(UID id) {
    const auto timer = m_timers.find(id);
    if (timer != m_timers.end()) {
        return timer->second.Reset();
    }
    return 0;
}

void Engine::RemoveTimer(UID id) {
    auto mapIter = m_timers.find(id);
    if (mapIter != m_timers.end()) {
        m_timers.erase(mapIter);
    }
}

Engine::EntityMap::iterator Engine::FindEntityById(UID id) {
    auto entityIter = m_entities.find(id);
    if (entityIter == m_entities.end()) {
        throw "FIXME";
    }
    return entityIter;
}
