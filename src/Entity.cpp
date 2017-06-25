#include "Entity.hpp"

const Vec2& Entity::GetPosition() const {
    return m_position;
}

void Entity::SetPosition(const Vec2& position) {
    m_position = position;
}
