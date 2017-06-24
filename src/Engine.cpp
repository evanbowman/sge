#include <iostream>
#include <thread>

#include "Engine.hpp"
#include "SchemeInterface.hpp"

Engine::Engine() : m_window(sf::VideoMode(640, 480),
                            "BE",
                            sf::Style::Default) {}

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

URI Engine::CreateEntity() {
    std::lock_guard<std::mutex> lock(m_entitiesMtx);
    m_entities[++m_uriCounter] = {};
    return m_uriCounter;
}

bool Engine::IsRunning() const {
    return m_window.isOpen();
}

URI Engine::CreateTimer() {
    m_timers.emplace_back();
    return m_timers.size() - 1;
}

USec Engine::ResetTimer(URI timer) {
    if (timer < m_timers.size()) {
        return m_timers[timer].Reset();
    }
    return 0;
}
