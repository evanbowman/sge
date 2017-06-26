#pragma once

#include <SFML/Graphics.hpp>

#include "Types.hpp"

class Animation {
public:
    Animation();
    
    Animation(const sf::Texture& texture,
              const Rect& keyframeInfo,
              const Vec2& origin);

    const sf::Sprite& GetKeyframe(size_t keyframe);

private:
    sf::Sprite m_sprite;
    Rect m_keyframeInfo;
};
