#include "Utility.hpp"

Vec2 AbsoluteTransform(const Vec2& viewCenter,
                       const Vec2& viewSize,
                       const Vec2& pos) {
    return {
        pos.x + viewCenter.x - viewSize.x / 2.f,
        pos.y + viewCenter.y - viewSize.y / 2.f
    };
}
