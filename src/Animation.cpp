#include "Animation.hpp"

Animation::Animation() {}

Animation::Animation(const sf::Texture& texture,
                     const Rect& keyframeInfo) :
    m_sprite(texture),
    m_keyframeInfo(keyframeInfo) {}
