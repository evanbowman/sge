#include <iostream>
#include <thread>

#include "Engine.hpp"
#include "SchemeInterface.hpp"

Engine::Engine() : m_window(sf::VideoMode(640, 480),
                            "BE",
                            sf::Style::Default),
                   m_uriCounters{} {
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
        EventLoop();
        m_window.clear();
        m_window.display();
    }
}

bool Engine::IsRunning() const {
    return m_window.isOpen();
}

URI Engine::CreateEntity() {
    std::lock_guard<std::mutex> lock(m_entitiesMtx);
    m_entities[m_uriCounters.entityCount] = std::make_shared<Entity>();
    return m_uriCounters.entityCount++;
}

URI Engine::CreateTimer() {
    m_timers[m_uriCounters.timerCount] = SteadyTimer{};
    return m_uriCounters.timerCount++;
}

void Engine::RemoveEntity(URI id) {
    std::lock_guard<std::mutex> lock(m_entitiesMtx);
    auto mapIter = m_entities.find(id);
    if (mapIter != m_entities.end()) {
        m_entities.erase(mapIter);
    }
}

Entity* Engine::GetEntityRaw(URI id) {
    const auto entity = m_entities.find(id);
    if (entity != m_entities.end()) {
        return entity->second.get();
    }
    return nullptr;
}

USec Engine::ResetTimer(URI id) {
    if (id < m_timers.size()) {
        return m_timers[id].Reset();
    }
    return 0;
}
