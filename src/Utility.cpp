#include "Utility.hpp"

Vec2 AbsoluteTransform(const sf::View& view, const Vec2& pos) {
    const Vec2& viewCenter = view.getCenter();
    const Vec2& viewSize = view.getSize();
    return {
        pos.x + viewCenter.x - viewSize.x / 2.f,
        pos.y + viewCenter.y - viewSize.y / 2.f
    };
}
