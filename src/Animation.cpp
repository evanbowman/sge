#include "Animation.hpp"

Animation::Animation() {}

Animation::Animation(const sf::Texture& texture,
                     const Rect& keyframeInfo,
                     const Vec2& origin) :
    m_sprite(texture),
    m_keyframeInfo(keyframeInfo) {
    m_sprite.setOrigin(origin);
}

const sf::Sprite& Animation::GetKeyframe(size_t keyframe) {
    m_sprite.setTextureRect(sf::IntRect(
            m_keyframeInfo.left + keyframe * m_keyframeInfo.width,
            m_keyframeInfo.top, m_keyframeInfo.width, m_keyframeInfo.height));
    return m_sprite;
}
