#include "Wall.hpp"

static const float width{32.f};
static const float height{26.f};

Wall::Wall(const Vec2& position) : Entity({
        Vec2{ position.x, position.y},
        Vec2{ position.x + width, position.y + height}
    }) {}

void Wall::SendTo(Renderer& renderer) const {
    /* Walls typically don't have a visible representation,
     * but an interface for displaying them might be useful
     * for debugging purposes. */
    renderer.Visit(*this);
}

void Wall::SendTo(Logic& logic) {
    logic.Visit(*this);
}

const Vec2& Wall::GetPosition() {
    return m_bounds.Min();
}
