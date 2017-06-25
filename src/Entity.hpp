#pragma once

#include "BB.hpp"
#include "Types.hpp"

class Entity {
public:
    const Vec2& GetPosition() const;
    void SetPosition(const Vec2& position);
    
private:
    AABB m_bounds;
    Vec2 m_position;
};
