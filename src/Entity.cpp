#include "Entity.hpp"

Entity::Entity() : m_enabled(true) {}

const Vec2& Entity::GetPosition() const {
    return m_position;
}

void Entity::SetPosition(const Vec2& position) {
    m_position = position;
}

void Entity::SetGraphicsComponent(GraphicsCompRef gfxComp) {
    m_graphicsComponent = std::move(gfxComp);
}

GraphicsComponent* Entity::GetGraphicsComponent() {
    return m_graphicsComponent.get();
}

void Entity::SetEnabled(bool enabled) {
    m_enabled = enabled;
}

bool Entity::IsEnabled() const {
    return m_enabled;
}
