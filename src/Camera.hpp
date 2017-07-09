#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

#include "Entity.hpp"

class Camera {
public:
    Camera(sf::RenderWindow& window);
    void SetTarget(std::shared_ptr<Entity> target);
    void Update(USec dt);
    void SetSpringiness(float springiness);
    
private:
    std::weak_ptr<Entity> m_target;
    float m_springiness;
    sf::RenderWindow& m_windowRef;
};
