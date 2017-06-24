#pragma once

#include "Types.hpp"

class AABB {
public:
    AABB();
    AABB(const Vec2& min, const Vec2& max);
    
    bool Intersects(const AABB& other);
    const Vec2& Min() const;
    const Vec2& Max() const;
    
private:
    Vec2 m_min, m_max;
};
