#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

#include "Entity.hpp"

class Camera {
public:
    Camera();
    void Init(const sf::RenderWindow& window);
    void SetTarget(std::shared_ptr<Entity> target);
    void Update(USec dt, sf::RenderWindow& window);
    void SetSpringiness(float springiness);
    const sf::View& GetView() const;
    void SetCenter(const Vec2& center);
    void SetZoom(float zoom, const UIVec2& window);
    FloatRect GetBounds() const;
    
private:
    std::weak_ptr<Entity> m_target;
    sf::View m_view;
    float m_springiness;
};
