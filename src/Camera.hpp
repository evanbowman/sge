#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

#include "Entity.hpp"

class Camera {
public:
    void SetTarget(std::shared_ptr<Entity> target);
    void Update(USec dt);
    void SetSpringiness(float springiness);
    const sf::View& GetView() const;
    
private:
    std::weak_ptr<Entity> m_target;
    float m_springiness;
    sf::View m_view;
};
