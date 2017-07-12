#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "Types.hpp"

class AnimationComponent;
class Entity;

class Renderer {
public:
    Renderer(sf::RenderWindow& window);

    void Visit(Entity& entity, AnimationComponent& comp);

    void Display();
    
private:
    struct RenderTask {
        sf::Sprite sprite;
        sf::RenderStates renderStates;
        ZOrderIndex zOrder;
    };
    sf::RenderWindow& m_windowRef;
    std::vector<RenderTask> m_drawList;
};
