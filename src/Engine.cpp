#include <thread>

#include "Engine.hpp"
#include "SchemeInterface.hpp"

Engine::Engine() : m_window(sf::VideoMode(480, 540),
                            "Engine",
                            sf::Style::Default),
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
            if (auto animation = entity.second->GetAnimation()) {
                auto keyframe =
                    animation->GetKeyframe(entity.second->GetKeyframe());
                keyframe.setPosition(entity.second->GetPosition());
                m_window.draw(keyframe);
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

UID Engine::CreateRNG() {
    std::random_device rd;
    m_rngs[m_uidCounters.rngCount] = std::mt19937(rd());
    return m_uidCounters.rngCount++;
}

void Engine::RemoveEntity(UID id) {
    std::lock_guard<std::mutex> lock(m_entitiesMtx);
    auto mapIter = m_entities.find(id);
    if (mapIter != m_entities.end()) {
        m_entities.erase(mapIter);
    }
}

Entity* Engine::GetEntityRaw(UID id) {
    const auto entity = m_entities.find(id);
    if (entity != m_entities.end()) {
        return entity->second.get();
    }
    return nullptr;
}

Animation* Engine::GetAnimationRaw(UID id) {
    const auto animation = m_animations.find(id);
    if (animation != m_animations.end()) {
        return &animation->second;
    }
    return nullptr;
}

USec Engine::ResetTimer(UID id) {
    const auto timer = m_timers.find(id);
    if (timer != m_timers.end()) {
        return timer->second.Reset();
    }
    return 0;
}

std::mt19937::result_type Engine::GetRandom(UID id) {
    const auto generator = m_rngs.find(id);
    if (generator != m_rngs.end()) {
        return generator->second();
    }
    return 0;
}

void Engine::RemoveTimer(UID id) {
    auto mapIter = m_timers.find(id);
    if (mapIter != m_timers.end()) {
        m_timers.erase(mapIter);
    }
}
