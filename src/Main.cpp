#include <list>
#include <memory>

#include "Entity.hpp"
#include "Wall.hpp"
#include "RendererImpl.hpp"
#include "LogicImpl.hpp"
#include "Timer.hpp"
#include "Player.hpp"
#include "Logic.hpp"

class Game {
public:
    Game() : m_window(sf::VideoMode::getDesktopMode(),
                      "BlindJump",
                      sf::Style::Fullscreen),
             m_renderer(new RendererImpl(m_window)),
             m_logic(new LogicImpl()){}

    void GraphicsStep() {
        for (auto & entity : m_entities) {
            entity->SendTo(*m_renderer);
        }
        if (auto hero = m_player.GetCharacter()) {
            hero->SendTo(*m_renderer);
        }
    }

    void LogicStep(USec dt) {
        m_logic->SetDT(dt);
        for (auto & entity : m_entities) {
            entity->SendTo(*m_logic);
        }
        if (auto hero = m_player.GetCharacter()) {
            hero->SendTo(*m_logic);
        }
    }

    void EventLoop() {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                m_window.close();
                break;
            }
        }
    }
    
    void Run() {
        Timer<std::chrono::steady_clock> timer;
        while (m_window.isOpen()) {
            m_window.clear();
            EventLoop();
            LogicStep(timer.Reset());
            GraphicsStep();
            m_window.display();
        }
    }
    
private:
    std::list<std::shared_ptr<Entity>> m_entities;
    sf::RenderWindow m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Logic> m_logic;
    Player m_player;
};

int main() {
    Game game;
    game.Run();
}
