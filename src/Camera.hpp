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
    const sf::View& GetView() const;
    void SetCenter(const Vec2& center);
    void SetZoom(float zoom);
    
private:
    sf::RenderWindow& m_window;
    std::weak_ptr<Entity> m_target;
    sf::View m_view;
    float m_springiness;
};
