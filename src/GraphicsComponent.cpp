#include "GraphicsComponent.hpp"
#include "Renderer.hpp"

GraphicsComponent::GraphicsComponent() : m_zOrder(0),
                                         m_scale(1.f, 1.f) {}

void GraphicsComponent::SetZOrder(ZOrderIndex zOrder) {
    m_zOrder = zOrder;
}

ZOrderIndex GraphicsComponent::GetZOrder() const {
    return m_zOrder;
}

void GraphicsComponent::SetScale(const Vec2& scale) {
    m_scale = scale;
}

const Vec2& GraphicsComponent::GetScale() const {
    return m_scale;
}

void SpriteComponent::Display(Entity& entity, Renderer& renderer) {
    renderer.Visit(entity, *this);
}

void AnimationComponent::Display(Entity& entity, Renderer& renderer) {
    renderer.Visit(entity, *this);
}

Sprite* SpriteComponent::GetSprite() {
    return m_sprite;
}

GraphicsComponent::Id SpriteComponent::TypeId() const {
    return Id::SpriteComponent;
}

void AnimationComponent::SetKeyframe(size_t keyframe) {
    m_keyframe = keyframe;
}

size_t AnimationComponent::GetKeyframe() const {
    return m_keyframe;
}

Animation* AnimationComponent::GetAnimation() {
    return m_animation;
}

GraphicsComponent::Id AnimationComponent::TypeId() const {
    return Id::AnimationComponent;
}

AnimationComponent::AnimationComponent(Animation* animation) :
    m_keyframe(0), m_animation(animation) {}

SpriteComponent::SpriteComponent(Sprite* sprite) :
    m_sprite(sprite) {}

