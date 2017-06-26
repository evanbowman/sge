#include "Entity.hpp"

Entity::Entity() :
    m_bounds{}, m_position{}, m_animation{nullptr},
    m_keyframe(0) {}

const Vec2& Entity::GetPosition() const {
    return m_position;
}

void Entity::SetPosition(const Vec2& position) {
    m_position = position;
}

Animation* Entity::GetAnimation() {
    return m_animation;
}

void Entity::SetAnimation(Animation* animation) {
    m_animation = animation;
}

size_t Entity::GetKeyframe() const {
    return m_keyframe;
}

void Entity::SetKeyframe(size_t keyframe) {
    m_keyframe = keyframe;
}
