#pragma once

#include <SFML/Graphics.hpp>

#include "Types.hpp"

class Animation {
public:
    Animation();
    
    Animation(const sf::Texture& texture,
              const Rect& keyframeInfo);

private:
    sf::Sprite m_sprite;
    Rect m_keyframeInfo;
};
