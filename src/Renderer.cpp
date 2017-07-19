#include "Renderer.hpp"
#include "GraphicsComponent.hpp"
#include "Animation.hpp"
#include "Entity.hpp"
#include "Camera.hpp"
#include "Utility.hpp"

Renderer::Renderer(sf::RenderWindow& window, Camera& camera) :
    m_windowRef(window),
    m_cameraRef(camera) {}

void Renderer::Visit(Entity& entity, AnimationComponent& comp) {
    auto keyframe = comp.GetAnimation()->GetKeyframe(comp.GetKeyframe());
    if (entity.HasAttribute(SGE_Attr_PositionAbsolute)) {
        keyframe.setPosition(AbsoluteTransform(m_cameraRef.GetView(),
                                               entity.GetPosition()));
    } else {
        keyframe.setPosition(entity.GetPosition());
    }
    keyframe.setColor(comp.GetColor());
    keyframe.setScale(comp.GetScale());
    m_drawList.push_back({
        keyframe, comp.GetRenderStates(), comp.GetZOrder()
    });
}

void Renderer::Display() {
    std::sort(m_drawList.begin(), m_drawList.end(),
              [](const RenderTask& lhs, const RenderTask& rhs) {
                  return lhs.zOrder < rhs.zOrder;
              });
    for (auto& element : m_drawList) {
        m_windowRef.draw(element.sprite, element.renderStates);
    }
    m_drawList.clear();
}
