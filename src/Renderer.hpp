#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <list>

#include "Types.hpp"

class AnimationComponent;
class Camera;
class Entity;

class Renderer {
public:
    void Configure(const Camera& camera);
    
    void Visit(Entity& entity, AnimationComponent& comp);

    void Display(sf::RenderWindow& window);
    
private:
    struct RenderTask {
        sf::Sprite sprite;
        sf::RenderStates renderStates;
        ZOrderIndex zOrder;
    };
    
    struct {
        FloatRect cameraViewBounds;
        Vec2 cameraViewCenter;
        Vec2 cameraViewSize;
    } m_config;

    std::vector<RenderTask> m_displayList;
};
