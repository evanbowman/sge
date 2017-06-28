#pragma once

#include <SFML/Graphics.hpp>

class SpriteComponent;
class AnimationComponent;
class Entity;

class Renderer {
public:
    Renderer(sf::RenderWindow& window);

    void Visit(Entity& entity, SpriteComponent& comp);
    void Visit(Entity& entity, AnimationComponent& comp);
    
private:
    sf::RenderWindow& m_windowRef;
};
