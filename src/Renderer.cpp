#include "Renderer.hpp"
#include "GraphicsComponent.hpp"
#include "Animation.hpp"
#include "Entity.hpp"

Renderer::Renderer(sf::RenderWindow& window) : m_windowRef(window) {}

void Renderer::Visit(Entity& entity, SpriteComponent& comp) {
    // TODO
}

#include <iostream>

void Renderer::Visit(Entity& entity, AnimationComponent& comp) {
    auto keyframe = comp.GetAnimation()->GetKeyframe(comp.GetKeyframe());
    keyframe.setPosition(entity.GetPosition());
    m_windowRef.draw(keyframe);
}
