#pragma once

#include "BB.hpp"
#include "Types.hpp"
#include "Animation.hpp"

class Entity {
public:
    Entity();
    
    const Vec2& GetPosition() const;
    void SetPosition(const Vec2& position);
    Animation* GetAnimation();
    void SetAnimation(Animation* animation);
    size_t GetKeyframe() const;
    void SetKeyframe(size_t keyframe);
    
private:
    AABB m_bounds;
    Vec2 m_position;
    Animation* m_animation;
    size_t m_keyframe;
};
