#pragma once

#include "Types.hpp"
#include "Renderer.hpp"
#include "Logic.hpp"
#include "BB.hpp"

class Entity {
public:
    Entity();
    Entity(const AABB& bounds);
    
    virtual void SendTo(Logic& logic) = 0;
    virtual void SendTo(Renderer& renderer) const = 0;
    virtual ~Entity() {};

    const AABB& GetBounds() const;
protected:
    AABB m_bounds;
};
