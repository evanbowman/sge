#include "Entity.hpp"

const AABB& Entity::GetBounds() const {
    return m_bounds;
}

Entity::Entity() {}

Entity::Entity(const AABB& bounds) : m_bounds(bounds) {}
