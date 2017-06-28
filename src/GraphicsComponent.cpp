#include "GraphicsComponent.hpp"
#include "Renderer.hpp"

void SpriteComponent::Display(Entity& entity, Renderer& renderer) {
    renderer.Visit(entity, *this);
}

void AnimationComponent::Display(Entity& entity, Renderer& renderer) {
    renderer.Visit(entity, *this);
}

Sprite* SpriteComponent::GetSprite() {
    return m_sprite;
}

GraphicsComponent::Id SpriteComponent::TypeId() {
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

GraphicsComponent::Id AnimationComponent::TypeId() {
    return Id::AnimationComponent;
}

AnimationComponent::AnimationComponent(Animation* animation) :
    m_keyframe(0), m_animation(animation) {}

SpriteComponent::SpriteComponent(Sprite* sprite) :
    m_sprite(sprite) {}
