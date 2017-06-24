#include "BB.hpp"

AABB::AABB() {}

AABB::AABB(const Vec2& min, const Vec2& max) :
    m_min(min), m_max(max) {}

bool AABB::Intersects(const AABB& other) {
    // TODO...
    return false;
}

const Vec2& AABB::Min() const {
    return m_min;
}

const Vec2& AABB::Max() const {
    return m_max;
}
