#include "Renderer.hpp"
#include "GraphicsComponent.hpp"
#include "Animation.hpp"
#include "Entity.hpp"

Renderer::Renderer(sf::RenderWindow& window) : m_windowRef(window) {}

void Renderer::Visit(Entity& entity, AnimationComponent& comp) {
    auto keyframe = comp.GetAnimation()->GetKeyframe(comp.GetKeyframe());
    keyframe.setPosition(entity.GetPosition());
    keyframe.setColor(comp.GetColor());
    keyframe.setScale(comp.GetScale());
    m_drawList.push_back({
        keyframe, comp.GetRenderStates(), comp.GetZOrder()
    });
}

void Renderer::Display() {
    std::sort(m_drawList.begin(), m_drawList.end(),
              [](const auto& lhs, const auto& rhs) {
                  return lhs.zOrder < rhs.zOrder;
              });
    for (auto& element : m_drawList) {
        m_windowRef.draw(element.sprite, element.renderStates);
    }
    m_drawList.clear();
}
